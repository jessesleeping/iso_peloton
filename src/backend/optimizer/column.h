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
  Column(ColumnID id,
         std::string name,
         oid_t base_table,
         oid_t column_index,
         ValueType type);

  ColumnID ID();

  hash_t Hash() const;

 private:
  const ColumnID id;
  const std::string name;
  const oid_t base_table;
  const oid_t column_index;
  const ValueType type;
};

} /* namespace optimizer */
} /* namespace peloton */
