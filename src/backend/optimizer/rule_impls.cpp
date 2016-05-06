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

///////////////////////////////////////////////////////////////////////////////
/// InnerJoinCommutativity
InnerJoinCommutativity::InnerJoinCommutativity() {
  logical = true;

  std::shared_ptr<Pattern> left_child(std::make_shared<Pattern>(OpType::Leaf));
  std::shared_ptr<Pattern> right_child(std::make_shared<Pattern>(OpType::Leaf));
  std::shared_ptr<Pattern> predicate(std::make_shared<Pattern>(OpType::Leaf));
  match_pattern = std::make_shared<Pattern>(OpType::InnerJoin);
  match_pattern->AddChild(left_child);
  match_pattern->AddChild(right_child);
  match_pattern->AddChild(predicate);
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
  (void)inner_join;

  auto result_plan = std::make_shared<OpExpression>(LogicalInnerJoin::make());
  std::vector<std::shared_ptr<OpExpression>> children = input->Children();
  assert(children.size() == 3);
  result_plan->PushChild(children[1]);
  result_plan->PushChild(children[0]);
  result_plan->PushChild(children[2]);

  transformed.push_back(result_plan);
}

///////////////////////////////////////////////////////////////////////////////
/// GetToScan
GetToScan::GetToScan() {
  physical = true;

  match_pattern = std::make_shared<Pattern>(OpType::Get);
}

bool GetToScan::Check(std::shared_ptr<OpExpression> plan) const {
  (void) plan;
  return true;
}

void GetToScan::Transform(
  std::shared_ptr<OpExpression> input,
  std::vector<std::shared_ptr<OpExpression>> &transformed) const
{
  const LogicalGet *get = input->Op().as<LogicalGet>();

  auto result_plan = std::make_shared<OpExpression>(
    PhysicalScan::make(get->table, get->columns));

  transformed.push_back(result_plan);
}

///////////////////////////////////////////////////////////////////////////////
/// ProjectToComputeExprs
ProjectToComputeExprs::ProjectToComputeExprs() {
  physical = true;

  std::shared_ptr<Pattern> child(std::make_shared<Pattern>(OpType::Leaf));
  std::shared_ptr<Pattern> project_list(
    std::make_shared<Pattern>(OpType::Leaf));
  match_pattern = std::make_shared<Pattern>(OpType::Project);
  match_pattern->AddChild(child);
  match_pattern->AddChild(project_list);
}

bool ProjectToComputeExprs::Check(std::shared_ptr<OpExpression> plan) const {
  (void) plan;
  return true;
}

void ProjectToComputeExprs::Transform(
    std::shared_ptr<OpExpression> input,
    std::vector<std::shared_ptr<OpExpression>> &transformed) const
{
  auto result =
    std::make_shared<OpExpression>(PhysicalComputeExprs::make());
  std::vector<std::shared_ptr<OpExpression>> children = input->Children();
  assert(children.size() == 2);
  result->PushChild(children[0]);
  result->PushChild(children[1]);

  transformed.push_back(result);
}

///////////////////////////////////////////////////////////////////////////////
/// SelectToFilter
SelectToFilter::SelectToFilter() {
  physical = true;

  std::shared_ptr<Pattern> child(std::make_shared<Pattern>(OpType::Leaf));
  std::shared_ptr<Pattern> predicate(std::make_shared<Pattern>(OpType::Leaf));
  match_pattern = std::make_shared<Pattern>(OpType::Select);
  match_pattern->AddChild(child);
  match_pattern->AddChild(predicate);
}

bool SelectToFilter::Check(std::shared_ptr<OpExpression> plan) const {
  (void) plan;
  return true;
}

void SelectToFilter::Transform(
  std::shared_ptr<OpExpression> input,
  std::vector<std::shared_ptr<OpExpression>> &transformed) const
{
  auto result =
    std::make_shared<OpExpression>(PhysicalFilter::make());
  std::vector<std::shared_ptr<OpExpression>> children = input->Children();
  assert(children.size() == 2);
  result->PushChild(children[0]);
  result->PushChild(children[1]);

  transformed.push_back(result);
}

///////////////////////////////////////////////////////////////////////////////
/// InnerJoinToInnerHashJoin
InnerJoinToInnerHashJoin::InnerJoinToInnerHashJoin() {
  physical = true;
}

bool InnerJoinToInnerHashJoin::Check(std::shared_ptr<OpExpression> plan) const {
  (void) plan;
  return true;
}

void InnerJoinToInnerHashJoin::Transform(
  std::shared_ptr<OpExpression> input,
  std::vector<std::shared_ptr<OpExpression>> &transformed) const
{
  // first build an expression representing hash join
  auto result_plan = std::make_shared<OpExpression>(PhysicalInnerHashJoin::make());
  std::vector<std::shared_ptr<OpExpression>> children = input->Children();
  assert(children.size() == 3);

  // Then push all children into the child list of the new operator
  result_plan->PushChild(children[0]);
  result_plan->PushChild(children[1]);
  result_plan->PushChild(children[2]);

  transformed.push_back(result_plan);

  return;
}

///////////////////////////////////////////////////////////////////////////////
/// LeftJoinToLeftHashJoin
LeftJoinToLeftHashJoin::LeftJoinToLeftHashJoin() {
  physical = true;
}

bool LeftJoinToLeftHashJoin::Check(std::shared_ptr<OpExpression> plan) const {
  (void) plan;
  return true;
}

void LeftJoinToLeftHashJoin::Transform(
  std::shared_ptr<OpExpression> input,
  std::vector<std::shared_ptr<OpExpression>> &transformed) const
{
  (void) input;
  (void) transformed;
}

///////////////////////////////////////////////////////////////////////////////
/// RightJoinToRightHashJoin
RightJoinToRightHashJoin::RightJoinToRightHashJoin() {
  physical = true;
}

bool RightJoinToRightHashJoin::Check(std::shared_ptr<OpExpression> plan) const {
  (void) plan;
  return true;
}

void RightJoinToRightHashJoin::Transform(
  std::shared_ptr<OpExpression> input,
  std::vector<std::shared_ptr<OpExpression>> &transformed) const
{
  (void) input;
  (void) transformed;
}

///////////////////////////////////////////////////////////////////////////////
/// OuterJoinToOuterHashJoin
OuterJoinToOuterHashJoin::OuterJoinToOuterHashJoin() {
  physical = true;
}

bool OuterJoinToOuterHashJoin::Check(std::shared_ptr<OpExpression> plan) const {
  (void) plan;
  return true;
}

void OuterJoinToOuterHashJoin::Transform(
  std::shared_ptr<OpExpression> input,
  std::vector<std::shared_ptr<OpExpression>> &transformed) const
{
  (void) input;
  (void) transformed;
}

} /* namespace optimizer */
} /* namespace peloton */
