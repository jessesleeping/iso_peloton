//===----------------------------------------------------------------------===//
//
//                         Peloton
//
// operator_printer.h
//
// Identification: src/backend/optimizer/operator_printer.h
//
// Copyright (c) 2015-16, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include "backend/optimizer/operator_visitor.h"
#include "backend/optimizer/operator_node.h"

#include <string>

namespace peloton {
namespace optimizer {

//===--------------------------------------------------------------------===//
// Operator Visitor
//===--------------------------------------------------------------------===//

class OperatorPrinter : OperatorVisitor {
 public:
  OperatorPrinter(Select *op);

  std::string print();

  virtual void visit(const Variable*);
  virtual void visit(const Constant*);
  virtual void visit(const AndOperator*);
  virtual void visit(const OrOperator*);
  virtual void visit(const NotOperator*);
  virtual void visit(const Attribute *);
  virtual void visit(const Table *);
  virtual void visit(const Join *);
  virtual void visit(const OrderBy *);
  virtual void visit(const Select *);

 private:
  void append(const std::string &string);

  void append_line(const std::string &string);

  void append_line();

  void push();

  void push_header(const std::string &string);

  void pop();

  Select *op_;

  int depth_;
  std::string printed_op_;
  bool new_line_;
};

std::string PrintOperator(Select *op);

} /* namespace optimizer */
} /* namespace peloton */
