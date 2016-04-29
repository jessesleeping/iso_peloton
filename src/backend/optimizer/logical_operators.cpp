//===----------------------------------------------------------------------===//
//
//                         Peloton
//
// logical_operators.cpp
//
// Identification: src/backend/optimizer/logical_operators.cpp
//
// Copyright (c) 2015-16, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "backend/optimizer/logical_operators.h"
#include "backend/optimizer/operator_visitor.h"

namespace peloton {
namespace optimizer {
//===--------------------------------------------------------------------===//
// Leaf
//===--------------------------------------------------------------------===//
Operator LeafOperator::make(GroupID group) {
  LeafOperator *op = new LeafOperator;
  op->origin_group = group;
  return Operator(op);
}

//===--------------------------------------------------------------------===//
// Get
//===--------------------------------------------------------------------===//
Operator LogicalGet::make(oid_t base_table, std::vector<oid_t> columns) {
  LogicalGet *get = new LogicalGet;
  get->base_table = base_table;
  get->columns = columns;
  return Operator(get);
}

//===--------------------------------------------------------------------===//
// Project
//===--------------------------------------------------------------------===//
Operator LogicalProject::make(GroupID child) {
  LogicalProject *project = new LogicalProject;
  project->child = child;
  return Operator(project);
}

//===--------------------------------------------------------------------===//
// Filter
//===--------------------------------------------------------------------===//
Operator LogicalFilter::make(GroupID child, QueryExpression *predicate) {
  LogicalFilter *filter = new LogicalFilter;
  filter->child = child;
  filter->predicate = predicate;
  return Operator(filter);
}

//===--------------------------------------------------------------------===//
// InnerJoin
//===--------------------------------------------------------------------===//
Operator LogicalInnerJoin::make(GroupID outer, GroupID inner) {
  LogicalInnerJoin *join = new LogicalInnerJoin;
  join->outer = outer;
  join->inner = inner;
  return Operator(join);
}

//===--------------------------------------------------------------------===//
// LeftJoin
//===--------------------------------------------------------------------===//
Operator LogicalLeftJoin::make(GroupID outer, GroupID inner) {
  LogicalLeftJoin *join = new LogicalLeftJoin;
  join->outer = outer;
  join->inner = inner;
  return Operator(join);
}

//===--------------------------------------------------------------------===//
// RightJoin
//===--------------------------------------------------------------------===//
Operator LogicalRightJoin::make(GroupID outer, GroupID inner) {
  LogicalRightJoin *join = new LogicalRightJoin;
  join->outer = outer;
  join->inner = inner;
  return Operator(join);
}

//===--------------------------------------------------------------------===//
// OuterJoin
//===--------------------------------------------------------------------===//
Operator LogicalOuterJoin::make(GroupID outer, GroupID inner) {
  LogicalOuterJoin *join = new LogicalOuterJoin;
  join->outer = outer;
  join->inner = inner;
  return Operator(join);
}

//===--------------------------------------------------------------------===//
// Aggregate
//===--------------------------------------------------------------------===//
Operator LogicalAggregate::make(GroupID child) {
  LogicalAggregate *agg = new LogicalAggregate;
  agg->child = child;
  return Operator(agg);
}

//===--------------------------------------------------------------------===//
// Limit
//===--------------------------------------------------------------------===//
Operator LogicalLimit::make(GroupID child, int limit) {
  LogicalLimit *limit_op = new LogicalLimit;
  limit_op->child = child;
  limit_op->limit = limit;;
  return Operator(limit_op);
}

template<>
void OperatorNode<LeafOperator>::accept(OperatorVisitor *v) const {
  v->visit((const LeafOperator *)this);
}
template<>
void OperatorNode<LogicalGet>::accept(OperatorVisitor *v) const {
  v->visit((const LogicalGet *)this);
}
template<>
void OperatorNode<LogicalProject>::accept(OperatorVisitor *v) const {
  v->visit((const LogicalProject *)this);
}
template<>
void OperatorNode<LogicalFilter>::accept(OperatorVisitor *v) const {
  v->visit((const LogicalFilter *)this);
}
template<>
void OperatorNode<LogicalInnerJoin>::accept(OperatorVisitor *v) const {
  v->visit((const LogicalInnerJoin *)this);
}
template<>
void OperatorNode<LogicalLeftJoin>::accept(OperatorVisitor *v) const {
  v->visit((const LogicalLeftJoin *)this);
}
template<>
void OperatorNode<LogicalRightJoin>::accept(OperatorVisitor *v) const {
  v->visit((const LogicalRightJoin *)this);
}
template<>
void OperatorNode<LogicalOuterJoin>::accept(OperatorVisitor *v) const {
  v->visit((const LogicalOuterJoin *)this);
}
template<>
void OperatorNode<LogicalAggregate>::accept(OperatorVisitor *v) const {
  v->visit((const LogicalAggregate *)this);
}
template<>
void OperatorNode<LogicalLimit>::accept(OperatorVisitor *v) const {
  v->visit((const LogicalLimit *)this);
}

template<>
std::string OperatorNode<LeafOperator>::_name = "LeafOperator";
template<>
std::string OperatorNode<LogicalGet>::_name = "LogicalGet";
template<>
std::string OperatorNode<LogicalProject>::_name = "LogicalProject";
template<>
std::string OperatorNode<LogicalFilter>::_name = "LogicalFilter";
template<>
std::string OperatorNode<LogicalInnerJoin>::_name = "LogicalInnerJoin";
template<>
std::string OperatorNode<LogicalLeftJoin>::_name = "LogicalLeftJoin";
template<>
std::string OperatorNode<LogicalRightJoin>::_name = "LogicalRightJoin";
template<>
std::string OperatorNode<LogicalOuterJoin>::_name = "LogicalOuterJoin";
template<>
std::string OperatorNode<LogicalAggregate>::_name = "LogicalAggregate";
template<>
std::string OperatorNode<LogicalLimit>::_name = "LogicalLimit";

template<>
OpType OperatorNode<LeafOperator>::_type = OpType::Leaf;
template<>
OpType OperatorNode<LogicalGet>::_type = OpType::Get;
template<>
OpType OperatorNode<LogicalProject>::_type = OpType::Project;
template<>
OpType OperatorNode<LogicalFilter>::_type = OpType::Filter;
template<>
OpType OperatorNode<LogicalInnerJoin>::_type = OpType::InnerJoin;
template<>
OpType OperatorNode<LogicalLeftJoin>::_type = OpType::LeftJoin;
template<>
OpType OperatorNode<LogicalRightJoin>::_type = OpType::RightJoin;
template<>
OpType OperatorNode<LogicalOuterJoin>::_type = OpType::OuterJoin;
template<>
OpType OperatorNode<LogicalAggregate>::_type = OpType::Aggregate;
template<>
OpType OperatorNode<LogicalLimit>::_type = OpType::Limit;

template<>
bool OperatorNode<LeafOperator>::is_logical() const {
  return false;
}
template<>
bool OperatorNode<LogicalGet>::is_logical() const {
  return true;
}
template<>
bool OperatorNode<LogicalProject>::is_logical() const {
  return true;
}
template<>
bool OperatorNode<LogicalFilter>::is_logical() const {
  return true;
}
template<>
bool OperatorNode<LogicalInnerJoin>::is_logical() const {
  return true;
}
template<>
bool OperatorNode<LogicalLeftJoin>::is_logical() const {
  return true;
}
template<>
bool OperatorNode<LogicalRightJoin>::is_logical() const {
  return true;
}
template<>
bool OperatorNode<LogicalOuterJoin>::is_logical() const {
  return true;
}
template<>
bool OperatorNode<LogicalAggregate>::is_logical() const {
  return true;
}
template<>
bool OperatorNode<LogicalLimit>::is_logical() const {
  return true;
}

template<>
bool OperatorNode<LeafOperator>::is_physical() const {
  return false;
}
template<>
bool OperatorNode<LogicalGet>::is_physical() const {
  return false;
}
template<>
bool OperatorNode<LogicalProject>::is_physical() const {
  return false;
}
template<>
bool OperatorNode<LogicalFilter>::is_physical() const {
  return false;
}
template<>
bool OperatorNode<LogicalInnerJoin>::is_physical() const {
  return false;
}
template<>
bool OperatorNode<LogicalLeftJoin>::is_physical() const {
  return false;
}
template<>
bool OperatorNode<LogicalRightJoin>::is_physical() const {
  return false;
}
template<>
bool OperatorNode<LogicalOuterJoin>::is_physical() const {
  return false;
}
template<>
bool OperatorNode<LogicalAggregate>::is_physical() const {
  return false;
}
template<>
bool OperatorNode<LogicalLimit>::is_physical() const {
  return false;
}

} /* namespace optimizer */
} /* namespace peloton */
