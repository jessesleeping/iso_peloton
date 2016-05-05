//===----------------------------------------------------------------------===//
//
//                         Peloton
//
// convert_op_to_plan.cpp
//
// Identification: src/backend/optimizer/convert_op_to_plan.cpp
//
// Copyright (c) 2015-16, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "backend/optimizer/convert_op_to_plan.h"
#include "backend/optimizer/operator_visitor.h"

#include "backend/planner/hash_join_plan.h"
#include "backend/planner/seq_scan_plan.h"
#include "backend/planner/projection_plan.h"

#include "backend/expression/expression_util.h"

namespace peloton {
namespace optimizer {

namespace {

class ExprOpToAbstractExpressionTransformer : public OperatorVisitor {
 public:
  ExprOpToAbstractExpressionTransformer() {
  }

  expression::AbstractExpression *ConvertOpExpression(
    std::shared_ptr<OpExpression> op)
  {
    VisitOpExpression(op);
    return output_expr;
  }

  void visit(const ExprVariable *var) override {
    (void) var;
    // TODO(abpoms): do this right
    oid_t table_idx = 0;
    oid_t column_idx = 0;
    output_expr =
      expression::ExpressionUtil::TupleValueFactory(table_idx, column_idx);
  }

  void visit(const ExprConstant *op) override {
    output_expr =
      expression::ExpressionUtil::ConstantValueFactory(op->value);
  }

  void visit(const ExprCompare *op) override {
    auto children = current_children;
    assert(children.size() == 2);

    std::vector<expression::AbstractExpression *> child_exprs;
    for (std::shared_ptr<OpExpression> child : children) {
      VisitOpExpression(child);
      child_exprs.push_back(output_expr);
    }
    child_exprs.resize(2, nullptr);

    output_expr = expression::ExpressionUtil::ComparisonFactory(
      op->expr_type, child_exprs[0], child_exprs[1]);
  }

  void visit(const ExprBoolOp *op) override {
    auto children = current_children;

    std::list<expression::AbstractExpression *> child_exprs;
    for (std::shared_ptr<OpExpression> child : children) {
      VisitOpExpression(child);
      child_exprs.push_back(output_expr);
    }

    switch (op->bool_type) {
    case BoolOpType::Not: {
      assert(children.size() == 1);
      output_expr = expression::ExpressionUtil::OperatorFactory(
        EXPRESSION_TYPE_OPERATOR_NOT, child_exprs.front(), nullptr);
    } break;
    case BoolOpType::And: {
      assert(children.size() > 0);
      output_expr = expression::ExpressionUtil::ConjunctionFactory(
        EXPRESSION_TYPE_CONJUNCTION_AND, child_exprs);
    } break;
    case BoolOpType::Or: {
      assert(children.size() > 0);
      output_expr = expression::ExpressionUtil::ConjunctionFactory(
        EXPRESSION_TYPE_CONJUNCTION_OR, child_exprs);
    } break;
    default: {
      assert(false);
    }
    }
    assert(output_expr != nullptr);
  }

  void visit(const ExprOp *op) override {
    auto children = current_children;

    std::vector<expression::AbstractExpression *> child_exprs;
    for (std::shared_ptr<OpExpression> child : children) {
      VisitOpExpression(child);
      child_exprs.push_back(output_expr);
    }
    child_exprs.resize(4, nullptr);

    output_expr = expression::ExpressionUtil::OperatorFactory(
      op->expr_type,
      child_exprs[0],
      child_exprs[1],
      child_exprs[2],
      child_exprs[3]);
  }

 private:
  void VisitOpExpression(std::shared_ptr<OpExpression> op) {
    std::vector<std::shared_ptr<OpExpression>> prev_children = current_children;
    current_children = op->Children();
    op->Op().accept(this);
    current_children = prev_children;
  }

  expression::AbstractExpression *output_expr;
  std::vector<std::shared_ptr<OpExpression>> current_children;
};

class OpToPlanTransformer : public OperatorVisitor {
 public:
  OpToPlanTransformer() {
  }

  planner::AbstractPlan *ConvertOpExpression(
    std::shared_ptr<OpExpression> plan)
  {
    VisitOpExpression(plan);
    return output_plan;
  }

  void visit(const PhysicalScan *op) override {
    std::vector<oid_t> column_ids;
    for (Column *col : op->columns) {
      TableColumn *table_col = dynamic_cast<TableColumn *>(col);
      assert(table_col != nullptr);
      column_ids.push_back(table_col->ColumnIndexOid());
    }

    output_plan = new planner::SeqScanPlan(op->table, nullptr, column_ids);
  }

  void visit(const PhysicalComputeExprs *) override {
    auto children = current_children;
    assert(children.size() == 2);

    VisitOpExpression(children[0]);
    planner::AbstractPlan *child_plan = output_plan;

    catalog::Schema *project_schema = nullptr;
    planner::ProjectInfo::TargetList target_list;
    {
      std::vector<catalog::Column> columns;
      // Build target list for projection from the child ExprProjectList
      oid_t out_col_id = 0;
      for (std::shared_ptr<OpExpression> op_expr : children[1]->Children()) {
        assert(op_expr->Op().type() == OpType::ProjectColumn);
        assert(op_expr->Children().size() == 1);

        const ExprProjectColumn *proj_col =
          op_expr->Op().as<ExprProjectColumn>();
        Column *column = proj_col->column;;
        columns.push_back(GetSchemaColumnFromOptimizerColumn(column));

        expression::AbstractExpression *expr =
          ConvertOpExpressionToAbstractExpression(op_expr->Children()[0]);

        target_list.push_back({out_col_id, expr});
        out_col_id++;
      }
      project_schema = new catalog::Schema(columns);
    }

    // Build projection info from target list
    planner::ProjectInfo *project_info =
      new planner::ProjectInfo(std::move(target_list), {});

    output_plan = new planner::ProjectionPlan(project_info, project_schema);
    output_plan->AddChild(child_plan);
  }

  void visit(const PhysicalFilter *) override {
    auto children = current_children;
    assert(children.size() == 2);

    VisitOpExpression(children[0]);
    planner::AbstractPlan *child_plan = output_plan;

    expression::AbstractExpression *predicate =
      ConvertOpExpressionToAbstractExpression(children[1]);

    output_plan = new planner::SeqScanPlan(nullptr, predicate, {});
    output_plan->AddChild(child_plan);
  }

  void visit(const PhysicalInnerHashJoin *) override {
  }

  void visit(const PhysicalLeftHashJoin *) override {
  }

  void visit(const PhysicalRightHashJoin *) override {
  }

  void visit(const PhysicalOuterHashJoin *) override {
  }

 private:
  void VisitOpExpression(std::shared_ptr<OpExpression> op) {
    std::vector<std::shared_ptr<OpExpression>> prev_children = current_children;
    current_children = op->Children();
    op->Op().accept(this);
    current_children = prev_children;
  }

  planner::AbstractPlan *output_plan;
  std::vector<std::shared_ptr<OpExpression>> current_children;
};

}

expression::AbstractExpression *ConvertOpExpressionToAbstractExpression(
  std::shared_ptr<OpExpression> op_expr)
{
  ExprOpToAbstractExpressionTransformer transformer;
  return transformer.ConvertOpExpression(op_expr);
}

planner::AbstractPlan *ConvertOpExpressionToPlan(
  std::shared_ptr<OpExpression> plan)
{
  OpToPlanTransformer transformer;
  return transformer.ConvertOpExpression(plan);
}

} /* namespace optimizer */
} /* namespace peloton */
