//===----------------------------------------------------------------------===//
//
//                         Peloton
//
// group.cpp
//
// Identification: src/backend/optimizer/group.cpp
//
// Copyright (c) 2015-16, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "backend/optimizer/group.h"

namespace peloton {
namespace optimizer {

//===--------------------------------------------------------------------===//
// Group
//===--------------------------------------------------------------------===//
Group::Group(GroupID id) : id(id) {}

void Group::AddExpression(std::shared_ptr<GroupExpression> expr) {
  // Do duplicate detection
  expr->SetGroupID(id);
  expressions.push_back(expr);
}

void Group::SetExpressionCost(std::shared_ptr<GroupExpression> expr,
                              double cost,
                              PropertySet properties)
{
  (void) expr;
  (void) cost;
  (void) properties;
}

std::shared_ptr<GroupExpression> Group::GetBestExpression(
  PropertySet properties)
{
  (void) properties;
  return nullptr;
}

void Group::set_explored(size_t item_index) {
  explored_flags[item_index] = true;
}

const std::vector<std::shared_ptr<GroupExpression>>
&Group::GetExpressions() const {
  return expressions;
}

const std::vector<bool> &Group::GetExploredFlags() const {
  return explored_flags;
}

} /* namespace optimizer */
} /* namespace peloton */
