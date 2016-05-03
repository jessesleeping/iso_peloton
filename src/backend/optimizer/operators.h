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
#include "backend/optimizer/column.h"
#include "backend/optimizer/util.h"

#include <vector>

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
  static Operator make(storage::DataTable *table,
                       std::vector<Column *> cols);

  hash_t Hash() const override;

  storage::DataTable *table;
  std::vector<Column *> columns;
};

//===--------------------------------------------------------------------===//
// Project
//===--------------------------------------------------------------------===//
class LogicalProject : public OperatorNode<LogicalProject> {
 public:
  static Operator make();
};

//===--------------------------------------------------------------------===//
// Filter
//===--------------------------------------------------------------------===//
class LogicalFilter : public OperatorNode<LogicalFilter> {
 public:
  static Operator make();
};

//===--------------------------------------------------------------------===//
// InnerJoin
//===--------------------------------------------------------------------===//
class LogicalInnerJoin : public OperatorNode<LogicalInnerJoin> {
 public:
  static Operator make();
};

//===--------------------------------------------------------------------===//
// LeftJoin
//===--------------------------------------------------------------------===//
class LogicalLeftJoin : public OperatorNode<LogicalLeftJoin> {
 public:
  static Operator make();
};

//===--------------------------------------------------------------------===//
// RightJoin
//===--------------------------------------------------------------------===//
class LogicalRightJoin : public OperatorNode<LogicalRightJoin> {
 public:
  static Operator make();
};

//===--------------------------------------------------------------------===//
// OuterJoin
//===--------------------------------------------------------------------===//
class LogicalOuterJoin : public OperatorNode<LogicalOuterJoin> {
 public:
  static Operator make();
};

//===--------------------------------------------------------------------===//
// Aggregate
//===--------------------------------------------------------------------===//
class LogicalAggregate : public OperatorNode<LogicalAggregate> {
 public:
  static Operator make();
};

//===--------------------------------------------------------------------===//
// Limit
//===--------------------------------------------------------------------===//
class LogicalLimit : public OperatorNode<LogicalLimit> {
 public:
  static Operator make();
};

//===--------------------------------------------------------------------===//
// Scan
//===--------------------------------------------------------------------===//
class PhysicalScan : public OperatorNode<PhysicalScan> {
 public:
  static Operator make(oid_t base_table, std::vector<Column *> columns);

  oid_t base_table;
  std::vector<Column *> columns;
};

//===--------------------------------------------------------------------===//
// InnerHashJoin
//===--------------------------------------------------------------------===//
class PhysicalInnerHashJoin : public OperatorNode<PhysicalInnerHashJoin> {
 public:
  static Operator make();
};

//===--------------------------------------------------------------------===//
// LeftHashJoin
//===--------------------------------------------------------------------===//
class PhysicalLeftHashJoin : public OperatorNode<PhysicalLeftHashJoin> {
 public:
  static Operator make();
};

//===--------------------------------------------------------------------===//
// RightHashJoin
//===--------------------------------------------------------------------===//
class PhysicalRightHashJoin : public OperatorNode<PhysicalRightHashJoin> {
 public:
  static Operator make();
};

//===--------------------------------------------------------------------===//
// OuterHashJoin
//===--------------------------------------------------------------------===//
class PhysicalOuterHashJoin : public OperatorNode<PhysicalOuterHashJoin> {
 public:
  static Operator make();
};

//===--------------------------------------------------------------------===//
// Variable
//===--------------------------------------------------------------------===//
class ExprVariable : public OperatorNode<ExprVariable> {
 public:
  static Operator make(Column *column);

  Column *column;
};

//===--------------------------------------------------------------------===//
// Constant
//===--------------------------------------------------------------------===//
class ExprConstant : public OperatorNode<ExprConstant> {
 public:
  static Operator make(Value value);

  Value value;
};

//===--------------------------------------------------------------------===//
// Compare
//===--------------------------------------------------------------------===//
class ExprCompare : public OperatorNode<ExprCompare> {
 public:
  static Operator make(ExpressionType type);

  ExpressionType type;
};

//===--------------------------------------------------------------------===//
// Boolean Operation
//===--------------------------------------------------------------------===//
class ExprBoolOp : public OperatorNode<ExprBoolOp> {
 public:
  static Operator make();
};

//===--------------------------------------------------------------------===//
// Operation (e.g. +, -, string functions)
//===--------------------------------------------------------------------===//
class ExprOp : public OperatorNode<ExprOp> {
 public:
  static Operator make();
};

} /* namespace optimizer */
} /* namespace peloton */
