//===----------------------------------------------------------------------===//
//
//                         Peloton
//
// operator_printer.cpp
//
// Identification: src/backend/optimizer/operator_printer.cpp
//
// Copyright (c) 2015-16, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "backend/optimizer/operator_printer.h"
#include "backend/optimizer/query_operators.h"

namespace peloton {
namespace optimizer {

OperatorPrinter::OperatorPrinter(BaseOperatorNode *op)
  : op_(op), depth_(0), new_line_(false) {}

std::string OperatorPrinter::print() {
  if (printed_op_.empty()) {
    op_->accept(this);
  }
  return printed_op_;
}

void OperatorPrinter::visit(const TableAttribute *op) {
  (void)op;
}

void OperatorPrinter::visit(const Table *op) {
  (void)op;
}

void OperatorPrinter::visit(const OrderBy *op) {
  (void)op;
}

void OperatorPrinter::visit(const Select *op) {
  push_header("Select");
  push_header("Table list");
  for (size_t i = 0; i < op->table_list.size(); ++i) {
    Table *table = op->table_list[i].get();
    append_line("Table: oid " + std::to_string(table->table_oid));
  }
  pop(); // Table list

  push_header("Output list");
  for (size_t i = 0; i < op->output_list.size(); ++i) {
    TableAttribute *attr = op->output_list[i].get();
    append("TableAttribute: ");
    append("table_index " + std::to_string(attr->table_list_index));
    append(", ");
    append("column_index " + std::to_string(attr->column_index));
    append_line();
  }
  pop(); // Output list

  push_header("Orderings");
  pop();

  pop(); // Select
}

void OperatorPrinter::append(const std::string &string) {
  if (new_line_) {
    for (int i = 0; i < depth_; ++i) {
      printed_op_ += "  ";
    }
    new_line_ = false;
  }
  printed_op_ += string;
}

void OperatorPrinter::append_line(const std::string &string) {
  append(string + "\n");
  new_line_ = true;
}

void OperatorPrinter::append_line() {
  append_line("");
}

void OperatorPrinter::push() {
  depth_++;
}

void OperatorPrinter::push_header(const std::string &string) {
  append_line(string);
  push();
}

void OperatorPrinter::pop() {
  depth_--;
  assert(depth_ >= 0);
}

std::string PrintOperator(BaseOperatorNode *op) {
  OperatorPrinter printer(op);
  return printer.print();
}

} /* namespace optimizer */
} /* namespace peloton */
