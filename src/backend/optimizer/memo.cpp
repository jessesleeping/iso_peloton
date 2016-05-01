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


void Memo::InsertExpression(std::shared_ptr<OpExpression> expr) {
  // Recursively insert

  // 
  (void)expr;
}

std::vector<Group> &Memo::Groups() {
  return groups;
}

} /* namespace optimizer */
} /* namespace peloton */
