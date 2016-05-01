//===----------------------------------------------------------------------===//
//
//                         Peloton
//
// optimizer.h
//
// Identification: src/backend/optimizer/optimizer.h
//
// Copyright (c) 2015-16, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include "backend/optimizer/memo.h"
#include "backend/optimizer/column_manager.h"
#include "backend/optimizer/query_operators.h"
#include "backend/optimizer/operator_node.h"
#include "backend/optimizer/binding.h"
#include "backend/optimizer/pattern.h"
#include "backend/optimizer/property.h"
#include "backend/optimizer/rule.h"
#include "backend/planner/abstract_plan.h"
#include "backend/common/logger.h"

#include <memory>

namespace peloton {
namespace optimizer {

class BindingIterator;
class GroupBindingIterator;
//===--------------------------------------------------------------------===//
// Optimizer
//===--------------------------------------------------------------------===//
class Optimizer {
  friend class BindingIterator;
  friend class GroupBindingIterator;
 public:
  Optimizer(const Optimizer &) = delete;
  Optimizer &operator=(const Optimizer &) = delete;
  Optimizer(Optimizer &&) = delete;
  Optimizer &operator=(Optimizer &&) = delete;

  Optimizer() {};

  static Optimizer &GetInstance();

  std::shared_ptr<planner::AbstractPlan> GeneratePlan(
    std::shared_ptr<Select> select_tree);

 private:
  /* OptimizerPlanToPlannerPlan - convert a tree of physical operators to
   * a planner plan for execution.
   *
   * plan: an optimizer plan composed soley of physical operators
   * return: the corresponding planner plan
   */
  planner::AbstractPlan *OptimizerPlanToPlannerPlan(OpExpression plan);

  /* TransformQueryTree - create an initial operator tree for the given query
   * to be used in performing optimization.
   *
   * tree: a peloton query tree representing a select query
   * return: the group id of the root of the tree
   */
  GroupID InsertQueryTree(std::shared_ptr<Select> tree);

  /* GetQueryTreeRequiredProperties - get the required physical properties for
   * a peloton query tree.
   *
   * tree: a peloton query tree representing a select query
   * return: the set of required physical properties for the query
   */
  std::vector<Property> GetQueryTreeRequiredProperties(
    std::shared_ptr<Select> tree);

  /* Optimize - produce the best plan for the given operator tree which has the
   * specified physical requirements
   *
   * root: a group to optimize
   * requirements: the set of requirements the returned operato tree
   *               must have
   * return: the best physical operator tree for the given group
   */
  OpExpression OptimizeGroup(GroupID id,
                       std::vector<Property> requirements);

  /*
   *
   */
  OpExpression OptimizeItem(GroupID id,
                      size_t item_index,
                      std::vector<Property> requirements);

  /* Explore - check the operator tree root for the given pattern
   *
   * root: an operator tree representing a query
   * pattern: an operator tree representing a query
   * return: the best physical plan which represents the given operator tree
   */
  void ExploreGroup(GroupID id, const Rule &rule);

  /* Explore - check the operator tree root for the given pattern
   *
   * root: an operator tree representing a query
   * pattern: an operator tree representing a query
   * return: the best physical plan which represents the given operator tree
   */
  void ExploreItem(GroupID id,
                   size_t item_index,
                   const Rule &rule);

  /* TransformOperator - apply the given rule to the operator tree root to
   * generate a logically equivalent operator tree or a physical implementation
   * of the operator depending on the type of rule
   *
   * root: an operator tree
   * rule: the rule to transform the given operator tree
   * return: a new operator tree after rule application
   */
  Operator TransformOperator(Operator root, const Rule &rule);

  Memo memo;
  ColumnManager column_manager;
  std::vector<Rule> rules;
};

} /* namespace optimizer */
} /* namespace peloton */
