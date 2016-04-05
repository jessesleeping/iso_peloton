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

#include "backend/optimizer/optimizer.h"

#include "miscadmin.h"
#include "storage/lock.h"
#include "access/heapam.h"
#include "utils/rel.h"
#include "catalog/pg_namespace.h"

namespace peloton {
namespace optimizer {

//===--------------------------------------------------------------------===//
// Optimizer
//===--------------------------------------------------------------------===//
Optimizer &Optimizer::GetInstance()
{
  thread_local static Optimizer optimizer;
  return optimizer;
}


std::shared_ptr<planner::AbstractPlan> Optimizer::GeneratePlan(
  std::shared_ptr<QueryTree> query_tree)
{
  (void)query_tree;
  return nullptr;
}

//===--------------------------------------------------------------------===//
// Compatibility with Postgres
//===--------------------------------------------------------------------===//
bool ShouldPelotonOptimize(Query *parse) {
  bool should_optimize = false;

  if (IsPostmasterEnvironment == false && IsBackend == false) {
    return false;
  }

  if (parse != NULL && parse->rtable != NULL) {
    ListCell   *l;

    // Go over each relation on which the plan depends
    foreach(l, parse->rtable) {
      RangeTblEntry *rte = static_cast<RangeTblEntry *>(lfirst(l));

      if (rte->rtekind == RTE_RELATION) {
        Oid relation_id = rte->relid;
        // Check if relation in public namespace
        Relation target_table = relation_open(relation_id, AccessShareLock);
        Oid target_table_namespace = target_table->rd_rel->relnamespace;

        if(target_table_namespace == PG_PUBLIC_NAMESPACE) {
          should_optimize = true;
        }

        relation_close(target_table, AccessShareLock);

        if (should_optimize)
          break;
      }
    }
  }

  return should_optimize;
}

PlannedStmt* PelotonOptimize(
  Optimizer &optimizer,
  Query *parse,
  int cursorOptions,
  ParamListInfo boundParams)
{
  (void)optimizer;
  (void)boundParams;
  (void)cursorOptions;

  char* query_output = nodeToString(parse);
  LOG_DEBUG("cursorOptions %d", cursorOptions);
  LOG_DEBUG("Query: %s", query_output);
  (void)query_output;

  PlannedStmt *result = nullptr;
  if (false) {
    result = makeNode(PlannedStmt);

    // result->commandType = parse->commandType;
    // result->queryId = parse->queryId;
    // result->hasReturning = (parse->returningList != NIL);
    // result->hasModifyingCTE = parse->hasModifyingCTE;
    // result->canSetTag = parse->canSetTag;
    // result->transientPlan = glob->transientPlan;
    // result->planTree = top_plan;
    // result->rtable = glob->finalrtable;
    // result->resultRelations = glob->resultRelations;
    // result->utilityStmt = parse->utilityStmt;
    // result->subplans = glob->subplans;
    // result->rewindPlanIDs = glob->rewindPlanIDs;
    // result->rowMarks = glob->finalrowmarks;
    // result->relationOids = glob->relationOids;
    // result->invalItems = glob->invalItems;
    // result->nParamExec = glob->nParamExec;
    // result->hasRowSecurity = glob->hasRowSecurity;
    result->pelotonQuery = true;
    result->pelotonOptimized = true;
  }

  return result;
}


}  // namespace optimizer
}  // namespace peloton
