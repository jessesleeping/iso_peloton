//===----------------------------------------------------------------------===//
//
//                         Peloton
//
// query_operators.h
//
// Identification: src/backend/optimizer/query_operators.h
//
// Copyright (c) 2015-16, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include "backend/optimizer/operator_node.h"

#include "backend/common/types.h"
#include "backend/bridge/dml/expr/pg_func_map.h"

#include <vector>
#include <memory>

namespace peloton {
namespace optimizer {

//===--------------------------------------------------------------------===//
// Constant
//===--------------------------------------------------------------------===//
// struct Constant : OperatorNode<Constant> {
//   static Operator make(Value v);

//   Value value;
// };

//===--------------------------------------------------------------------===//
// TableAttribute
//===--------------------------------------------------------------------===//
struct TableAttribute : OperatorNode<TableAttribute> {
  static TableAttribute *make(
    int table_list_index,
    int column_index);

  int table_list_index;
  int column_index;
};

//===--------------------------------------------------------------------===//
// Table
//===--------------------------------------------------------------------===//
struct Table : OperatorNode<Table> {
  static Table *make(oid_t table_oid);

  oid_t table_oid;
};

//===--------------------------------------------------------------------===//
// Join
//===--------------------------------------------------------------------===//
// struct Join : OperatorNode<Join> {
//   static Join *make(PelotonJoinType join_type,
//                     int left_table,
//                     Attribute left_attribute,
//                     int right_table,
//                     Attribute right_attribute,);

//   PelotonJoinType join_type;
// };

//===--------------------------------------------------------------------===//
// Group By
//===--------------------------------------------------------------------===//
// struct GroupBy : OperatorNode<GroupBy> {
//   static Operator make(Operator a);

//   Operator a;
// };

//===--------------------------------------------------------------------===//
// Order By
//===--------------------------------------------------------------------===//
struct OrderBy : OperatorNode<OrderBy> {
  static OrderBy *make(
    int output_list_index,
    bridge::PltFuncMetaInfo equality_fn,
    bridge::PltFuncMetaInfo sort_fn,
    bool hashable,
    bool nulls_first,
    bool reverse);

  int output_list_index;
  bridge::PltFuncMetaInfo equality_fn;
  bridge::PltFuncMetaInfo sort_fn;
  bool hashable;
  bool nulls_first;

  bool reverse;
};

//===--------------------------------------------------------------------===//
// Select
//===--------------------------------------------------------------------===//
struct Select : OperatorNode<Select> {
  static Select *make(
    std::vector<std::shared_ptr<Table>> table_list,
    std::vector<std::shared_ptr<TableAttribute>> output_list,
    std::vector<std::shared_ptr<OrderBy>> orderings);

  std::vector<std::shared_ptr<Table>> table_list;
  std::vector<std::shared_ptr<TableAttribute>> output_list;
  std::vector<std::shared_ptr<OrderBy>> orderings;
};

//===--------------------------------------------------------------------===//
// Aggregate Functions
//===--------------------------------------------------------------------===//
// struct MaxFn : OperatorNode<MaxFn> {
//   static Operator make(Operator a);

//   Operator a;
// };

// struct MinFn : OperatorNode<MinFn> {
//   static Operator make(Operator a);

//   Operator a;
// };

// struct SumFn : OperatorNode<SumFn> {
//   static Operator make(Operator a);

//   Operator a;
// };

// struct AverageFn : OperatorNode<AverageFn> {
//   static Operator make(Operator a);

//   Operator a;
// }

// struct CountFn : OperatorNode<CountFn> {
//   static Operator make(Operator a);

//   Operator a;
// }

//===--------------------------------------------------------------------===//
// Scalar Function
//===--------------------------------------------------------------------===//

// Boolean
// struct AndFn : OperatorNode<AndOperator> {
//   static Operator make(Operator a, Operator b);

//   Operator a;
//   Operator b;
// };

// struct OrFn : OperatorNode<OrOperator> {
//   static Operator make(Operator a, Operator b);

//   Operator a;
//   Operator b;
// };

// struct EqualFn : OperatorNode<EqualFn> {
//   static Operator make(Operator a, Operator b);

//   Operator a;
//   Operator b;
// };

// struct NotEqualFn : OperatorNode<NotEqualFn> {
//   static Operator make(Operator a, Operator b);

//   Operator a;
//   Operator b;
// };

// struct LessFn : OperatorNode<LessFn> {
//   static Operator make(Operator a, Operator b);

//   Operator a;
//   Operator b;
// };

// struct GreaterFn : OperatorNode<GreaterFn> {
//   static Operator make(Operator a, Operator b);

//   Operator a;
//   Operator b;
// };

// struct LessEqualFn : OperatorNode<LessEqualFn> {
//   static Operator make(Operator a, Operator b);

//   Operator a;
//   Operator b;
// };

// struct GreaterEqualFn : OperatorNode<GreaterEqualFn> {
//   static Operator make(Operator a, Operator b);

//   Operator a;
//   Operator b;
// };


} /* namespace optimizer */
} /* namespace peloton */
