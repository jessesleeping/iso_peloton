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
  virtual ~OperatorVisitor();

  virtual void visit(const TableAttribute*);
  virtual void visit(const Table*);
  virtual void visit(const OrderBy*);
  virtual void visit(const Select*);
};

} /* namespace optimizer */
} /* namespace peloton */
