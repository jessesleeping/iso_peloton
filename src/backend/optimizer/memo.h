//===----------------------------------------------------------------------===//
//
//                         Peloton
//
// memo.h
//
// Identification: src/backend/optimizer/memo.h
//
// Copyright (c) 2015-16, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include "backend/optimizer/group.h"
#include "backend/optimizer/op_expression.h"

#include <unordered_set>
#include <vector>
#include <map>

namespace peloton {
namespace optimizer {

//===--------------------------------------------------------------------===//
// Memo
//===--------------------------------------------------------------------===//
class Memo {
 public:
  Memo();

  GroupID InsertExpression(std::shared_ptr<GroupExpression> expr);

  const std::vector<Group> &Groups() const;

  Group *GetGroupByID(GroupID id);

 private:
  std::unordered_set<GroupExpression *> group_expressions;
  std::vector<Group> groups;
  std::map<std::vector<Property>, size_t> lowest_cost_items;
};

} /* namespace optimizer */
} /* namespace peloton */
