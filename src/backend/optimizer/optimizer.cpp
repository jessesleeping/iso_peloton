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
  std::vector<Property> properties =
    GetQueryTreeRequiredProperties(select_tree);

  // Find least cost plan for root group
  OpExpression optimizer_plan = OptimizeGroup(root_id, properties);

  planner::AbstractPlan* top_plan = OptimizerPlanToPlannerPlan(optimizer_plan);

  std::shared_ptr<planner::AbstractPlan> final_plan(
    top_plan, bridge::PlanTransformer::CleanPlan);

  return final_plan;
}

planner::AbstractPlan *Optimizer::OptimizerPlanToPlannerPlan(OpExpression plan) {
  (void)plan;
  return nullptr;
}

GroupID Optimizer::InsertQueryTree(std::shared_ptr<Select> tree) {
  (void) tree;
  return 0;
}

std::vector<Property> Optimizer::GetQueryTreeRequiredProperties(
  std::shared_ptr<Select> tree)
{
  (void)tree;
  return {};
}

OpExpression Optimizer::OptimizeGroup(GroupID id,
                                    std::vector<Property> requirements)
{
  const std::vector<Operator> &items = memo.Groups()[id].GetOperators();
  for (size_t item_index = 0; item_index < items.size(); ++item_index) {
    OptimizeItem(id,
                 item_index,
                 requirements);
  }

  // Choose best from group
  return Operator();
}

OpExpression Optimizer::OptimizeItem(GroupID group_id,
                                   size_t item_index,
                                   std::vector<Property> requirements)
{
  // Apply all rules to operator which match. We apply all rules to one operator
  // before moving on to the next operator in the group because then we avoid
  // missing the application of a rule e.g. an application of some rule creates
  // a match for a previously applied rule, but it is missed because the prev
  // rule was already checked
  for (const Rule &rule : rules) {
    ExploreItem(group_id, item_index, rule);
  }
  (void)requirements;
  return Operator();
}

void Optimizer::ExploreGroup(GroupID id, const Rule &rule) {
  // for (Operator op : group.GetOperators()) {
  //   ExploreItem(op, rule);
  // }
  (void)id;
  (void)rule;
}

void Optimizer::ExploreItem(GroupID id,
                            size_t item_index,
                            const Rule &rule)
{
  std::shared_ptr<Pattern> pattern = rule.GetMatchPattern();

  ItemBindingIterator iterator(*this, id, item_index, pattern);
  while (iterator.HasNext()) {
    std::shared_ptr<OpExpression> plan = iterator.Next();
    // Check rule condition function
    if (rule.Check(plan)) {
      // Apply rule transformations
      // We need to be able to analyze the transformations performed by this
      // rule in order to perform deduplication and launch an exploration of
      // the newly applied rule
      std::vector<std::shared_ptr<OpExpression>> output_plans;
      rule.Transform(plan, output_plans);

      // Integrate transformed plans back into groups and explore/cost
    }
  }
}

}  // namespace optimizer
}  // namespace peloton
