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
  Operator initial_op_tree = TransformQueryTree(select_tree);

  // Find least cost plan for logical operator tree
  planner::AbstractPlan* top_plan = nullptr;

  std::shared_ptr<planner::AbstractPlan> final_plan(
    top_plan, bridge::PlanTransformer::CleanPlan);

  return final_plan;
}

Operator Optimizer::TransformQueryTree(std::shared_ptr<Select> tree) {
  (void) tree;
  return Operator();
}

Operator Optimizer::OptimizeGroup(GroupID id,
                                  const Group &group,
                                  std::vector<Property> requirements)
{
  const std::vector<Operator> &items = group.GetOperators();
  for (size_t item_index = 0; item_index < items.size(); ++item_index) {
    OptimizeItem(id,
                 item_index,
                 group.GetOperators()[item_index],
                 requirements);
  }

  // Choose best from group
  return Operator();
}

Operator Optimizer::OptimizeItem(GroupID group_id,
                                 size_t item_index,
                                 Operator item,
                                 std::vector<Property> requirements)
{
  // Apply all rules to operator which match. We apply all rules to one operator
  // before moving on to the next operator in the group because then we avoid
  // missing the application of a rule e.g. an application of some rule creates
  // a match for a previously applied rule, but it is missed because the prev
  // rule was already checked
  for (const Rule &rule : rules) {
    ExploreItem(group_id, item_index, item, rule);
  }
  (void)requirements;
  return Operator();
}

void Optimizer::ExploreGroup(GroupID id, const Group &group, const Rule &rule) {
  // for (Operator op : group.GetOperators()) {
  //   ExploreItem(op, rule);
  // }
  (void)id;
  (void)group;
  (void)rule;
}

void Optimizer::ExploreItem(GroupID id,
                            size_t item_index,
                            Operator item,
                            const Rule &rule)
{
  std::shared_ptr<Pattern> pattern = rule.GetMatchPattern();

  ItemBindingIterator iterator(groups, id, item_index, pattern);
  (void)iterator;
  (void)item;
  // for (const Binding &binding : iter) {
  //   // Check if the rule can be properly applied to this binding
  //   if (rule.Check(binding)) {
  //   }
  // }
}

}  // namespace optimizer
}  // namespace peloton
