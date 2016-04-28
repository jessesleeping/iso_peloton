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

#include <vector>
#include <map>

namespace peloton {
namespace optimizer {

using GroupID = int32_t;

//===--------------------------------------------------------------------===//
// Group
//===--------------------------------------------------------------------===//
class Group {
 public:
  Group();

  void add_item(Operator op);

  void set_explored(size_t item_index);

  const std::vector<Operator> &GetOperators() const;

  const std::vector<bool> &GetExploredFlags() const;

 private:
  std::vector<Operator> items;
  std::vector<bool> explored_flags;
  std::map<std::vector<Property>, size_t> lowest_cost_items;
};

} /* namespace optimizer */
} /* namespace peloton */
