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
#include "utils/lsyscache.h"
#include "catalog/pg_namespace.h"

namespace peloton {
namespace optimizer {

namespace {



// Forward declare
QueryExpression *convertPostgresExpression(Node *expr);

QueryJoinNode *convertFromTreeNode(
  Node *node,
  std::vector<RangeTblEntry*>& rte_entries);


PelotonJoinType transformJoinType(const JoinType type) {
  switch (type) {
    case JOIN_INNER:
      return JOIN_TYPE_INNER;
    case JOIN_FULL:
      return JOIN_TYPE_OUTER;
    case JOIN_LEFT:
      return JOIN_TYPE_LEFT;
    case JOIN_RIGHT:
      return JOIN_TYPE_RIGHT;
    case JOIN_SEMI:  // IN+Subquery is JOIN_SEMI
      return JOIN_TYPE_SEMI;
    default:
      return JOIN_TYPE_INVALID;
  }
}

QueryExpression *convert_Var(Var *expr) {
  (void)expr;
  return nullptr;
}

QueryExpression *convert_Const(Const *expr) {
  (void)expr;
  return nullptr;
}

QueryExpression *convert_BoolExpr(BoolExpr *expr) {
  QueryExpression *query_expression = nullptr;

  std::vector<QueryExpression *> args;
  ListCell *cell;
  foreach(cell, expr->args) {
    args.push_back(convertPostgresExpression((Node *)lfirst(cell)));
  }

  switch (expr->boolop) {
    case AND_EXPR: {
      query_expression = new AndOperator(args);
      break;
    }
    case OR_EXPR: {
      query_expression = new OrOperator(args);
      break;
    }
    case NOT_EXPR: {
      assert(args.size() == 1);
      query_expression = new NotOperator(args[0]);
      break;
    }
  }
  return query_expression;
}

QueryExpression *convert_OpExpr(OpExpr *expr) {
  (void)expr;
  return nullptr;
}

QueryExpression *convertPostgresExpression(Node *expr) {
  QueryExpression *query_expression = nullptr;
  switch (expr->type) {
    case T_Var: {
      query_expression = convert_Var((Var *)expr);
      break;
    }
    case T_Const: {
      query_expression = convert_Const((Const *)expr);
      break;
    }
    case T_BoolExpr: {
      query_expression = convert_BoolExpr((BoolExpr *)expr);
      break;
    }
    case T_OpExpr: {
      query_expression = convert_OpExpr((OpExpr *)expr);
      break;
    }
    default: {
    }
  }
  return query_expression;
}

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

  Oid opfamily;
  Oid opcintype;
  int16 strategy;
  get_ordering_op_properties(sort_clause->sortop,
                             &opfamily, &opcintype, &strategy);
  bool reverse = (strategy == BTGreaterStrategyNumber);
  order_op = new OrderBy(output_list_index,
                         bridge::kPgFuncMap[sort_clause->eqop],
                         bridge::kPgFuncMap[sort_clause->sortop],
                         sort_clause->hashable,
                         sort_clause->nulls_first,
                         reverse);

