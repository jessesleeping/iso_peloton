//===----------------------------------------------------------------------===//
//
//                         Peloton
//
// convert_query_to_op.cpp
//
// Identification: src/backend/optimizer/convert_query_to_op.cpp
//
// Copyright (c) 2015-16, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "backend/optimizer/convert_query_to_op.h"
#include "backend/optimizer/query_node_visitor.h"
#include "backend/optimizer/operators.h"

#include "backend/planner/projection_plan.h"
#include "backend/planner/seq_scan_plan.h"
#include "backend/planner/order_by_plan.h"

#include "backend/catalog/manager.h"
#include "backend/bridge/ddl/bridge.h"
#include "backend/bridge/dml/mapper/mapper.h"

namespace peloton {
namespace optimizer {

class QueryToOpTransformer : public QueryNodeVisitor {
 public:
  QueryToOpTransformer(ColumnManager &manager)
    : manager(manager) {
  }

  std::shared_ptr<OpExpression> ConvertToOpExpression(
    std::shared_ptr<Select> op)
  {
    op->accept(this);
    return output_expr;
  }

  void visit(const Variable *op) override {
    storage::DataTable *data_table = static_cast<storage::DataTable *>(
      catalog::Manager::GetInstance().GetTableWithOid(
        bridge::Bridge::GetCurrentDatabaseOid(), op->base_table_oid));
    catalog::Column schema_col = op->column;

    oid_t table_oid = data_table->GetOid();
    oid_t col_id = schema_col.GetOffset();
    assert(col_id != INVALID_OID);

    Column *col = manager.LookupColumn(table_oid, col_id);
    if (col == nullptr) {
      col = manager.AddBaseColumn(schema_col.GetType(),
                                  schema_col.GetLength(),
                                  schema_col.GetName(),
                                  schema_col.IsInlined(),
                                  table_oid,
                                  col_id);
    }

    output_expr = std::make_shared<OpExpression>(ExprVariable::make(col));
  }

  void visit(const Constant *op) override {
    output_expr = std::make_shared<OpExpression>(ExprConstant::make(op->value));
  }

  void visit(const OperatorExpression *op) override {
    auto expr = std::make_shared<OpExpression>(ExprOp::make(op->type));

    for (QueryExpression *arg : op->args) {
      arg->accept(this);
      expr->PushChild(output_expr);
    }

    output_expr = expr;
  }

  void visit(const AndOperator *op) override {
    auto expr =
      std::make_shared<OpExpression>(ExprBoolOp::make(BoolOpType::And));

    for (QueryExpression *arg : op->args) {
      arg->accept(this);
      expr->PushChild(output_expr);
    }

    output_expr = expr;
  }

  void visit(const OrOperator *op) override {
    auto expr =
      std::make_shared<OpExpression>(ExprBoolOp::make(BoolOpType::Or));

    for (QueryExpression *arg : op->args) {
      arg->accept(this);
      expr->PushChild(output_expr);
    }

    output_expr = expr;
  }

  void visit(const NotOperator *op) override {
    auto expr =
      std::make_shared<OpExpression>(ExprBoolOp::make(BoolOpType::Not));

    QueryExpression *arg = op->arg;
    arg->accept(this);
    expr->PushChild(output_expr);

    output_expr = expr;
  }

  void visit(const Attribute *op) override {
    op->expression->accept(this);

    // TODO(abpoms): actually figure out what the type should be by deriving
    // it from the expression tree
    Column *col =
      manager.AddExprColumn(VALUE_TYPE_NULL, 0, op->name, true);
    auto expr = std::make_shared<OpExpression>(ExprProjectColumn::make(col));
    expr->PushChild(output_expr);

    output_expr = expr;
  }

  void visit(const Table *op) override {
    std::vector<Column *> columns;

    storage::DataTable *table = op->data_table;
    catalog::Schema *schema = table->GetSchema();
    oid_t table_oid = table->GetOid();
    for (oid_t col_id = 0; col_id < schema->GetColumnCount(); col_id++) {
      catalog::Column schema_col = schema->GetColumn(col_id);
      Column *col = manager.LookupColumn(table_oid, col_id);
      if (col == nullptr) {
        col = manager.AddBaseColumn(schema_col.GetType(),
                                    schema_col.GetLength(),
                                    schema_col.GetName(),
                                    schema_col.IsInlined(),
                                    table_oid,
                                    col_id);
      }
      columns.push_back(col);
    }
    output_expr =
      std::make_shared<OpExpression>(LogicalGet::make(op->data_table, columns));
  }

  void visit(const Join *op) override {
    // Self
    std::shared_ptr<OpExpression> expr;
    switch (op->join_type) {
    case JOIN_TYPE_INNER: {
      expr = std::make_shared<OpExpression>(LogicalInnerJoin::make());
    } break;
    case JOIN_TYPE_LEFT: {
      expr = std::make_shared<OpExpression>(LogicalLeftJoin::make());
    } break;
    case JOIN_TYPE_RIGHT: {
      expr = std::make_shared<OpExpression>(LogicalRightJoin::make());
    } break;
    case JOIN_TYPE_OUTER: {
      expr = std::make_shared<OpExpression>(LogicalOuterJoin::make());
    } break;
    default:
      assert(false);
    }

    // Left child
    op->left_node->accept(this);
    expr->PushChild(output_expr);

    // Right child
    op->right_node->accept(this);
    expr->PushChild(output_expr);

    // Join condition predicate
    op->predicate->accept(this);
    expr->PushChild(output_expr);

    output_expr = expr;
  }

  void visit(const OrderBy *op) override {
    (void) op;
  }

  void visit(const Select *op) override {
    // Add join tree op expression
    op->join_tree->accept(this);
    std::shared_ptr<OpExpression> expr = output_expr;

    // Add filter for where predicate
    if (op->where_predicate) {
      op->where_predicate->accept(this);
      output_expr->PushChild(expr);
      expr = output_expr;
    }

    // Add all attributes in output list as projection at top level
    auto project_expr = std::make_shared<OpExpression>(LogicalProject::make());
    project_expr->PushChild(expr);
    auto project_list = std::make_shared<OpExpression>(ExprProjectList::make());
    project_expr->PushChild(project_list);
    for (Attribute *attr : op->output_list) {
      // Ignore intermediate columns for output projection
      if (!attr->intermediate) {
        attr->accept(this);
        project_list->PushChild(output_expr);
      }
    }

    output_expr = project_expr;
  }

 private:
  ColumnManager &manager;

  std::shared_ptr<OpExpression> output_expr;
};

std::shared_ptr<OpExpression> ConvertQueryToOpExpression(
  ColumnManager &manager,
  std::shared_ptr<Select> tree)
{
  QueryToOpTransformer converter(manager);
  return converter.ConvertToOpExpression(tree);
}


} /* namespace optimizer */
} /* namespace peloton */
