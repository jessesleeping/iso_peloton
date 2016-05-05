//===----------------------------------------------------------------------===//
//
//                         Peloton
//
// property_set.cpp
//
// Identification: src/backend/optimizer/property_set.cpp
//
// Copyright (c) 2015-16, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "backend/optimizer/property_set.h"

namespace peloton {
namespace optimizer {

PropertySet::PropertySet() {
}

bool PropertySet::IsSubset(const PropertySet &r) {
  (void) r;
  return true;
}

hash_t PropertySet::Hash() const {
  size_t prop_size = properties.size();
  hash_t hash = util::Hash<size_t>(&prop_size);
  for (auto &prop : properties) {
    hash = util::CombineHashes(hash, prop->Hash());
  }
  return hash;
}

bool PropertySet::operator==(const PropertySet &r) {
  for (auto &left_prop : properties) {
    bool match = false;
    for (auto &right_prop : r.properties) {
      match = match || (*left_prop == *right_prop);
    }
    if (!match) return false;
  }
  return true;
}

} /* namespace optimizer */
} /* namespace peloton */
