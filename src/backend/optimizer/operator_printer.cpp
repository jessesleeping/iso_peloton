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
#include "backend/common/types.h"

namespace peloton {
namespace optimizer {

OperatorPrinter::OperatorPrinter(Select *op)
  : op_(op), depth_(0), new_line_(false) {}

std::string OperatorPrinter::print() {
  if (printed_op_.empty()) {
    op_->accept(this);
  }
  return printed_op_;
}

void OperatorPrinter::visit(const Variable *op) {
  append("Variable");
  (void)op;
}

void OperatorPrinter::visit(const Constant *op) {
  append("Constant");
  (void)op;
}

void OperatorPrinter::visit(const OperatorExpression* op) {
  push_header("OperatorExpression, type: " +
              ExpressionTypeToString(op->type));
  for (QueryExpression *e : op->args) {
    e->accept(this);
    append_line();
  }
  pop(); // OperatorExpression
}

void OperatorPrinter::visit(const AndOperator *op) {
  push_header("And");
  for (QueryExpression *e : op->args) {
    e->accept(this);
    append_line();
  }
  pop(); // And
}

void OperatorPrinter::visit(const OrOperator *op) {
  push_header("Or");
  for (QueryExpression *e : op->args) {
    e->accept(this);
    append_line();
  }
  pop(); // Or
}

void OperatorPrinter::visit(const NotOperator *op) {
  push_header("Not");
  op->arg->accept(this);
  pop();
}

void OperatorPrinter::visit(const Attribute *op) {
  append("Attribute: ");
  append("table_index " + std::to_string(op->table_index));
  append(", ");
  append("column_index " + std::to_string(op->column_index));
}

void OperatorPrinter::visit(const Table *op) {
  append("Table: oid " + std::to_string(op->table_oid));
}

void OperatorPrinter::visit(const Join *op) {
  push_header("Join: type " + PelotonJoinTypeToString(op->join_type));
  push_header("Left child");
  op->left_node->accept(this);
  pop(); // Left child
  push_header("Right child");
  op->right_node->accept(this);
  pop(); // Right child
  push_header("Predicate");
  op->predicate->accept(this);
  pop(); // Join
}

void OperatorPrinter::visit(const OrderBy *op) {
  append("OrderBy: ");
  append("output_column_index " + std::to_string(op->output_list_index));
  append(", ");
  append("equalify_fn " + ExpressionTypeToString(op->equality_fn.exprtype));
  append(", ");
  append("sort_fn " + ExpressionTypeToString(op->sort_fn.exprtype));
  append(", ");
  append("hashable " + std::to_string(op->hashable));
  append(", ");
  append("nulls_first " + std::to_string(op->nulls_first));
  append(", ");
  append("reverse " + std::to_string(op->reverse));

}

void OperatorPrinter::visit(const Select *op) {
  push_header("Select");

  push_header("Join Tree");
  if (op->join_tree) {
    op->join_tree->accept(this);
    append_line();
  }
  pop(); // Join Tree

  push_header("Where Predicate");
  if (op->where_predicate) {
    op->where_predicate->accept(this);
    append_line();
  }
  pop(); // Where predicate

  push_header("Output list");
  for (size_t i = 0; i < op->output_list.size(); ++i) {
    Attribute *attr = op->output_list[i];
    attr->accept(this);
    append_line();
  }
  pop(); // Output list

  push_header("Orderings");
  for (size_t i = 0; i < op->orderings.size(); ++i) {
    OrderBy *ordering = op->orderings[i];
    ordering->accept(this);
    append_line();
  }
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
  if (!new_line_ || string != "") {
    append(string + "\n");
    new_line_ = true;
  }
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

std::string PrintOperator(Select *op) {
  OperatorPrinter printer(op);
  return printer.print();
}

} /* namespace optimizer */
} /* namespace peloton */
