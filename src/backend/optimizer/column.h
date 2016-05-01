//===----------------------------------------------------------------------===//
//
//                         Peloton
//
// column.h
//
// Identification: src/backend/optimizer/column.h
//
// Copyright (c) 2015-16, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include "backend/optimizer/operator_node.h"
#include "backend/optimizer/query_operators.h"
#include "backend/optimizer/group.h"
#include "backend/optimizer/util.h"

#include "backend/common/types.h"

namespace peloton {
namespace optimizer {

using ColumnID = int32_t;

//===--------------------------------------------------------------------===//
// Column
//===--------------------------------------------------------------------===//
class Column {
 public:
  Column(ColumnID id, peloton::ExpressionType type, std::string name);

  hash_t Hash() const;

 private:
  ColumnID id;

  peloton::ExpressionType type;

  std::string name;
};

} /* namespace optimizer */
} /* namespace peloton */
