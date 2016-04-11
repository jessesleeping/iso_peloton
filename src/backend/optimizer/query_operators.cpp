//===----------------------------------------------------------------------===//
//
//                         Peloton
//
// query_operators.cpp
//
// Identification: src/backend/optimizer/query_operators.cpp
//
// Copyright (c) 2015-16, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "backend/optimizer/query_operators.h"
#include "backend/optimizer/operator_visitor.h"

namespace peloton {
namespace optimizer {

//===--------------------------------------------------------------------===//
// TableAttribute
//===--------------------------------------------------------------------===//
TableAttribute *TableAttribute::make(
  int table_list_index,
  int column_index)
{
  TableAttribute *attr = new TableAttribute;
  attr->table_list_index = table_list_index;
  attr->column_index = column_index;
  return attr;
}

//===--------------------------------------------------------------------===//
// Table
//===--------------------------------------------------------------------===//
Table *Table::make(oid_t table_oid) {
  Table *table = new Table;
  table->table_oid = table_oid;
  return table;
}

//===--------------------------------------------------------------------===//
// Order By
//===--------------------------------------------------------------------===//
OrderBy *OrderBy::make(
  int output_list_index,
  bridge::PltFuncMetaInfo equality_fn,
  bridge::PltFuncMetaInfo sort_fn,
  bool hashable,
  bool nulls_first)
{
  OrderBy *order = new OrderBy;
  order->output_list_index = output_list_index;
  order->equality_fn = equality_fn;
  order->sort_fn = sort_fn;
  order->hashable = hashable;
  order->nulls_first = nulls_first;
  return order;
}

//===--------------------------------------------------------------------===//
// Select
//===--------------------------------------------------------------------===//
Select *Select::make(
  std::vector<std::shared_ptr<Table>> table_list,
  std::vector<std::shared_ptr<TableAttribute>> output_list,
  std::vector<std::shared_ptr<OrderBy>> orderings)
{
  Select *select = new Select;
  select->table_list = table_list;
  select->output_list = output_list;
  select->orderings = orderings;
  return select;
}

template<> void OperatorNode<TableAttribute>::accept(OperatorVisitor *v) const {
  v->visit((const TableAttribute *)this);
}
template<> void OperatorNode<Table>::accept(OperatorVisitor *v) const {
  v->visit((const Table *)this);
}
template<> void OperatorNode<OrderBy>::accept(OperatorVisitor *v) const {
  v->visit((const OrderBy *)this);
}
template<> void OperatorNode<Select>::accept(OperatorVisitor *v) const {
  v->visit((const Select *)this);
}

template <> std::string OperatorNode<TableAttribute>::_name = "TableAttribute";
template <> std::string OperatorNode<Table>::_name = "Table";
template <> std::string OperatorNode<OrderBy>::_name = "OrderBy";
template <> std::string OperatorNode<Select>::_name = "Select";

} /* namespace optimizer */
} /* namespace peloton */
