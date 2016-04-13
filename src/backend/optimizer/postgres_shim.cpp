//===----------------------------------------------------------------------===//
//
//                         Peloton
//
// postgres_shim.cpp
//
// Identification: src/backend/optimizer/postgres_shim.cpp
//
// Copyright (c) 2015-16, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "c.h"
#include "postgres.h"
#include "nodes/memnodes.h"

#include "backend/optimizer/postgres_shim.h"
#include "backend/optimizer/query_operators.h"
#include "backend/optimizer/operator_printer.h"

#include "access/attnum.h"
#include "miscadmin.h"
#include "storage/lock.h"
#include "access/heapam.h"
#include "utils/rel.h"
#include "catalog/pg_namespace.h"

namespace peloton {
namespace optimizer {

namespace {

// Forward declare
Operator convertParsenodes(Node *node);

OrderBy *convert_SortGroupClause(
  SortGroupClause *sort_clause,
  List *targetList)
{
  OrderBy *order_op = nullptr;

  // Find the output this sort clause corresponds to
  int output_list_index = 0;
  ListCell *o_target;
  foreach(o_target, targetList) {
    TargetEntry *tle = (TargetEntry *) lfirst(o_target);
    if (tle->ressortgroupref == sort_clause->tleSortGroupRef) {
      break;
    }
    output_list_index++;
  }

  order_op = OrderBy::make(output_list_index,
                           bridge::kPgFuncMap[sort_clause->eqop],
                           bridge::kPgFuncMap[sort_clause->sortop],
                           sort_clause->hashable,
                           sort_clause->nulls_first);

  return order_op;
}

TableAttribute *convert_TargetEntry(TargetEntry *te) {
  // Fail if the target is anything other than basic column reference
  if (te->resorigtbl == 0) return nullptr;

  if (!IsA(te->expr, Var)) return nullptr;

  Var *var = reinterpret_cast<Var*>(te->expr);

  int table_list_index = var->varno - 1;
  AttrNumber column_index = var->varattno;
  return TableAttribute::make(table_list_index, column_index);
}

Table *convert_RangeTblEntry(RangeTblEntry *rte) {
  Table *table = nullptr;
  switch (rte->rtekind) {
  case RTE_RELATION: {
    if (rte->relkind == RELKIND_RELATION) {
      table = Table::make(rte->relid);
    }
    break;
  }
  default:
    break;
  }
  return table;
}

bool check_FromExpr(FromExpr *from) {
  ListCell *o_target;
  foreach(o_target, from->fromlist) {
    if (!IsA(lfirst(o_target), RangeTblRef)) {
      return false;
    }
  }
  if (from->quals != nullptr) return false;

  return true;
}

Select *convert_Query(Query *pg_query) {
  Select *query = nullptr;
  if (pg_query->commandType == CMD_SELECT) {
    // Check if HAVING clause - fail for now
    if (pg_query->havingQual) return query;
    // Check if WINDOW functions - fail for now
    if (pg_query->hasWindowFuncs) return query;
    // Check if aggregate - fail for now
    if (pg_query->hasAggs) return query;

    std::vector<std::shared_ptr<Table>> table_list;
    // Convert range table list
	ListCell *o_target;
    foreach(o_target, pg_query->rtable) {
      RangeTblEntry *rte = static_cast<RangeTblEntry *>(lfirst(o_target));
      std::shared_ptr<Table> table_entry{convert_RangeTblEntry(rte)};
      // Fail if unsupported range table entry
      if (table_entry == nullptr) return query;
      table_list.push_back(table_entry);
    }

    std::vector<std::shared_ptr<TableAttribute>> output_list;
    // Convert target list
    foreach(o_target, pg_query->targetList) {
      TargetEntry *tle = static_cast<TargetEntry*>(lfirst(o_target));

      // Can ignore for now?
      if (tle->resjunk) continue;

      std::shared_ptr<TableAttribute> table_attribute{convert_TargetEntry(tle)};
      if (table_attribute == nullptr) return query;

      output_list.push_back(table_attribute);
    }

    // Convert sort clauses
    std::vector<std::shared_ptr<OrderBy>> orderings;
    if (pg_query->sortClause) {
      List *sort_list = pg_query->sortClause;
      ListCell *list_item;

      foreach(list_item, sort_list) {
        SortGroupClause *sort_clause = (SortGroupClause *) lfirst(list_item);
        convert_SortGroupClause(sort_clause, pg_query->targetList);
      }
    }
    // Check join tree - fail if anything more than range table references
    assert(pg_query->jointree);
    if (!check_FromExpr(static_cast<FromExpr*>(pg_query->jointree)))
      return query;

    // Check if predicates

    query = Select::make(table_list, output_list, orderings);
  }
  return query;
}

Operator convertParsenodes(Node *node) {
  Operator op{};
  (void)node;
  // switch (node->type) {
  //   // primnodes.h
  // case T_Alias: {
  //   break;
  // }
  // case T_RangeVar: {
  //   break;
  // }
  // case T_Expr: {
  //   break;
  // }
  // case T_Var: {
  //   break;
  // }
  // case T_Const: {
  //   break;
  // }
  // case T_Param: {
  //   break;
  // }
  // case T_Aggref: {
  //   break;
  // }
  // case T_GroupingFunc: {
  //   break;
  // }
  // case T_NamedArgExpr: {
  //   break;
  // }
  // case T_OpExr: {
  //   break;
  // }
  // case T_BoolExpr: {
  //   break;
  // }
  // case T_TargetEntry: {
  //   break;
  // }
  // case T_RangeTblRef: {
  //   break;
  // }
  // case T_JoinExpr: {
  //   break;
  // }
  // case T_FromExpr: {
  //   op = convert_FromExpr((FromExpr*)node);
  //   break;
  // }
  //   // parsenodes.h
  // case T_Query: {
  //   op = convert_Query((Query*)node);
  //   break;
  // }
  // case T_ColumnRef: {
  //   break;
  // }
  // case T_ParamRef: {
  //   break;
  // }
  // case T_A_Const: {
  //   break;
  // }
  // case T_FuncCall: {
  //   break;
  // }
  // case T_SortBy: {
  //   break;
  // }
  // case T_RangeTblEntry: {
  //   break;
  // }
  // case T_RangeTblFunction: {
  //   break;
  // }
  // case T_SortGroupClause: {
  //   break;
  // }
  // }

  return op;
}


} // anonymous namespace

//===--------------------------------------------------------------------===//
// Compatibility with Postgres
//===--------------------------------------------------------------------===//
bool ShouldPelotonOptimize(Query *parse) {
  bool should_optimize = false;

  if (!IsPostmasterEnvironment && !IsBackend) {
    return false;
  }

  if (parse != NULL && parse->rtable != NULL) {
    ListCell *l;

    // Go over each relation on which the plan depends
    foreach(l, parse->rtable) {
      RangeTblEntry *rte = static_cast<RangeTblEntry *>(lfirst(l));

      if (rte->rtekind == RTE_RELATION) {
        Oid relation_id = rte->relid;
        // Check if relation in public namespace
        Relation target_table = relation_open(relation_id, AccessShareLock);
        Oid target_table_namespace = target_table->rd_rel->relnamespace;

        should_optimize = (target_table_namespace == PG_PUBLIC_NAMESPACE);

        relation_close(target_table, AccessShareLock);

        if (should_optimize)
          break;
      }
    }
  }
  return should_optimize;
}

std::shared_ptr<Select> PostgresQueryToPelotonQuery(Query *parse) {
  std::shared_ptr<Select> select_query{convert_Query(parse)};
  return select_query;
}

std::shared_ptr<planner::AbstractPlan>
PelotonOptimize(
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

  std::shared_ptr<Select> query_tree = PostgresQueryToPelotonQuery(parse);
  std::shared_ptr<planner::AbstractPlan> plan;
  if (query_tree) {
    LOG_DEBUG("Succesfully converted postgres query to Peloton query");
    LOG_DEBUG("Peloton query:\n %s", PrintOperator(query_tree.get()).c_str());
    LOG_DEBUG("Invoking peloton optimizer...");
    plan = optimizer.GeneratePlan(query_tree);
    if (plan) {
      LOG_DEBUG("Succesfully converted Peloton query into Peloton plan");
    }
  }

  return plan;
  // PlannedStmt *result = nullptr;
  // if (plan) {
  //   LOG_DEBUG("Created Peloton plan");
  //   result = makeNode(PlannedStmt);

  //   // result->commandType = parse->commandType;
  //   // result->queryId = parse->queryId;
  //   // result->hasReturning = (parse->returningList != NIL);
  //   // result->hasModifyingCTE = parse->hasModifyingCTE;
  //   // result->canSetTag = parse->canSetTag;
  //   // result->transientPlan = glob->transientPlan;
  //   // result->planTree = top_plan;
  //   // result->rtable = glob->finalrtable;
  //   // result->resultRelations = glob->resultRelations;
  //   // result->utilityStmt = parse->utilityStmt;
  //   // result->subplans = glob->subplans;
  //   // result->rewindPlanIDs = glob->rewindPlanIDs;
  //   // result->rowMarks = glob->finalrowmarks;
  //   // result->relationOids = glob->relationOids;
  //   // result->invalItems = glob->invalItems;
  //   // result->nParamExec = glob->nParamExec;
  //   // result->hasRowSecurity = glob->hasRowSecurity;
  //   result->pelotonQuery = true;
  //   result->pelotonOptimized = true;
  // }

  //return result;
}

}
}
