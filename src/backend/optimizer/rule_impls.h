//===----------------------------------------------------------------------===//
//
//                         Peloton
//
// rule_impls.h
//
// Identification: src/backend/optimizer/rule_impls.h
//
// Copyright (c) 2015-16, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include "backend/optimizer/rule.h"

#include <memory>

namespace peloton {
namespace optimizer {

class InnerJoinCommutativity : public Rule {
 public:
  InnerJoinCommutativity();

  bool Check(std::shared_ptr<OpPlanNode> plan) const override;

  void Transform(
    std::shared_ptr<OpPlanNode> input,
    std::vector<std::shared_ptr<OpPlanNode>> &transformed) const override;
};

} /* namespace optimizer */
} /* namespace peloton */
