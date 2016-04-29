//===----------------------------------------------------------------------===//
//
//                         Peloton
//
// op_plan_node.h
//
// Identification: src/backend/optimizer/op_plan_node.h
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
// Operator Plan Node
//===--------------------------------------------------------------------===//
class OpPlanVisitor;

class OpPlanNode {
public:
  OpPlanNode(std::vector<Group> &groups,
             GroupID id,
             size_t item_index);

  OpPlanNode(Operator op);

  void PushChild(std::shared_ptr<OpPlanNode> op);

  void PopChild();

  const std::vector<std::shared_ptr<OpPlanNode>> &Children() const;

  void Accept(OpPlanVisitor *v) const;

  GroupID ID() const;

  size_t ItemIndex() const;

  const Operator &Op() const;

private:
  GroupID id;
  size_t item_index;
  Operator op;

  std::vector<std::shared_ptr<OpPlanNode>> children;
};

} /* namespace optimizer */
} /* namespace peloton */
