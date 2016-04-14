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
#include "backend/planner/order_by_plan.h"

#include "backend/catalog/manager.h"
#include "backend/bridge/ddl/bridge.h"
#include "backend/bridge/dml/mapper/mapper.h"

#include <memory>

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
  std::vector<std::unique_ptr<planner::AbstractPlan>> scans;

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
        col_list.push_back(table_attr->column_index);
      }
    }

    LOG_DEBUG("Adding SeqScanPlan for table %d with %lu columns",
              table_index, col_list.size());
    planner::SeqScanPlan *scan_plan =
      new planner::SeqScanPlan(target_table, predicate, col_list);

    scans.emplace_back(scan_plan);
  }
  if (scans.size() == 1) {
    LOG_DEBUG("Only one scan, setting plan");
    if (top_plan != nullptr) {
      top_plan->AddChild(scans[0].release());
    } else {
      top_plan = scans[0].release();
    }
  } else {
    // Exit because we can't handle multiple scans atm
    return nullptr;
  }
  if (!select_tree->orderings.empty()) {
    LOG_DEBUG("Adding OrderBy with %lu terms", select_tree->orderings.size());
    std::vector<oid_t> sort_keys;
    std::vector<bool> reverse_flags;
    for (size_t i = 0; i < select_tree->orderings.size(); ++i) {
      auto ordering = select_tree->orderings[i];
      sort_keys.push_back(ordering->output_list_index);
      reverse_flags.push_back(ordering->reverse);
    }
    auto order_by = new planner::OrderByPlan(sort_keys, reverse_flags, {});
    order_by->AddChild(top_plan);
    top_plan = order_by;
  }

  std::shared_ptr<planner::AbstractPlan> final_plan(
    top_plan, bridge::PlanTransformer::CleanPlan);

  return final_plan;
}

}  // namespace optimizer
}  // namespace peloton
