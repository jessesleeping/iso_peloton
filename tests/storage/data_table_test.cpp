//===----------------------------------------------------------------------===//
//
//                         Peloton
//
// data_table_test.cpp
//
// Identification: tests/storage/data_table_test.cpp
//
// Copyright (c) 2015-16, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "harness.h"

#include "backend/storage/data_table.h"
#include "backend/storage/tile_group.h"
#include "backend/concurrency/transaction_manager_factory.h"
#include "executor/executor_tests_util.h"

namespace peloton {
namespace test {

//===--------------------------------------------------------------------===//
// Data Table Tests
//===--------------------------------------------------------------------===//

class DataTableTests : public PelotonTest {};

TEST_F(DataTableTests, TransformTileGroupTest) {
  const int tuple_count = TESTS_TUPLES_PER_TILEGROUP;

  // Create a table and wrap it in logical tiles
  auto &txn_manager = concurrency::TransactionManagerFactory::GetInstance();
  auto txn = txn_manager.BeginTransaction();
  std::unique_ptr<storage::DataTable> data_table(
      ExecutorTestsUtil::CreateTable(tuple_count, false));
  ExecutorTestsUtil::PopulateTable(txn, data_table.get(), tuple_count, false,
                                   false, true);
  txn_manager.CommitTransaction();

  // Create the new column map
  storage::column_map_type column_map;
  column_map[0] = std::make_pair(0, 0);
  column_map[1] = std::make_pair(0, 1);
  column_map[2] = std::make_pair(1, 0);
  column_map[3] = std::make_pair(1, 1);

  auto theta = 0.0;

  // Transform the tile group
  data_table->TransformTileGroup(0, theta);

  // Create the another column map
  column_map[0] = std::make_pair(0, 0);
  column_map[1] = std::make_pair(0, 1);
  column_map[2] = std::make_pair(0, 2);
  column_map[3] = std::make_pair(1, 0);

  // Transform the tile group
  data_table->TransformTileGroup(0, theta);

  // Create the another column map
  column_map[0] = std::make_pair(0, 0);
  column_map[1] = std::make_pair(1, 0);
  column_map[2] = std::make_pair(1, 1);
  column_map[3] = std::make_pair(1, 2);

  // Transform the tile group
  data_table->TransformTileGroup(0, theta);
}

/*
 * SamplingForOptimizerTest - Tests basic table basic sampling
 */
TEST_F(DataTableTests, SamplingForOptimizerTest) {
  const int tuple_count = TESTS_TUPLES_PER_TILEGROUP;

  // Create a table and wrap it in logical tiles
  auto &txn_manager = concurrency::TransactionManagerFactory::GetInstance();
  auto txn = txn_manager.BeginTransaction();
  std::unique_ptr<storage::DataTable> data_table(
      ExecutorTestsUtil::CreateTable(tuple_count, false));
  ExecutorTestsUtil::PopulateTable(txn, data_table.get(), tuple_count, false,
                                   false, true);
  txn_manager.CommitTransaction();

  size_t sample_size = data_table->SampleRows(10);
  (void)sample_size;

  LOG_TRACE("Sample size = %lu; actual size = %lu\n",
            sample_size,
            data_table->GetOptimizerSampleSize());

  return;
}

}  // End test namespace
}  // End peloton namespace
