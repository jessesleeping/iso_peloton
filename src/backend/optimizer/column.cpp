//===----------------------------------------------------------------------===//
//
//                         Peloton
//
// column.cpp
//
// Identification: src/backend/optimizer/column.cpp
//
// Copyright (c) 2015-16, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "backend/optimizer/column.h"

namespace peloton {
namespace optimizer {

//===--------------------------------------------------------------------===//
// Column
//===--------------------------------------------------------------------===//
Column::Column(ColumnID id,
               std::string name,
               oid_t base_table,
               oid_t column_index,
               ValueType type)
  : id(id),
    name(name),
    base_table(base_table),
    column_index(column_index),
    type(type)
{
}

ColumnID Column::ID() {
  return id;
}

hash_t Column::Hash() const {
  return util::Hash<ColumnID>(&id);
}

} /* namespace optimizer */
} /* namespace peloton */
