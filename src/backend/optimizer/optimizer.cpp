//===----------------------------------------------------------------------===//
//
//                         Peloton
//
// optimizer.cpp
//
// Identification: src/backend/optimizer/optimizer.cpp
//
// Copyright (c) 2015-16, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "backend/optimizer/plan_column.h"
#include "backend/common/exception.h"

#include "nodes/parsenodes.h"

namespace peloton {
namespace optimizer {

Optimizer &Optimizer::GetInstance()
{
  thread_local static Optimizer optimizer;
  return optimizer;
}

planner::AbstractPlan* Optimizer::GeneratePlan(
  Query *parse, int cursorOptions, ParamListInfo boundParams)
{
  assert(false);
  return nullptr;
}

}  // namespace optimizer
}  // namespace peloton
