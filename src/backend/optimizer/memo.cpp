//===----------------------------------------------------------------------===//
//
//                         Peloton
//
// memo.cpp
//
// Identification: src/backend/optimizer/memo.cpp
//
// Copyright (c) 2015-16, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "backend/optimizer/memo.h"

namespace peloton {
namespace optimizer {

//===--------------------------------------------------------------------===//
// Memo
//===--------------------------------------------------------------------===//
Memo::Memo() {}

GroupID Memo::InsertExpression(std::shared_ptr<GroupExpression> expr) {
  // Lookup in hash table
  (void)expr;

  return 0;
}

const std::vector<Group> &Memo::Groups() const {
  return groups;
}

Group *Memo::GetGroupByID(GroupID id) {
  return &(groups[id]);
}


} /* namespace optimizer */
} /* namespace peloton */
