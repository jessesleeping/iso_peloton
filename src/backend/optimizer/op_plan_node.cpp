//===----------------------------------------------------------------------===//
//
//                         Peloton
//
// op_plan_node.cpp
//
// Identification: src/backend/optimizer/op_plan_node.cpp
//
// Copyright (c) 2015-16, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "backend/optimizer/op_plan_node.h"

#include <limits>

namespace peloton {
namespace optimizer {

//===--------------------------------------------------------------------===//
// Operator Plan Node
//===--------------------------------------------------------------------===//
OpPlanNode::OpPlanNode(std::vector<Group> &groups,
                       GroupID id,
                       size_t item_index)
  : id(id), item_index(item_index), op(groups[id].GetOperators()[item_index])
{
}

OpPlanNode::OpPlanNode(Operator op)
  : id(UNDEFINED_GROUP), item_index(0), op(op)
{
}

void OpPlanNode::PushChild(std::shared_ptr<OpPlanNode> op) {
  children.push_back(op);
}

void OpPlanNode::PopChild() {
  children.pop_back();
}

const std::vector<std::shared_ptr<OpPlanNode>> &OpPlanNode::Children() const {
  return children;
}

void OpPlanNode::Accept(OpPlanVisitor *v) const {
  (void)v;
}

GroupID OpPlanNode::ID() const {
  return id;
}

size_t OpPlanNode::ItemIndex() const {
  return item_index;
}

const Operator &OpPlanNode::Op() const {
  return op;
}

} /* namespace optimizer */
} /* namespace peloton */
