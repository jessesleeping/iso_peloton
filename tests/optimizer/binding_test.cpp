//===----------------------------------------------------------------------===//
//
//                         Peloton
//
// binding_test.cpp
//
// Identification: tests/optimizer/binding_test.cpp
//
// Copyright (c) 2015-16, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "harness.h"

#include "backend/optimizer/binding.h"
#include "backend/optimizer/logical_operators.h"

namespace peloton {
namespace test {

//===--------------------------------------------------------------------===//
// Binding Tests
//===--------------------------------------------------------------------===//

using namespace optimizer;

class BindingTests : public PelotonTest {};

TEST_F(BindingTests, SimpleMatchTest) {
  // Make groups to match against
  std::vector<Group> groups;

  GroupID root_group_id;
  {
    Group right_group;
    right_group.add_item(LogicalGet::make(0, {0, 1}));
    groups.push_back(right_group);
    GroupID right_id = groups.size() - 1;

    Group left_group;
    left_group.add_item(LogicalGet::make(1, {0}));
    groups.push_back(left_group);
    GroupID left_id = groups.size() - 1;

    Group join_group;
    join_group.add_item(LogicalInnerJoin::make(left_id, right_id));
    groups.push_back(join_group);
    GroupID join_id = groups.size() - 1;

    Group root_group;
    root_group.add_item(LogicalProject::make(join_id));
    groups.push_back(root_group);
    root_group_id = groups.size() - 1;
  }

  // Make pattern to detect
  auto right_relation = std::make_shared<Pattern>(OpType::Get);
  auto left_relation = std::make_shared<Pattern>(OpType::Get);
  auto join = std::make_shared<Pattern>(OpType::InnerJoin);
  auto root = std::make_shared<Pattern>(OpType::Project);

  join->add_child(right_relation);
  join->add_child(left_relation);
  root->add_child(join);

  GroupBindingIterator iter(groups, root_group_id, root);

  EXPECT_TRUE(iter.HasNext());
  Binding binding = iter.Next();
  EXPECT_EQ(binding.GetRootKey(), std::make_tuple(root_group_id, 0));
  EXPECT_EQ(binding.GetItemChildMapping(binding.GetRootKey()).size(), 1);
}

} /* namespace test */
} /* namespace peloton */
