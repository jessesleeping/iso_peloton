//===----------------------------------------------------------------------===//
//
//                         Peloton
//
// epoch_manager.h
//
// Identification: src/backend/concurrency/epoch_manager.h
//
// Copyright (c) 2015-16, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//
#pragma once

#include <thread>
#include <vector>

#include "backend/common/macros.h"
#include "backend/common/types.h"
#include "backend/common/platform.h"

namespace peloton {
namespace concurrency {

#define EPOCH_LENGTH 40

struct Epoch {
  std::atomic<int> ro_txn_ref_count_;
  std::atomic<int> rw_txn_ref_count_;
  cid_t max_cid_;
  cid_t max_readonly_cid_;

  Epoch()
    :ro_txn_ref_count_(0), rw_txn_ref_count_(0),max_cid_(0),max_readonly_cid_(0) {}

  void Init() {
    ro_txn_ref_count_ = 0;
    rw_txn_ref_count_ = 0;
    max_cid_ = 0;
    max_readonly_cid_ = 0;
  }
};

/*
  Epoch queue layout:
     current epoch               readonly tail              gc tail
    /                           /                          /
    +--------+--------+--------+--------+--------+--------+--------+-------
    | head   | safety |  ....  |readonly| safety |  ....  |gc usage|  ....
    +--------+--------+--------+--------+--------+--------+--------+-------
    New                                                   Old

  Note:
    1) readonly tail epoch and epochs which is older than it have 0 rw txn ref count
    2) gc tail epoch and epochs which is older than it have 0 ro && 0 rw txn ref count
    3) gc tail <= readonly tail
    4) readonly tail is at least 2 turns older than the head epoch
*/

class EpochManager {
  EpochManager(const EpochManager&) = delete;
  static const int safety_interval_ = 2;

 public:
  EpochManager()
 : epoch_queue_(epoch_queue_size_),
   readonly_tail_(0), gc_tail_(0), current_epoch_(0),
   readonly_tail_token_(true), gc_tail_token_(true),
    max_cid_ro_(READ_ONLY_START_CID), max_cid_gc_(0), finish_(false) {
    //ts_thread_.reset(new std::thread(&EpochManager::Start, this));
    //ts_thread_->detach();
    ts_thread_ = std::thread(&EpochManager::Start, this);
  }

  void Reset() {
    finish_ = true;
    ts_thread_.join();

    InitEpochQueue();

    readonly_tail_token_ = true;
    gc_tail_token_ = true;
    max_cid_ro_ = READ_ONLY_START_CID;

    finish_ = false;
    ts_thread_ = std::thread(&EpochManager::Start, this);
  }

  ~EpochManager() {
    finish_ = true;
    ts_thread_.join();
  }

  size_t EnterReadOnlyEpoch(cid_t begin_cid) {
    auto epoch = current_epoch_.load();

    size_t epoch_idx = epoch % epoch_queue_size_;
    epoch_queue_[epoch_idx].ro_txn_ref_count_++;

    // Set the max cid in the tuple
    // auto max_cid_ptr = &(epoch_queue_[epoch_idx].max_cid_);
    auto max_rcid_ptr = &(epoch_queue_[epoch_idx].max_readonly_cid_);
    AtomicMax(max_rcid_ptr, begin_cid);
    
    return epoch;
  }

  size_t EnterEpoch(cid_t begin_cid) {
    auto epoch = current_epoch_.load();

    size_t epoch_idx = epoch % epoch_queue_size_;
    epoch_queue_[epoch_idx].rw_txn_ref_count_++;

    // Set the max cid in the tuple
    auto max_cid_ptr = &(epoch_queue_[epoch_idx].max_cid_);
    AtomicMax(max_cid_ptr, begin_cid);

    return epoch;
  }

  void ExitReadOnlyEpoch(size_t epoch) {
    // PL_ASSERT(epoch > gc_tail_);
    // PL_ASSERT(epoch <= readonly_tail_);

    auto epoch_idx = epoch % epoch_queue_size_;
    epoch_queue_[epoch_idx].ro_txn_ref_count_--;
  }

  void ExitEpoch(size_t epoch) {
    // PL_ASSERT(epoch > readonly_tail_);
    // PL_ASSERT(epoch <= current_epoch_);

    auto epoch_idx = epoch % epoch_queue_size_;
    epoch_queue_[epoch_idx].rw_txn_ref_count_--;
  }

  // assume we store epoch_store max_store previously
  cid_t GetMaxDeadTxnCid() {
    IncreaseReadOnlyTail();
    IncreaseGCTail();
    return max_cid_gc_;

    // Note: Comment out the above three lines and uncomment the following
    // 2 lines to disable 2 phase epoch
//    cid_t res = GetReadOnlyTxnCid();
//    return (res > START_CID) ? res : 0;
  }

