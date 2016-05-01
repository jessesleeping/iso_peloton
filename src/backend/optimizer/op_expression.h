//===----------------------------------------------------------------------===//
//
//                         Peloton
//
// op_expression.h
//
// Identification: src/backend/optimizer/op_expression.h
//
// Copyright (c) 2015-16, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include "backend/optimizer/operator_node.h"
#include "backend/optimizer/group.h"

#include <string>
#include <memory>

namespace peloton {
namespace optimizer {

//===--------------------------------------------------------------------===//
// Operator Expr
//===--------------------------------------------------------------------===//
class OpExpressionVisitor;

class OpExpression {
public:
  OpExpression(std::vector<Group> &groups,
             GroupID id,
             size_t item_index);

  OpExpression(Operator op);

  void PushChild(std::shared_ptr<OpExpression> op);

  void PopChild();

  const std::vector<std::shared_ptr<OpExpression>> &Children() const;

  void Accept(OpExpressionVisitor *v) const;

  GroupID ID() const;

  size_t ItemIndex() const;

  const Operator &Op() const;

private:
  GroupID id;
  size_t item_index;
  Operator op;

  std::vector<std::shared_ptr<OpExpression>> children;
};

} /* namespace optimizer */
} /* namespace peloton */
