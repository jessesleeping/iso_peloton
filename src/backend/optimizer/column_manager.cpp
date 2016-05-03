//===----------------------------------------------------------------------===//
//
//                         Peloton
//
// column_manager.cpp
//
// Identification: src/backend/optimizer/column_manager.cpp
//
// Copyright (c) 2015-16, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "backend/optimizer/column_manager.h"

#include <tuple>

namespace peloton {
namespace optimizer {

ColumnManager::ColumnManager() {
}

ColumnManager::~ColumnManager() {
  for (Column *col : columns) {
    delete col;
  }
}

Column *ColumnManager::LookupColumn(oid_t base_table, oid_t column_index) {
  auto key = std::make_tuple(base_table, column_index);
  auto it = table_col_index_to_column.find(key);
  if (it == table_col_index_to_column.end()) {
    return nullptr;
  } else {
    return it->second;
  }
}

Column *ColumnManager::LookupColumnByID(ColumnID id) {
  return id_to_column.at(id);
}

Column *ColumnManager::AddColumn(std::string name,
                                 oid_t base_table,
                                 oid_t column_index,
                                 ValueType type)
{
  Column *col =
    new Column(next_column_id++, name, base_table, column_index, type);

  auto key = std::make_tuple(base_table, column_index);
  table_col_index_to_column.insert(std::pair<decltype(key), Column*>(key, col));
  id_to_column.insert(std::pair<ColumnID, Column *>(col->ID(), col));
  columns.push_back(col);
  return col;
}

} /* namespace optimizer */
} /* namespace peloton */
