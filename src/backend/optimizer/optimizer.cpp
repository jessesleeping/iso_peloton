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

#include "backend/planner/projection_plan.h"
#include "backend/planner/seq_scan_plan.h"
#include "backend/catalog/manager.h"
#include "backend/bridge/ddl/bridge.h"
#include "backend/bridge/dml/mapper/mapper.h"

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
  std::shared_ptr<Select> select_tree)
{
  // Always perform a projection for now
  // planner::ProjectionPlan* proj_plan =
  //   new planner::ProjectionPlan();
  planner::AbstractPlan* top_plan = nullptr;

  // Scan base tables
  oid_t database_oid = bridge::Bridge::GetCurrentDatabaseOid();
  std::vector<planner::AbstractPlan*> scans;

  LOG_DEBUG("Generting SeqScanPlans for %lu tables",
            select_tree->table_list.size());
  for (int table_index = 0;
       table_index < static_cast<int>(select_tree->table_list.size());
       ++table_index)
  {
    auto table = select_tree->table_list[table_index];
    storage::DataTable *target_table = static_cast<storage::DataTable *>(
      catalog::Manager::GetInstance().GetTableWithOid(
        database_oid, table->table_oid));

    expression::AbstractExpression *predicate = nullptr;

    std::vector<oid_t> col_list;
    for (auto table_attr : select_tree->output_list)
    {
      if (table_attr->table_list_index == table_index) {
        col_list.push_back(table_attr->column_index - 1);
      }
    }

    LOG_DEBUG("Adding SeqScanPlan for table %d with %lu columns",
              table_index, col_list.size());
    planner::SeqScanPlan *scan_plan =
      new planner::SeqScanPlan(target_table, predicate, col_list);

    scans.emplace_back(scan_plan);
  }
  if (scans.size() == 1 && select_tree->orderings.size() == 0) {
    LOG_DEBUG("Only one scan, setting final plan");
    top_plan = scans[0];
  } else {
    for (planner::AbstractPlan* plan : scans) {
      delete plan;
    }
  }

  std::shared_ptr<planner::AbstractPlan> final_plan(
    top_plan, bridge::PlanTransformer::CleanPlan);

  return final_plan;
}

}  // namespace optimizer
}  // namespace peloton
