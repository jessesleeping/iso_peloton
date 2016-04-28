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
#include "backend/optimizer/binding.h"

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
  OpPlanNode(Operator op);

  void add_child(std::shared_ptr<OpPlanNode> op);

  void accept(OpPlanVisitor *v) const;

private:
  Operator op;

  std::vector<std::shared_ptr<OpPlanNode>> children;
};

std::shared_ptr<OpPlanNode> BindingToOpPlan(const std::vector<Group> &groups,
                                            const Binding &binding);

std::shared_ptr<OpPlanNode>
BindingToOpPlan(const std::vector<Group> &groups,
                const Binding &binding,
                const std::tuple<GroupID, size_t> &root_key);

} /* namespace optimizer */
} /* namespace peloton */
