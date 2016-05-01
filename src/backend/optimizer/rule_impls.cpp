//===----------------------------------------------------------------------===//
//
//                         Peloton
//
// rule_impls.cpp
//
// Identification: src/backend/optimizer/rule_impls.cpp
//
// Copyright (c) 2015-16, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "backend/optimizer/rule_impls.h"
#include "backend/optimizer/operators.h"

#include <memory>

namespace peloton {
namespace optimizer {

InnerJoinCommutativity::InnerJoinCommutativity() {
  std::shared_ptr<Pattern> left_child(std::make_shared<Pattern>(OpType::Leaf));
  std::shared_ptr<Pattern> right_child(std::make_shared<Pattern>(OpType::Leaf));
  match_pattern = std::make_shared<Pattern>(OpType::InnerJoin);
  match_pattern->AddChild(left_child);
  match_pattern->AddChild(right_child);
}

bool InnerJoinCommutativity::Check(std::shared_ptr<OpExpression> expr) const {
  (void)expr;
  return true;
}

void InnerJoinCommutativity::Transform(
  std::shared_ptr<OpExpression> input,
  std::vector<std::shared_ptr<OpExpression>> &transformed) const
{
  const LogicalInnerJoin *inner_join = input->Op().as<LogicalInnerJoin>();
  assert(inner_join != nullptr);
  GroupID left = inner_join->outer;
  GroupID right = inner_join->inner;
  auto result_plan =
    std::make_shared<OpExpression>(LogicalInnerJoin::make(right, left));
  transformed.push_back(result_plan);
}

} /* namespace optimizer */
} /* namespace peloton */
