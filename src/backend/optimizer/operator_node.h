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

#include "backend/optimizer/util.h"

#include <string>
#include <memory>

namespace peloton {
namespace optimizer {

enum class OpType {
  Undefined,
  // Special match operators
  Leaf,
  Tree,
  // Logical ops
  Get,
  Project,
  Filter,
  InnerJoin,
  LeftJoin,
  RightJoin,
  OuterJoin,
  Aggregate,
  Limit,
  // Physical ops
  Scan,
  InnerHashJoin,
  LeftHashJoin,
  RightHashJoin,
  OuterHashJoin,
  // Exprs
  Variable,
  Constant,
  Compare,
  BoolOp,
  Op,
};

//===--------------------------------------------------------------------===//
// Operator Node
//===--------------------------------------------------------------------===//
class OperatorVisitor;

struct BaseOperatorNode {
  BaseOperatorNode() {}

  virtual ~BaseOperatorNode() {}

  virtual void accept(OperatorVisitor *v) const = 0;

  virtual std::string name() const = 0;

  virtual OpType type() const = 0;

  virtual bool is_logical() const = 0;

  virtual bool is_physical() const = 0;

  virtual bool operator==(const BaseOperatorNode &r) {
    return type() == r.type();
  }

  virtual hash_t Hash() const {
    OpType t = type();
    return util::Hash(&t);
  }
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

  hash_t Hash() const;

  bool defined() const;

  template <typename T> const T *as() const {
    if (node && typeid(*node) == typeid(T)) {
      return (const T *)node.get();
    }
    return nullptr;
  }

private:
  std::shared_ptr<BaseOperatorNode> node;
};

} /* namespace optimizer */
} /* namespace peloton */

namespace std {

template<> struct hash<peloton::optimizer::BaseOperatorNode> {
  typedef peloton::optimizer::BaseOperatorNode argument_type;
  typedef std::size_t result_type;
  result_type operator()(argument_type const& s) const {
    return s.Hash();
  }
};

} /* namespace std */