  return order_op;
}

Attribute *convert_TargetEntry(TargetEntry *te) {
  // Fail if the target is anything other than basic column reference
  if (te->resorigtbl == 0) return nullptr;

  if (!IsA(te->expr, Var)) return nullptr;

  Var *var = reinterpret_cast<Var*>(te->expr);

  // We subtract one because postgres indexes starting at 1
  // but peloton starts at 0
  int table_list_index = var->varno - 1;
  oid_t column_index =
    static_cast<oid_t>(AttrNumberGetAttrOffset(var->varattno));
  return new Attribute(table_list_index, column_index);
}

Table *convert_RangeTblEntry(RangeTblEntry *rte) {
  Table *table = nullptr;
  switch (rte->rtekind) {
  case RTE_RELATION: {
    if (rte->relkind == RELKIND_RELATION) {
      table = new Table(rte->relid);
    }
    break;
  }
  default:
    break;
  }
  return table;
}

QueryJoinNode *convert_RangeTblRef(RangeTblRef *expr,
                                   std::vector<RangeTblEntry*>& rte_entries) {
  int rte_index = expr->rtindex - 1;
  RangeTblEntry *rte = rte_entries[rte_index];
  assert(rte->rtekind == RTE_RELATION);
  return new Table(rte->relid);
}

QueryJoinNode *convert_JoinExpr(JoinExpr *expr,
                                std::vector<RangeTblEntry*>& rte_entries) {
  // Don't support natural queries yet
  if (expr->isNatural) return nullptr;

  // Don't support using yet
  if (expr->usingClause) return nullptr;

  // Don't support alias yet
  if (expr->alias) return nullptr;

  PelotonJoinType join_type = transformJoinType(expr->jointype);

  QueryJoinNode *left_child = convertFromTreeNode(expr->larg, rte_entries);
  if (left_child == nullptr) return nullptr;

  QueryJoinNode *right_child = convertFromTreeNode(expr->rarg, rte_entries);
  if (right_child == nullptr) return nullptr;

  // Get join predicate
  QueryExpression *predicate = convertPostgresExpression(expr->quals);
  if (predicate == nullptr) return nullptr;

  Join *join = new Join(join_type, left_child, right_child, predicate);

  return join;
}

QueryJoinNode *convertFromTreeNode(
  Node *node,
  std::vector<RangeTblEntry*>& rte_entries)
{
  QueryJoinNode *join_node = nullptr;
  switch (node->type) {
    case T_RangeTblRef: {
      join_node = convert_RangeTblRef((RangeTblRef*)node, rte_entries);
      break;
    }
    case T_JoinExpr: {
      join_node = convert_JoinExpr((JoinExpr*)node, rte_entries);
      break;
    }
    default:
      assert(false);
  }
  return join_node;
}

std::pair<QueryJoinNode *, QueryExpression *> convert_FromExpr(
  FromExpr *from,
  std::vector<RangeTblEntry*>& rte_entries)
{
  // Wha?
  if (from->fromlist == nullptr) {
    return {nullptr, nullptr};
  }

  if (list_length(from->fromlist) > 1) {
    // Don't handle old-style joins yet
    return {nullptr, nullptr};
  }

  assert(list_length(from->fromlist) == 1);

  // There should only be one node because we aren't handling old-style joins
  QueryJoinNode *join_node = nullptr;
  if (from->fromlist) {
      join_node = convertFromTreeNode((Node *)lfirst(list_head(from->fromlist)),
                                      rte_entries);
  }

  QueryExpression *where_predicate = nullptr;
  if (from->quals) {
    where_predicate = convertPostgresExpression(from->quals);
    if (where_predicate == nullptr) {
      return {nullptr, nullptr};
    }
  }

  return {join_node, where_predicate};
}

Select *convert_Query(Query *pg_query) {
  Select *query = nullptr;
  if (pg_query->commandType == CMD_SELECT) {
    // Check if aggregate - fail for now
    if (pg_query->hasAggs) return query;
    // Check if WINDOW functions - fail for now
    if (pg_query->hasWindowFuncs) return query;
    // Check if sub links - fail for now
    if (pg_query->hasSubLinks) return query;
    // Check if distinct on - fail for now
    if (pg_query->hasDistinctOn) return query;
    // Check if WITH RECURSIVE - fail for now
    if (pg_query->hasRecursive) return query;
    // Check if INSERT/UPDATE/DELETE in WITH - fail for now
    if (pg_query->hasModifyingCTE) return query;
    // Check if FOR UPDATE/share - fail for now
    if (pg_query->hasForUpdate) return query;
    // Check if row security applied - fail for now
    if (pg_query->hasRowSecurity) return query;
    // Check if has CTEs - fail for now
    if (pg_query->cteList) return query;
    // Check if withCheckOptions - fail for now
    if (pg_query->withCheckOptions) return query;
    // Check if ON CONFLICT clause - fail for now
    if (pg_query->onConflict) return query;
    // Check if GROUP clause - fail for now
    if (pg_query->groupClause) return query;
    // Check if GROUP clause - fail for now
    if (pg_query->groupingSets) return query;
    // Check if HAVING clause - fail for now
    if (pg_query->havingQual) return query;
    // Check if WINDOW clauses - fail for now
    if (pg_query->windowClause) return query;
    // No limit support
    if (pg_query->limitOffset) return query;
    if (pg_query->limitCount) return query;
    // Check if row marks - fail for now
    if (pg_query->rowMarks) return query;
    // Check if set operations - fail for now
    if (pg_query->setOperations) return query;
    // Check if constraints - fail for now
    if (pg_query->constraintDeps) return query;

    // Convert range table list into vector for easy access
    std::vector<RangeTblEntry*> rte_entries;
	ListCell *o_target;
    foreach(o_target, pg_query->rtable) {
      RangeTblEntry *rte = static_cast<RangeTblEntry *>(lfirst(o_target));
      rte_entries.push_back(rte);
    }

    // Convert join tree
    auto join_and_where_pred =
      convert_FromExpr(pg_query->jointree, rte_entries);
    std::unique_ptr<QueryJoinNode> join_tree(
      join_and_where_pred.first);
    std::unique_ptr<QueryExpression> where_predicate(
      join_and_where_pred.second);
    if (join_tree == nullptr) return query;

    std::vector<std::unique_ptr<Attribute>> output_list;
    // Convert target list
    foreach(o_target, pg_query->targetList) {
      TargetEntry *tle = static_cast<TargetEntry*>(lfirst(o_target));

      // Can ignore for now?
      if (tle->resjunk) continue;

      std::unique_ptr<Attribute> attribute{convert_TargetEntry(tle)};
      if (attribute == nullptr) return query;

      output_list.push_back(std::move(attribute));
    }

    // Convert sort clauses
    std::vector<std::unique_ptr<OrderBy>> orderings;
    if (pg_query->sortClause) {
      List *sort_list = pg_query->sortClause;
      ListCell *list_item;

      foreach(list_item, sort_list) {
        SortGroupClause *sort_clause = (SortGroupClause *) lfirst(list_item);
        OrderBy* order =
          convert_SortGroupClause(sort_clause, pg_query->targetList);
        orderings.emplace_back(order);
      }
    }

    {
      std::vector<Attribute *> attrs;
      for (std::unique_ptr<Attribute>& at : output_list) {
        attrs.push_back(at.release());
      }
      std::vector<OrderBy *> ords;
      for (std::unique_ptr<OrderBy>& ord : orderings) {
        ords.push_back(ord.release());
      }
      query = new Select(join_tree.release(),
                         where_predicate.release(),
                         attrs,
                         ords);
    }
  }
  return query;
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
