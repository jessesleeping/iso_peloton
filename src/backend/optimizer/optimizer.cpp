//===----------------------------------------------------------------------===//
//
//                         Peloton
//
// optimizer.cpp
//
// Identification: src/backend/optimizer/optimizer.cpp
//
// Copyright (c) 2015-16, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "backend/optimizer/optimizer.h"
#include "backend/optimizer/operator_visitor.h"
#include "backend/optimizer/query_node_visitor.h"

#include "backend/planner/projection_plan.h"
#include "backend/planner/seq_scan_plan.h"
#include "backend/planner/order_by_plan.h"

#include "backend/catalog/manager.h"
#include "backend/bridge/ddl/bridge.h"
#include "backend/bridge/dml/mapper/mapper.h"

#include <memory>

namespace peloton {
namespace optimizer {

namespace {

class QueryTreeConverter : public QueryNodeVisitor {
 public:
  QueryTreeConverter(ColumnManager &manager)
    : manager(manager) {
  }

  std::shared_ptr<OpExpression> ConvertToOpExpression(const Select *op) {
    op->accept(this);
    return output_expr;
  }

  void visit(const Variable *op) override {
    storage::DataTable *data_table = static_cast<storage::DataTable *>(
      catalog::Manager::GetInstance().GetTableWithOid(
        bridge::Bridge::GetCurrentDatabaseOid(), op->base_table_oid));
    catalog::Column schema_col = op->column;

    Column *col = manager.LookupColumn(data_table->GetOid(),
                                       schema_col.GetOffset());
    if (col == nullptr) {
      col = manager.AddColumn(schema_col.GetName(),
                              data_table->GetOid(),
                              schema_col.GetOffset(),
                              schema_col.GetType());
    }

    output_expr = std::make_shared<OpExpression>(ExprVariable::make(col));
  }

  void visit(const Constant *op) override {
    (void) op;
  }

  void visit(const OperatorExpression *op) override {
    (void) op;
  }

  void visit(const AndOperator *op) override {
    (void) op;
  }

  void visit(const OrOperator *op) override {
    (void) op;
  }

  void visit(const NotOperator *op) override {
    (void) op;
  }

  void visit(const Attribute *op) override {
    (void) op;
  }

