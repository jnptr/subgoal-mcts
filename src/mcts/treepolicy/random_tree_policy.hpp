#ifndef SMCTS_MCTS_TREE_POLICY_RANDOM_TREE_POLICY_H_
#define SMCTS_MCTS_TREE_POLICY_RANDOM_TREE_POLICY_H_

#include <cmath>
#include <iostream>
#include "../../util/random_util.hpp"

namespace mcts {

template<class StateNode>
class RandomTreePolicy {
 protected:
  ~RandomTreePolicy() = default;

  StateNode* bestChildNode(StateNode& parent_node) {
    assert(!parent_node.child_nodes.empty());

    std::uniform_int_distribution<> dist_indices(0, parent_node.child_nodes.size() - 1);
    return &parent_node.child_nodes[dist_indices(random_util::engine)];
  }

 private:
  template<class S>
  friend std::ostream& operator<<(std::ostream& out, const RandomTreePolicy<S>& policy);
};

template<class S>
std::ostream& operator<<(std::ostream& out, const RandomTreePolicy<S>& /*policy*/) {
  return out << "Random \n";
}

}  // namespace mcts

#endif  // SMCTS_MCTS_TREE_POLICY_RANDOM_TREE_POLICY_H_
