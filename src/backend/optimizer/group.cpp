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
}

const std::vector<Operator> &Group::GetOperators() const {
  return items;
}

} /* namespace optimizer */
} /* namespace peloton */
