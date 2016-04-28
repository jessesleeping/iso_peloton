//===----------------------------------------------------------------------===//
//
//                         Peloton
//
// rule.h
//
// Identification: src/backend/optimizer/rule.h
//
// Copyright (c) 2015-16, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include "backend/optimizer/binding.h"
#include "backend/optimizer/op_plan_node.h"

#include <memory>

namespace peloton {
namespace optimizer {

class Rule {
 public:
  virtual ~Rule() {};

  std::shared_ptr<Pattern> GetMatchPattern() const { return match_pattern; }

  virtual bool Check(std::shared_ptr<OpPlanNode> plan) const = 0;

  virtual void Transform(
    std::shared_ptr<OpPlanNode> input,
    std::vector<std::shared_ptr<OpPlanNode>> &transformed) const = 0;

 private:
  std::shared_ptr<Pattern> match_pattern;
};

} /* namespace optimizer */
} /* namespace peloton */
