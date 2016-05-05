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
#include "backend/planner/projection_plan.h"

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
    (void) op;
    return nullptr;
  }

  void visit(const ExprVariable *) override {
  }

  void visit(const ExprConstant *) override {
  }

  void visit(const ExprCompare *) override {
  }

  void visit(const ExprBoolOp *) override {
  }

  void visit(const ExprOp *) override {
  }

 private:

};

class OpToPlanTransformer : public OperatorVisitor {
 public:
  OpToPlanTransformer() {
  }

  planner::AbstractPlan *ConvertOpExpression(
    std::shared_ptr<OpExpression> plan)
  {
    plan->Op().accept(this);
    return output_plan;
  }

  void visit(const PhysicalScan *) override {
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
