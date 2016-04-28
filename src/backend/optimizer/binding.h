//===----------------------------------------------------------------------===//
//
//                         Peloton
//
// binding.h
//
// Identification: src/backend/optimizer/binding.h
//
// Copyright (c) 2015-16, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include "backend/optimizer/operator_node.h"
#include "backend/optimizer/group.h"
#include "backend/optimizer/pattern.h"

#include <map>
#include <tuple>
#include <memory>

namespace peloton {
namespace optimizer {

class Optimizer;

//===--------------------------------------------------------------------===//
// Binding
//===--------------------------------------------------------------------===//
class Binding {
 public:
  Binding(std::tuple<GroupID, size_t> item_key);

  std::tuple<GroupID, size_t> GetRootKey() const;

  void PushBinding(const Binding& binding);

  void PopBinding();

  const std::vector<std::tuple<GroupID, size_t>> &GetItemChildMapping(
    const std::tuple<GroupID, size_t>& key) const;

 private:
  // Mapping from a specific operator in a group (keyed by the GroupID and index
  // into the group) to the index of the bound operators in the child groups
  // of the keyed operator
  std::tuple<GroupID, size_t> root_key;

  std::map<std::tuple<GroupID, size_t>,
           std::vector<std::tuple<GroupID, size_t>>> binding_mapping;

  std::vector<std::vector<std::tuple<GroupID, size_t>>> pushed_binding_keys;
  std::vector<std::tuple<GroupID, size_t>> pushed_root_keys;
};

//===--------------------------------------------------------------------===//
// Binding Iterator
//===--------------------------------------------------------------------===//
class BindingIterator {
 public:
  BindingIterator(Optimizer &optimizer);

  virtual ~BindingIterator() {};

  virtual bool HasNext() = 0;

  virtual Binding Next() = 0;

 protected:
  Optimizer &optimizer;
  std::vector<Group> &groups;
};

class GroupBindingIterator : public BindingIterator {
 public:
  GroupBindingIterator(Optimizer &optimizer,
                       GroupID id,
                       std::shared_ptr<Pattern> pattern);

  bool HasNext() override;

  Binding Next() override;

 private:
  GroupID group_id;
  std::shared_ptr<Pattern> pattern;
  Group &target_group;
  const std::vector<Operator> &target_group_items;
  const std::vector<bool> &target_group_explored;

  size_t current_item_index;
  std::unique_ptr<BindingIterator> current_iterator;
};

class ItemBindingIterator : public BindingIterator {
 public:
  ItemBindingIterator(Optimizer &optimizer,
                      GroupID id,
                      size_t item_index,
                      std::shared_ptr<Pattern> pattern);

  bool HasNext() override;

  Binding Next() override;

 private:
  GroupID group_id;
  size_t item_index;
  std::shared_ptr<Pattern> pattern;

  bool has_next;
  Binding current_binding;
  std::vector<std::vector<Binding>> children_bindings;
  std::vector<size_t> children_bindings_pos;
};

} /* namespace optimizer */
} /* namespace peloton */
