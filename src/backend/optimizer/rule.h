//===----------------------------------------------------------------------===//
//
//                         Peloton
//
// rule.h
//
// Identification: src/backend/optimizer/rule.h
//
// Copyright (c) 2015-16, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include "backend/optimizer/binding.h"

#include <memory>

namespace peloton {
namespace optimizer {

class Rule {
 public:
  virtual ~Rule() {};

  std::shared_ptr<Pattern> GetMatchPattern() const { return match_pattern; }

  virtual bool Check(const std::vector<Group> &groups,
                     const Binding &binding) const = 0;

  virtual Operator Transform(Operator item) const = 0;

 private:
  std::shared_ptr<Pattern> match_pattern;
};

} /* namespace optimizer */
} /* namespace peloton */
