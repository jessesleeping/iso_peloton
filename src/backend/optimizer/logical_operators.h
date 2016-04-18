//===----------------------------------------------------------------------===//
//
//                         Peloton
//
// logical_operators.h
//
// Identification: src/backend/optimizer/logical_operators.h
//
// Copyright (c) 2015-16, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include "backend/optimizer/operator_node.h"
#include "backend/optimizer/query_operators.h"

namespace peloton {
namespace optimizer {

//===--------------------------------------------------------------------===//
// Get
//===--------------------------------------------------------------------===//
class LogicalGet : OperatorNode<LogicalGet> {
 public:
  static Operator make(oid_t base_table, std::vector<oid_t> columns);

  oid_t base_table;
  std::vector<oid_t> columns;
};

//===--------------------------------------------------------------------===//
// Project
//===--------------------------------------------------------------------===//
class LogicalProject : OperatorNode<LogicalProject> {
 public:
  static Operator make(Operator child);

  Operator child;
};

//===--------------------------------------------------------------------===//
// Filter
//===--------------------------------------------------------------------===//
class LogicalFilter : OperatorNode<LogicalFilter> {
 public:
  static Operator make(Operator child, QueryExpression *predicate);

  Operator child;
  QueryExpression *predicate;
};

//===--------------------------------------------------------------------===//
// InnerJoin
//===--------------------------------------------------------------------===//
class LogicalInnerJoin : OperatorNode<LogicalInnerJoin> {
 public:
  static Operator make(Operator outer, Operator inner);

  Operator outer;
  Operator inner;
};

//===--------------------------------------------------------------------===//
// LeftJoin
//===--------------------------------------------------------------------===//
class LogicalLeftJoin : OperatorNode<LogicalLeftJoin> {
 public:
  static Operator make(Operator outer, Operator inner);

  Operator outer;
  Operator inner;
};

//===--------------------------------------------------------------------===//
// RightJoin
//===--------------------------------------------------------------------===//
class LogicalRightJoin : OperatorNode<LogicalRightJoin> {
 public:
  static Operator make(Operator outer, Operator inner);

  Operator outer;
  Operator inner;
};

//===--------------------------------------------------------------------===//
// OuterJoin
//===--------------------------------------------------------------------===//
class LogicalOuterJoin : OperatorNode<LogicalOuterJoin> {
 public:
  static Operator make(Operator outer, Operator inner);

  Operator outer;
  Operator inner;
};

//===--------------------------------------------------------------------===//
// Aggregate
//===--------------------------------------------------------------------===//
class LogicalAggregate : OperatorNode<LogicalAggregate> {
 public:
  static Operator make(Operator child);

  Operator child;
};

//===--------------------------------------------------------------------===//
// Limit
//===--------------------------------------------------------------------===//
class LogicalLimit : OperatorNode<LogicalLimit> {
 public:
  static Operator make(Operator child, int limit);

  Operator child;
  int limit;
};

} /* namespace optimizer */
} /* namespace peloton */
