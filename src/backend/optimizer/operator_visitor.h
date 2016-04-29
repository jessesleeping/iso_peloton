//===----------------------------------------------------------------------===//
//
//                         Peloton
//
// operator_visitor.h
//
// Identification: src/backend/optimizer/operator_visitor.h
//
// Copyright (c) 2015-16, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include "backend/optimizer/logical_operators.h"

namespace peloton {
namespace optimizer {

//===--------------------------------------------------------------------===//
// Operator Visitor
//===--------------------------------------------------------------------===//

class OperatorVisitor {
 public:
  virtual ~OperatorVisitor() {};

  virtual void visit(const LeafOperator*);
  virtual void visit(const LogicalGet*);
  virtual void visit(const LogicalProject*);
  virtual void visit(const LogicalFilter*);
  virtual void visit(const LogicalInnerJoin*);
  virtual void visit(const LogicalLeftJoin*);
  virtual void visit(const LogicalRightJoin*);
  virtual void visit(const LogicalOuterJoin*);
  virtual void visit(const LogicalAggregate*);
  virtual void visit(const LogicalLimit*);
};

} /* namespace optimizer */
} /* namespace peloton */