  void visit(const Table *op) override {
    std::vector<Column *> columns;

    storage::DataTable *table = op->data_table;
    catalog::Schema *schema = table->GetSchema();
    oid_t table_oid = table->GetOid();
    for (oid_t column_id = 0;
         column_id < schema->GetColumnCount();
         column_id++)
    {
      catalog::Column schema_col = schema->GetColumn(column_id);
      Column *col = manager.LookupColumn(table_oid, column_id);
      if (col == nullptr) {
        col = manager.AddColumn(schema_col.GetName(),
                                table_oid,
                                column_id,
                                schema_col.GetType());
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
    for (Attribute *attr : op->output_list) {
      (void) attr;
    }

    output_expr = expr;
  }

 private:
  ColumnManager &manager;

  std::shared_ptr<OpExpression> output_expr;
};

}

//===--------------------------------------------------------------------===//
// Optimizer
//===--------------------------------------------------------------------===//
Optimizer &Optimizer::GetInstance()
{
  thread_local static Optimizer optimizer;
  return optimizer;
}

std::shared_ptr<planner::AbstractPlan> Optimizer::GeneratePlan(
  std::shared_ptr<Select> select_tree)
{
  return nullptr;
  // Generate initial operator tree from query tree
  GroupID root_id = InsertQueryTree(select_tree);

  // Get the physical properties the final plan must output
  PropertySet properties =
    GetQueryTreeRequiredProperties(select_tree);

  // Find least cost plan for root group
  OptimizeGroup(root_id, properties);
  OpExpression optimizer_plan(LogicalInnerJoin::make());

  planner::AbstractPlan* top_plan = OptimizerPlanToPlannerPlan(optimizer_plan);

  std::shared_ptr<planner::AbstractPlan> final_plan(
    top_plan, bridge::PlanTransformer::CleanPlan);

  return final_plan;
}

GroupID Optimizer::InsertQueryTree(std::shared_ptr<Select> tree) {
  (void) tree;
  return 0;
}

PropertySet Optimizer::GetQueryTreeRequiredProperties(
  std::shared_ptr<Select> tree)
{
  (void)tree;
  return PropertySet();
}

planner::AbstractPlan *Optimizer::OptimizerPlanToPlannerPlan(
  OpExpression plan)
{
  (void)plan;
  return nullptr;
}

void Optimizer::OptimizeGroup(GroupID id, PropertySet requirements) {
  Group *group = memo.GetGroupByID(id);
  const std::vector<std::shared_ptr<GroupExpression>> exprs =
    group->GetExpressions();
  for (size_t i = 0; i < exprs.size(); ++i) {
    OptimizeExpression(exprs[i], requirements);
  }

  // Choose best from group
}

void Optimizer::OptimizeExpression(std::shared_ptr<GroupExpression> gexpr,
                                   PropertySet requirements)
{
  // Apply all rules to operator which match. We apply all rules to one operator
  // before moving on to the next operator in the group because then we avoid
  // missing the application of a rule e.g. an application of some rule creates
  // a match for a previously applied rule, but it is missed because the prev
  // rule was already checked
  for (const Rule &rule : rules) {
    ExploreExpression(gexpr, rule);
  }

  // Apply physical transformations and cost those which match the requirements
  for (const Rule &rule : rules) {
    if (!rule.IsPhysical()) continue;

    std::vector<std::shared_ptr<GroupExpression>> candidates =
      TransformExpression(gexpr, rule);

    for (std::shared_ptr<GroupExpression> candidate : candidates) {
      // Ignore if it does not fulfill requirements
    }
  }
  (void)requirements;
}

void Optimizer::ExploreGroup(GroupID id, const Rule &rule) {
  // for (Operator op : group.GetOperators()) {
  //   ExploreItem(op, rule);
  // }
  (void) id;
  (void) rule;
}

void Optimizer::ExploreExpression(std::shared_ptr<GroupExpression> gexpr,
                                  const Rule &rule)
{
  if (!rule.IsLogical()) return;

  (void) TransformExpression(gexpr, rule);
}


//////////////////////////////////////////////////////////////////////////////
/// Rule application
std::vector<std::shared_ptr<GroupExpression>>
Optimizer::TransformExpression(std::shared_ptr<GroupExpression> gexpr,
                                    const Rule &rule)
{
  std::shared_ptr<Pattern> pattern = rule.GetMatchPattern();

  std::vector<std::shared_ptr<GroupExpression>> output_plans;
  ItemBindingIterator iterator(*this, gexpr, pattern);
  while (iterator.HasNext()) {
    std::shared_ptr<OpExpression> plan = iterator.Next();
    // Check rule condition function
    if (rule.Check(plan)) {
      // Apply rule transformations
      // We need to be able to analyze the transformations performed by this
      // rule in order to perform deduplication and launch an exploration of
      // the newly applied rule
      std::vector<std::shared_ptr<OpExpression>> transformed_plans;
      rule.Transform(plan, transformed_plans);

      // Integrate transformed plans back into groups and explore/cost if new
      for (std::shared_ptr<OpExpression> plan : transformed_plans) {
        std::shared_ptr<GroupExpression> gexpr;
        bool new_expression = RecordTransformedExpression(plan, gexpr);
        // Should keep exploring this new expression with the current rule
        // but because we do exhaustive search anyway right now we do not
        // gain much from doing so, so don't
        if (new_expression) {
          output_plans.push_back(gexpr);
        }
      }
    }
  }
  return output_plans;
}

//////////////////////////////////////////////////////////////////////////////
/// Memo insertion
std::shared_ptr<GroupExpression> Optimizer::MakeGroupExpression(
  std::shared_ptr<OpExpression> expr)
{
  std::vector<GroupID> child_groups = MemoTransformedChildren(expr);
  return std::make_shared<GroupExpression>(expr->Op(), child_groups);
}

std::vector<GroupID> Optimizer::MemoTransformedChildren(
  std::shared_ptr<OpExpression> expr)
{
  std::vector<GroupID> child_groups;
  for (std::shared_ptr<OpExpression> child : expr->Children()) {
    child_groups.push_back(MemoTransformedExpression(child));
  }

  return child_groups;
}

GroupID Optimizer::MemoTransformedExpression(
  std::shared_ptr<OpExpression> expr)
{
  std::shared_ptr<GroupExpression> gexpr = MakeGroupExpression(expr);
  // Ignore whether this expression is new or not as we only care about that
  // at the top level
  (void)memo.InsertExpression(gexpr);
  return gexpr->GetGroupID();
}

bool Optimizer::RecordTransformedExpression(
  std::shared_ptr<OpExpression> expr,
  std::shared_ptr<GroupExpression> &gexpr)
{
  gexpr = MakeGroupExpression(expr);
  return memo.InsertExpression(gexpr);
}

}  // namespace optimizer
}  // namespace peloton
