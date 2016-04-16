//===----------------------------------------------------------------------===//
//
//                         Peloton
//
// query_transformer.h
//
// Identification: src/backend/optimizer/query_transformer.h
//
// Copyright (c) 2015-16, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include "backend/optimizer/optimizer.h"
#include "backend/optimizer/query_operators.h"

#include "nodes/nodes.h"
#include "nodes/parsenodes.h"
#include "nodes/plannodes.h"
#include "nodes/params.h"

namespace peloton {
namespace optimizer {

//===--------------------------------------------------------------------===//
// Postgres to Peloton QueryTransformer
//===--------------------------------------------------------------------===//

class QueryTransformer {
 public:
  QueryTransformer(const QueryTransformer &) = delete;
  QueryTransformer &operator=(const QueryTransformer &) = delete;
  QueryTransformer(QueryTransformer &&) = delete;
  QueryTransformer &operator=(QueryTransformer &&) = delete;

  QueryTransformer();

  Select *Transform(Query *pg_query);

 private:
  PelotonJoinType TransformJoinType(const JoinType type);

  QueryExpression *ConvertVar(Var *expr);

  QueryExpression *ConvertConst(Const *expr);

  QueryExpression *ConvertBoolExpr(BoolExpr *expr);

  QueryExpression *ConvertOpExpr(OpExpr *expr);

  QueryExpression *ConvertPostgresExpression(Node *expr);

  OrderBy *ConvertSortGroupClause(SortGroupClause *sort_clause,
                                  List *targetList);

  Attribute *ConvertTargetEntry(TargetEntry *te);

  Table *ConvertRangeTblEntry(RangeTblEntry *rte);

  QueryJoinNode *ConvertRangeTblRef(RangeTblRef *expr);

  QueryJoinNode *ConvertJoinExpr(JoinExpr *expr);

  QueryJoinNode *ConvertFromTreeNode(Node *node);

  std::pair<QueryJoinNode *, QueryExpression *> ConvertFromExpr(FromExpr *from);

  Select *ConvertQuery(Query *pg_query);

  //===--------------------------------------------------------------------===//
  // Member variables for tracking partial query state
  //===--------------------------------------------------------------------===//

  // List of all range table entries in original query rtable
  std::vector<RangeTblEntry*> rte_entries;

  // Current expression trees inner and outer tables

};

} /* namespace optimizer */
} /* namespace peloton */
