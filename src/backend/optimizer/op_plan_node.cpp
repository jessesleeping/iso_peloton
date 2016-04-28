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

namespace peloton {
namespace optimizer {

//===--------------------------------------------------------------------===//
// Operator Plan Node
//===--------------------------------------------------------------------===//
OpPlanNode::OpPlanNode(Operator op)
  : op(op)
{
}

void OpPlanNode::add_child(std::shared_ptr<OpPlanNode> op) {
  children.push_back(op);
}

void OpPlanNode::accept(OpPlanVisitor *v) const {
  (void)v;
}

std::shared_ptr<OpPlanNode> BindingToOpPlan(const std::vector<Group> &groups,
                                            const Binding &binding)
{
  return BindingToOpPlan(groups, binding, binding.GetRootKey());
}

std::shared_ptr<OpPlanNode>
BindingToOpPlan(const std::vector<Group> &groups,
                const Binding &binding,
                const std::tuple<GroupID, size_t> &root_key)
{
  std::shared_ptr<OpPlanNode> root_plan(
    std::make_shared<OpPlanNode>(
      groups[std::get<0>(root_key)].GetOperators()[std::get<1>(root_key)]));

  for (const std::tuple<GroupID, size_t> &child_key :
         binding.GetItemChildMapping(root_key))
  {
    root_plan->add_child(BindingToOpPlan(groups, binding, child_key));
  }
  return root_plan;
}

} /* namespace optimizer */
} /* namespace peloton */
