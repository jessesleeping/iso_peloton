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
  return false;
}

} /* namespace optimizer */
} /* namespace peloton */
