//===----------------------------------------------------------------------===//
//
//                         Peloton
//
// op_expression.cpp
//
// Identification: src/backend/optimizer/op_expression.cpp
//
// Copyright (c) 2015-16, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "backend/optimizer/op_expression.h"

#include <limits>

namespace peloton {
namespace optimizer {

//===--------------------------------------------------------------------===//
// Operator Expression
//===--------------------------------------------------------------------===//
OpExpression::OpExpression(std::vector<Group> &groups,
               GroupID id,
               size_t item_index)
  : id(id), item_index(item_index), op(groups[id].GetOperators()[item_index])
{
}

OpExpression::OpExpression(Operator op)
  : id(UNDEFINED_GROUP), item_index(0), op(op)
{
}

void OpExpression::PushChild(std::shared_ptr<OpExpression> op) {
  children.push_back(op);
}

void OpExpression::PopChild() {
  children.pop_back();
}

const std::vector<std::shared_ptr<OpExpression>> &OpExpression::Children() const {
  return children;
}

void OpExpression::Accept(OpExpressionVisitor *v) const {
  (void)v;
}

GroupID OpExpression::ID() const {
  return id;
}

size_t OpExpression::ItemIndex() const {
  return item_index;
}

const Operator &OpExpression::Op() const {
  return op;
}

} /* namespace optimizer */
} /* namespace peloton */
