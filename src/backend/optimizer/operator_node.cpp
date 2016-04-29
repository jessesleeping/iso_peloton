//===----------------------------------------------------------------------===//
//
//                         Peloton
//
// operator_node.cpp
//
// Identification: src/backend/optimizer/operator_node.cpp
//
// Copyright (c) 2015-16, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "backend/optimizer/operator_node.h"

namespace peloton {
namespace optimizer {

//===--------------------------------------------------------------------===//
// Operator
//===--------------------------------------------------------------------===//
Operator::Operator() : node(nullptr) {}

Operator::Operator(BaseOperatorNode* node) : node(node) {}

void Operator::accept(OperatorVisitor *v) const {
  node->accept(v);
}

std::string Operator::name() const {
  if (defined()) {
    return node->name();
  }
  return "Undefined";
}

OpType Operator::type() const {
  if (defined()) {
    return node->type();
  }
  return OpType::Undefined;
}

bool Operator::is_logical() const {
  if (defined()) {
    return node->is_logical();
  }
  return false;
}

bool Operator::is_physical() const {
  if (defined()) {
    return node->is_physical();
  }
  return false;
}

bool Operator::defined() const {
  return node != nullptr;
}

} /* namespace optimizer */
} /* namespace peloton */
