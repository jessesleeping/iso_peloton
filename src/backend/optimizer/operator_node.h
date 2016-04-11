//===----------------------------------------------------------------------===//
//
//                         Peloton
//
// operator_node.h
//
// Identification: src/backend/optimizer/operator_node.h
//
// Copyright (c) 2015-16, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <string>
#include <memory>

namespace peloton {
namespace optimizer {

//===--------------------------------------------------------------------===//
// Operator Node
//===--------------------------------------------------------------------===//
class OperatorVisitor;

struct BaseOperatorNode {
  BaseOperatorNode() {};

  virtual void accept(OperatorVisitor *v) const = 0;
  virtual const std::string& name() const = 0;
};

// Curiously recurring template pattern
template<typename T>
struct OperatorNode : public BaseOperatorNode {
  void accept(OperatorVisitor *v) const;

  virtual const std::string& name() const { return _name; }

  static std::string _name;
};

class Operator {
public:
  Operator();

  Operator(BaseOperatorNode* node);

  void accept(OperatorVisitor *v) const;

  bool defined() const;

  template<typename T> const T *as() const;

private:
  std::shared_ptr<BaseOperatorNode> node;
};

} /* namespace optimizer */
} /* namespace peloton */
