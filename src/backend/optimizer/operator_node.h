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

enum class OpType {
  Undefined,
  // Special match operators
  Leaf,
  Tree,
  //
  Get,
  Project,
  Filter,
  InnerJoin,
  LeftJoin,
  RightJoin,
  OuterJoin,
  Aggregate,
  Limit,
};

//===--------------------------------------------------------------------===//
// Operator Node
//===--------------------------------------------------------------------===//
class OperatorVisitor;

struct BaseOperatorNode {
  BaseOperatorNode() {};

  virtual void accept(OperatorVisitor *v) const = 0;

  virtual std::string name() const = 0;

  virtual OpType type() const = 0;

  virtual bool is_logical() const = 0;

  virtual bool is_physical() const = 0;
};

// Curiously recurring template pattern
template<typename T>
struct OperatorNode : public BaseOperatorNode {
  void accept(OperatorVisitor *v) const;

  std::string name() const { return _name; }

  OpType type() const { return _type; }

  bool is_logical() const;

  bool is_physical() const;

  static std::string _name;

  static OpType _type;
};

class Operator {
public:
  Operator();

  Operator(BaseOperatorNode* node);

  void accept(OperatorVisitor *v) const;

  std::string name() const;

  OpType type() const;

  bool is_logical() const;

  bool is_physical() const;

  bool defined() const;

  template<typename T> const T *as() const;

private:
  std::shared_ptr<BaseOperatorNode> node;
};

//===--------------------------------------------------------------------===//
// Leaf
//===--------------------------------------------------------------------===//
class LeafOperator : OperatorNode<LeafOperator> {
};

} /* namespace optimizer */
} /* namespace peloton */
