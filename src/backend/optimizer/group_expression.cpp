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

namespace peloton {
namespace optimizer {

//===--------------------------------------------------------------------===//
// Group Expression
//===--------------------------------------------------------------------===//
GroupExpression::GroupExpression(Operator op,
                                 std::vector<GroupID> child_groups)
  : op(op), child_groups(child_groups)
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

hash_t GroupExpression::Hash() const {
  size_t hash = op.Hash();

  for (size_t i = 0; i < child_groups.size(); ++i) {
    hash = util::CombineHashes(hash, util::Hash<GroupID>(&(child_groups[i])));
  }

  return hash;
}

} /* namespace optimizer */
} /* namespace peloton */
