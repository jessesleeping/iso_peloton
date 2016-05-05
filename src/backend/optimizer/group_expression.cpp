//===----------------------------------------------------------------------===//
//
//                         Peloton
//
// group_expression.cpp
//
// Identification: src/backend/optimizer/group_expression.cpp
//
// Copyright (c) 2015-16, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "backend/optimizer/group_expression.h"
#include "backend/optimizer/group.h"

namespace peloton {
namespace optimizer {

//===--------------------------------------------------------------------===//
// Group Expression
//===--------------------------------------------------------------------===//
GroupExpression::GroupExpression(Operator op, std::vector<GroupID> child_groups)
  : group_id(UNDEFINED_GROUP), op(op), child_groups(child_groups)
{
}

GroupID GroupExpression::GetGroupID() const {
  return group_id;
}

void GroupExpression::SetGroupID(GroupID id) {
  group_id = id;
}

const std::vector<GroupID> &GroupExpression::ChildGroupIDs() const {
  return child_groups;
}

Operator GroupExpression::Op() const {
  return op;
}

std::tuple<std::shared_ptr<Stats>, double> GroupExpression::DeriveStatsAndCost(
  std::vector<std::shared_ptr<Stats>> child_stats,
  std::vector<double> child_costs)
{
  (void) child_stats;
  (void) child_costs;
  return std::make_tuple(std::shared_ptr<Stats>(nullptr), 0.0);
}

hash_t GroupExpression::Hash() const {
  size_t hash = op.Hash();

  for (size_t i = 0; i < child_groups.size(); ++i) {
    hash = util::CombineHashes(hash, util::Hash<GroupID>(&(child_groups[i])));
  }

  return hash;
}

bool GroupExpression::operator==(const GroupExpression &r) {
  bool eq = (op == r.Op());

  for (size_t i = 0; i < child_groups.size(); ++i) {
    eq = eq && (child_groups[i] == r.child_groups[i]);
  }

  return eq;
}

} /* namespace optimizer */
} /* namespace peloton */
