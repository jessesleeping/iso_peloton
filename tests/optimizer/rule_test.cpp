//===----------------------------------------------------------------------===//
//
//                         Peloton
//
// rule_test.cpp
//
// Identification: tests/optimizer/rule_test.cpp
//
// Copyright (c) 2015-16, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "harness.h"

#define private public

#include "backend/optimizer/optimizer.h"
#include "backend/optimizer/rule.h"
#include "backend/optimizer/rule_impls.h"
#include "backend/optimizer/op_plan_node.h"
#include "backend/optimizer/logical_operators.h"

namespace peloton {
namespace test {

//===--------------------------------------------------------------------===//
// Binding Tests
//===--------------------------------------------------------------------===//

using namespace optimizer;

class RuleTests : public PelotonTest {};

TEST_F(RuleTests, SimpleRuleApplyTest) {
  Optimizer optimizer;

  // Make groups to apply rule against
  std::vector<Group> &groups = optimizer.groups;

  GroupID right_id;
  GroupID left_id;
  GroupID join_id;
  {
    Group right_group;
    right_group.add_item(LogicalGet::make(0, {0, 1}));
    groups.push_back(right_group);
    right_id = groups.size() - 1;

    Group left_group;
    left_group.add_item(LogicalGet::make(1, {0}));
    groups.push_back(left_group);
    left_id = groups.size() - 1;

    Group join_group;
    join_group.add_item(LogicalInnerJoin::make(left_id, right_id));
    groups.push_back(join_group);
    join_id = groups.size() - 1;
  }

  // Build op plan node to match rule
  auto left_get = std::make_shared<OpPlanNode>(groups, left_id, 0);
  auto right_get = std::make_shared<OpPlanNode>(groups, right_id, 0);

  auto join = std::make_shared<OpPlanNode>(groups, join_id, 0);
  join->PushChild(left_get);
  join->PushChild(right_get);

  // Setup rule
  InnerJoinCommutativity rule;

  EXPECT_TRUE(rule.Check(join));

  std::vector<std::shared_ptr<OpPlanNode>> outputs;
  rule.Transform(join, outputs);
  EXPECT_EQ(outputs.size(), 1);
}

} /* namespace test */
} /* namespace peloton */
