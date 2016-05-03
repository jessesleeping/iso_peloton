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
#include "backend/optimizer/convert_query_to_op.h"

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
  std::shared_ptr<OpExpression> initial =
    ConvertQueryToOpExpression(column_manager, tree);
  std::shared_ptr<GroupExpression> gexpr;
  RecordTransformedExpression(initial, gexpr);
  return gexpr->GetGroupID();
}

PropertySet Optimizer::GetQueryTreeRequiredProperties(
  std::shared_ptr<Select> tree)
{
  (void) tree;
  return PropertySet();
}

planner::AbstractPlan *Optimizer::OptimizerPlanToPlannerPlan(
  OpExpression plan)
{
  (void) plan;
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
  (void) memo.InsertExpression(gexpr);
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
