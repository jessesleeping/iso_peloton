//===----------------------------------------------------------------------===//
//
//                         Peloton
//
// property_set.h
//
// Identification: src/backend/optimizer/property_set.h
//
// Copyright (c) 2015-16, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include "backend/optimizer/property.h"

#include <vector>

namespace peloton {
namespace optimizer {

class PropertySet {
 public:
  PropertySet();

  bool IsSubset(const PropertySet &r);

 private:
  std::vector<Property> properties;
};

} /* namespace optimizer */
} /* namespace peloton */
