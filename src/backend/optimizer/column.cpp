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
Column::Column(ColumnID id, peloton::ExpressionType type, std::string name)
  : id(id), type(type), name(name)
{
}

hash_t Column::Hash() const {
  return util::Hash<ColumnID>(&id);
}

} /* namespace optimizer */
} /* namespace peloton */
