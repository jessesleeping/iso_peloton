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

  void InsertExpression(std::shared_ptr<OpExpression> expr);

  std::vector<Group> &Groups();

 private:
  std::vector<Group> groups;
  std::map<std::vector<Property>, size_t> lowest_cost_items;
};

} /* namespace optimizer */
} /* namespace peloton */
