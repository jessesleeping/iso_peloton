//===----------------------------------------------------------------------===//
//
//                         Peloton
//
// binding.cpp
//
// Identification: src/backend/optimizer/binding.cpp
//
// Copyright (c) 2015-16, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "backend/optimizer/binding.h"
#include "backend/optimizer/operator_visitor.h"

#include <cassert>

namespace peloton {
namespace optimizer {

namespace {

class ChildVisitor : public OperatorVisitor {
 public:
  std::vector<GroupID> GetChildren(Operator op) {
    op.accept(this);
    return children;
  }

 private:
  void visit(const LogicalGet* op) {
    (void)op;
  }

  void visit(const LogicalProject* op) {
    children.push_back(op->child);
  }

  void visit(const LogicalFilter* op) {
    children.push_back(op->child);
  }

  void visit(const LogicalInnerJoin* op) {
    children.push_back(op->outer);
    children.push_back(op->inner);
  }

  void visit(const LogicalLeftJoin* op) {
    children.push_back(op->outer);
    children.push_back(op->inner);
  }

  void visit(const LogicalRightJoin* op) {
    children.push_back(op->outer);
    children.push_back(op->inner);
  }

  void visit(const LogicalOuterJoin* op) {
    children.push_back(op->outer);
    children.push_back(op->inner);
  }

  void visit(const LogicalAggregate* op) {
    children.push_back(op->child);
  }

  void visit(const LogicalLimit* op) {
    children.push_back(op->child);
  }

  std::vector<GroupID> children;
};

}

//===--------------------------------------------------------------------===//
// Binding
//===--------------------------------------------------------------------===//
Binding::Binding(std::tuple<GroupID, size_t> item_key)
  : root_key(item_key)
{
  binding_mapping.insert({root_key, {}});
}

std::tuple<GroupID, size_t> Binding::GetRootKey() const {
  return root_key;
}

void Binding::PushBinding(const Binding &binding) {
  pushed_binding_keys.resize(pushed_binding_keys.size() + 1);
  std::vector<std::tuple<GroupID, size_t>>& inserted_keys =
    pushed_binding_keys.back();
  for (const auto& kv : binding.binding_mapping) {
    binding_mapping.insert(kv);
    inserted_keys.push_back(kv.first);
  }
  // Add child key
  binding_mapping.at(root_key).push_back(binding.GetRootKey());
  pushed_root_keys.push_back(binding.GetRootKey());
}

void Binding::PopBinding() {
  assert(!pushed_binding_keys.empty());

  for (const std::tuple<GroupID, size_t>& key : pushed_binding_keys.back()) {
    assert(binding_mapping.erase(key) == 1);
  }
  pushed_binding_keys.pop_back();

  std::tuple<GroupID, size_t> binding_key = pushed_root_keys.back();
  pushed_root_keys.pop_back();

  auto& root_mappings = binding_mapping.at(root_key);
  bool found = false;
  for (auto it = root_mappings.begin(); it != root_mappings.end(); ++it) {
    if (*it == binding_key) {
      root_mappings.erase(it);
      found = true;
      break;
    }
  }
  assert(found);
}

const std::vector<std::tuple<GroupID, size_t>>& Binding::GetItemChildMapping(
  const std::tuple<GroupID, size_t>& key) const
{
  return binding_mapping.at(key);
}

//===--------------------------------------------------------------------===//
// Group Binding Iterator
//===--------------------------------------------------------------------===//
GroupBindingIterator::GroupBindingIterator(const std::vector<Group> &groups,
                                           GroupID id,
                                           std::shared_ptr<Pattern> pattern)
  : groups(groups),
    group_id(id),
    pattern(pattern),
    target_group(groups[id]),
    target_group_items(target_group.GetOperators()),
    current_item_index(0)
{
}

bool GroupBindingIterator::HasNext() {
  if (current_iterator) {
    // Check if still have bindings in current item
    if (!current_iterator->HasNext()) {
      current_iterator.reset(nullptr);
      current_item_index++;
    }
  }

  if (current_iterator == nullptr) {
    // Keep checking item iterators until we find a match
    while (current_item_index < target_group_items.size()) {
      current_iterator.reset(
        new ItemBindingIterator(groups, group_id, current_item_index, pattern));

      if (current_iterator->HasNext()) {
        break;
      }

      current_iterator.reset(nullptr);
      current_item_index++;
    }
  }

  return current_iterator != nullptr;
}

Binding GroupBindingIterator::Next() {
  return current_iterator->Next();
}

//===--------------------------------------------------------------------===//
// Item Binding Iterator
//===--------------------------------------------------------------------===//
ItemBindingIterator::ItemBindingIterator(const std::vector<Group> &groups,
                                         GroupID id,
                                         size_t item_index,
                                         std::shared_ptr<Pattern> pattern)
  : groups(groups),
    group_id(id),
    item_index(item_index),
    pattern(pattern),
    has_next(false),
    current_binding(std::make_tuple(id, item_index))
{
  Operator item = groups[id].GetOperators()[item_index];
  if (item.type() != pattern->type()) return;

  std::vector<GroupID> child_groups = ChildVisitor().GetChildren(item);
  const std::vector<std::shared_ptr<Pattern>> &child_patterns =
    pattern->Children();

  if (child_groups.size() != child_patterns.size()) return;

  // Find all bindings for children
  children_bindings.resize(child_groups.size(), {});
  children_bindings_pos.resize(child_groups.size(), 0);
  for (size_t i = 0; i < child_groups.size(); ++i) {
    // Try to find a match in the given group
    std::vector<Binding>& child_bindings = children_bindings[i];
    GroupBindingIterator iterator(groups,
                                  child_groups[i],
                                  child_patterns[i]);

    // Get all bindings
    while (iterator.HasNext()) {
      child_bindings.push_back(iterator.Next());
    }

    if (child_bindings.size() == 0) return;

    current_binding.PushBinding(child_bindings[0]);
  }

  has_next = true;
}

bool ItemBindingIterator::HasNext() {
  return has_next;
}

Binding ItemBindingIterator::Next() {
  Binding ret_binding = current_binding;

  size_t size = children_bindings_pos.size();
  size_t i;
  for (i = 0; i < size; ++i) {
    current_binding.PopBinding();

    const std::vector<Binding>& child_binding =
      children_bindings[size - 1 - i];

    size_t new_pos = ++children_bindings_pos[size - 1 - i];
    if (new_pos < child_binding.size()) {
      break;
    } else {
      children_bindings_pos[size - 1 - i] = 0;
    }
  }

  if (i == size) {
    // We have explored all combinations of the child bindings
    has_next = false;
  } else {
    // Replay to end
    size_t offset = size - 1 - i;
    for (size_t j = 0; j < i + 1; ++j) {
      const std::vector<Binding>& child_binding = children_bindings[offset + j];
      Binding binding = child_binding[children_bindings_pos[offset + j]];
      current_binding.PushBinding(binding);
    }
  }

  return ret_binding;
}

} /* namespace optimizer */
} /* namespace peloton */
