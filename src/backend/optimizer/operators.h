//===----------------------------------------------------------------------===//
//
//                         Peloton
//
// operators.h
//
// Identification: src/backend/optimizer/operators.h
//
// Copyright (c) 2015-16, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include "backend/optimizer/operator_node.h"
#include "backend/optimizer/query_operators.h"
#include "backend/optimizer/group.h"

namespace peloton {
namespace optimizer {

//===--------------------------------------------------------------------===//
// Leaf
//===--------------------------------------------------------------------===//
class LeafOperator : OperatorNode<LeafOperator> {
 public:
  static Operator make(GroupID group);

  GroupID origin_group;
};

//===--------------------------------------------------------------------===//
// Get
//===--------------------------------------------------------------------===//
class LogicalGet : public OperatorNode<LogicalGet> {
 public:
  static Operator make(oid_t base_table, std::vector<oid_t> columns);

  oid_t base_table;
  std::vector<oid_t> columns;
};

//===--------------------------------------------------------------------===//
// Project
//===--------------------------------------------------------------------===//
class LogicalProject : public OperatorNode<LogicalProject> {
 public:
  static Operator make(GroupID child);

  GroupID child;
};

//===--------------------------------------------------------------------===//
// Filter
//===--------------------------------------------------------------------===//
class LogicalFilter : public OperatorNode<LogicalFilter> {
 public:
  static Operator make(GroupID child, QueryExpression *predicate);

  GroupID child;
  QueryExpression *predicate;
};

//===--------------------------------------------------------------------===//
// InnerJoin
//===--------------------------------------------------------------------===//
class LogicalInnerJoin : public OperatorNode<LogicalInnerJoin> {
 public:
  static Operator make(GroupID outer, GroupID inner);

  GroupID outer;
  GroupID inner;
};

//===--------------------------------------------------------------------===//
// LeftJoin
//===--------------------------------------------------------------------===//
class LogicalLeftJoin : public OperatorNode<LogicalLeftJoin> {
 public:
  static Operator make(GroupID outer, GroupID inner);

  GroupID outer;
  GroupID inner;
};

//===--------------------------------------------------------------------===//
// RightJoin
//===--------------------------------------------------------------------===//
class LogicalRightJoin : public OperatorNode<LogicalRightJoin> {
 public:
  static Operator make(GroupID outer, GroupID inner);

  GroupID outer;
  GroupID inner;
};

//===--------------------------------------------------------------------===//
// OuterJoin
//===--------------------------------------------------------------------===//
class LogicalOuterJoin : public OperatorNode<LogicalOuterJoin> {
 public:
  static Operator make(GroupID outer, GroupID inner);

  GroupID outer;
  GroupID inner;
};

//===--------------------------------------------------------------------===//
// Aggregate
//===--------------------------------------------------------------------===//
class LogicalAggregate : public OperatorNode<LogicalAggregate> {
 public:
  static Operator make(GroupID child);

  GroupID child;
};

//===--------------------------------------------------------------------===//
// Limit
//===--------------------------------------------------------------------===//
class LogicalLimit : public OperatorNode<LogicalLimit> {
 public:
  static Operator make(GroupID child, int limit);

  GroupID child;
  int limit;
};

//===--------------------------------------------------------------------===//
// Scan
//===--------------------------------------------------------------------===//
class PhysicalScan : public OperatorNode<PhysicalScan> {
 public:
  static Operator make(oid_t base_table, std::vector<oid_t> columns);

  oid_t base_table;
  std::vector<oid_t> columns;
};

//===--------------------------------------------------------------------===//
// InnerHashJoin
//===--------------------------------------------------------------------===//
class PhysicalInnerHashJoin : public OperatorNode<PhysicalInnerHashJoin> {
 public:
  static Operator make(GroupID outer, GroupID inner);

  GroupID outer;
  GroupID inner;
};

//===--------------------------------------------------------------------===//
// LeftHashJoin
//===--------------------------------------------------------------------===//
class PhysicalLeftHashJoin : public OperatorNode<PhysicalLeftHashJoin> {
 public:
  static Operator make(GroupID outer, GroupID inner);

  GroupID outer;
  GroupID inner;
};

//===--------------------------------------------------------------------===//
// RightHashJoin
//===--------------------------------------------------------------------===//
class PhysicalRightHashJoin : public OperatorNode<PhysicalRightHashJoin> {
 public:
  static Operator make(GroupID outer, GroupID inner);

  GroupID outer;
  GroupID inner;
};

//===--------------------------------------------------------------------===//
// OuterHashJoin
//===--------------------------------------------------------------------===//
class PhysicalOuterHashJoin : public OperatorNode<PhysicalOuterHashJoin> {
 public:
  static Operator make(GroupID outer, GroupID inner);

  GroupID outer;
  GroupID inner;
};

} /* namespace optimizer */
} /* namespace peloton */
