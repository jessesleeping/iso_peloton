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

namespace peloton {
namespace optimizer {

class TableAttribute;
class Table;
class OrderBy;
class Select;

//===--------------------------------------------------------------------===//
// Operator Visitor
//===--------------------------------------------------------------------===//

class OperatorVisitor {
 public:
  virtual ~OperatorVisitor() {};

  virtual void visit(const TableAttribute*) = 0;
  virtual void visit(const Table*) = 0;
  virtual void visit(const OrderBy*) = 0;
  virtual void visit(const Select*) = 0;
};

} /* namespace optimizer */
} /* namespace peloton */