  cid_t GetReadOnlyTxnCid() {
    IncreaseReadOnlyTail();
    return max_cid_ro_;
  }

 private:
  void Start() {
    while (!finish_) {
      // the epoch advances every 40 milliseconds.
      std::this_thread::sleep_for(std::chrono::milliseconds(EPOCH_LENGTH));

      auto next_idx = (current_epoch_.load() + 1) % epoch_queue_size_;
      auto tail_idx = gc_tail_.load() % epoch_queue_size_;

//      if (current_epoch_.load() % 10 == 0) {
//        fprintf(stderr, "head = %d, queue_tail = %d, reclaim_tail = %d\n",
//                (int)current_epoch_.load(),
//                (int)readonly_tail_.load(),
//                (int)gc_tail_.load()
//        );
//        fprintf(stderr, "gc_ts = %d, ro_ts = %d\n", (int)max_cid_gc_, (int)max_cid_ro_);
//      }

      if(next_idx  == tail_idx) {
        // overflow
        // in this case, just increase tail
        IncreaseReadOnlyTail();
        IncreaseGCTail();
        continue;
      }

      // we have to init it first, then increase current epoch
      // otherwise may read dirty data
      epoch_queue_[next_idx].Init();
      current_epoch_++;

      IncreaseReadOnlyTail();
      IncreaseGCTail();
    }
  }

  void IncreaseGCTail() {
    bool expect = true, desired = false;
    if(!gc_tail_token_.compare_exchange_weak(expect, desired)){
      // someone now is increasing tail
      return;
    }

    auto current = readonly_tail_.load();
    auto tail = gc_tail_.load();

    while(true) {
      if(tail > current) {
        break;
      }

      auto idx = tail % epoch_queue_size_;

      // inc tail until we find an epoch that has running txn
      if(epoch_queue_[idx].ro_txn_ref_count_ > 0) {
        break;
      }

      // save max cid
      auto max_ = std::max(epoch_queue_[idx].max_cid_, epoch_queue_[idx].max_readonly_cid_);
      AtomicMax(&max_cid_gc_, max_);
      tail++;
      gc_tail_ = tail;
    }

    expect = false;
    desired = true;

    gc_tail_token_.compare_exchange_weak(expect, desired);
    return;
  }

  void IncreaseReadOnlyTail() {
    bool expect = true, desired = false;
    if(!readonly_tail_token_.compare_exchange_weak(expect, desired)){
      // someone now is increasing tail
      return;
    }

    auto current = current_epoch_.load();
    auto tail = readonly_tail_.load();

    while(true) {
      if(tail + safety_interval_ >= current) {
        break;
      }

      auto idx = tail % epoch_queue_size_;

      // inc tail until we find an epoch that has running txn
      if(epoch_queue_[idx].rw_txn_ref_count_ > 0) {
        break;
      }

      // save max cid
      auto max_ = epoch_queue_[idx].max_cid_;
      AtomicMax(&max_cid_ro_, max_);
      tail++;
      readonly_tail_ = tail;
    }

    expect = false;
    desired = true;

    readonly_tail_token_.compare_exchange_weak(expect, desired);
    return;
  }


  void AtomicMax(cid_t* addr, cid_t max) {
    while(true) {
      auto old = *addr;
      if(old > max) {
        return;
      }else if ( __sync_bool_compare_and_swap(addr, old, max) ) {
        return;
      }
    }
  }

//  inline size_t GetReclaimTailIdx() {
//    return (readonly_tail_ - 2) % epoch_queue_size_;
//  }

  inline void InitEpochQueue() {
    for (int i = 0; i <= safety_interval_; ++i) {
      epoch_queue_[i].Init();
    }

    current_epoch_ = safety_interval_;
    readonly_tail_ = 0;
    gc_tail_ = 0;
  }

private:
  // queue size
  static const size_t epoch_queue_size_ = 4096;

  // Epoch vector
  std::vector<Epoch> epoch_queue_;
  std::atomic<size_t> readonly_tail_;
  std::atomic<size_t> gc_tail_;
  std::atomic<size_t> current_epoch_;
  std::atomic<bool> readonly_tail_token_;
  std::atomic<bool> gc_tail_token_;
  cid_t max_cid_ro_;
  cid_t max_cid_gc_;
  bool finish_;

  std::thread ts_thread_;
};


class EpochManagerFactory {
 public:
  static EpochManager &GetInstance() {
    static EpochManager epoch_manager;
    return epoch_manager;
  }
};

}
}

