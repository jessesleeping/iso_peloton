//===----------------------------------------------------------------------===//
//
//                         Peloton
//
// group.h
//
// Identification: src/backend/optimizer/group.h
//
// Copyright (c) 2015-16, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include "backend/optimizer/operator_node.h"
#include "backend/optimizer/property.h"
#include "backend/optimizer/group_expression.h"

#include <vector>
#include <map>

namespace peloton {
namespace optimizer {

using GroupID = int32_t;

const GroupID UNDEFINED_GROUP = -1;

//===--------------------------------------------------------------------===//
// Group
//===--------------------------------------------------------------------===//
class Group {
 public:
  Group();

  void AddExpression(std::shared_ptr<GroupExpression> expr);

  void set_explored(size_t item_index);

  const std::vector<std::shared_ptr<GroupExpression>> &GetExpressions() const;

  const std::vector<bool> &GetExploredFlags() const;

 private:
  std::vector<std::shared_ptr<GroupExpression>> expressions;
  std::vector<bool> explored_flags;
  std::map<std::vector<Property>, size_t> lowest_cost_items;
};

} /* namespace optimizer */
} /* namespace peloton */
