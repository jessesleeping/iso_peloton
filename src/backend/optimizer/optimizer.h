//===----------------------------------------------------------------------===//
//
//                         Peloton
//
// optimizer.h
//
// Identification: src/backend/optimizer/optimizer.h
//
// Copyright (c) 2015-16, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include "backend/optimizer/query_operators.h"
#include "backend/planner/abstract_plan.h"
#include "backend/common/logger.h"

#include <memory>

namespace peloton {
namespace optimizer {

//===--------------------------------------------------------------------===//
// Optimizer
//===--------------------------------------------------------------------===//
class Optimizer {
 public:
  Optimizer(const Optimizer &) = delete;
  Optimizer &operator=(const Optimizer &) = delete;
  Optimizer(Optimizer &&) = delete;
  Optimizer &operator=(Optimizer &&) = delete;

  Optimizer() {};

  static Optimizer &GetInstance();

  std::shared_ptr<planner::AbstractPlan> GeneratePlan(
    std::shared_ptr<Select> select_tree);
};

} /* namespace optimizer */
} /* namespace peloton */
