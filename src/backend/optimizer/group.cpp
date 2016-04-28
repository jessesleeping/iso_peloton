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
Group::Group() {}

void Group::add_item(Operator op) {
  // TODO(abpoms): do duplicate checking
  items.push_back(op);
  explored_flags.push_back(false);
}

void Group::set_explored(size_t item_index) {
  explored_flags[item_index] = true;
}

const std::vector<Operator> &Group::GetOperators() const {
  return items;
}

const std::vector<bool> &Group::GetExploredFlags() const {
  return explored_flags;
}

} /* namespace optimizer */
} /* namespace peloton */
