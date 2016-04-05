//===----------------------------------------------------------------------===//
//
//                         Peloton
//
// query_tree.h
//
// Identification: src/backend/optimizer/query_tree.h
//
// Copyright (c) 2015-16, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include "backend/planner/abstract_plan.h"
#include "backend/common/logger.h"

#include "postgres.h"
#include "nodes/nodes.h"
#include "nodes/parsenodes.h"
#include "nodes/plannodes.h"
#include "nodes/params.h"

namespace peloton {
namespace optimizer {

//===--------------------------------------------------------------------===//
// Query Tree
//===--------------------------------------------------------------------===//

class QueryTree {
 public:
  QueryTree(const QueryTree &) = delete;
  QueryTree &operator=(const QueryTree &) = delete;
  QueryTree(QueryTree &&) = delete;
  QueryTree &operator=(QueryTree &&) = delete;

  QueryTree() {};
};

} /* namespace optimizer */
} /* namespace peloton */
