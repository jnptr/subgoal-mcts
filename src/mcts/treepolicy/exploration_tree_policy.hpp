#ifndef SMCTS_MCTS_TREE_POLICY_EXPLORATION_TREE_POLICY_H_
#define SMCTS_MCTS_TREE_POLICY_EXPLORATION_TREE_POLICY_H_

#include <iostream>
#include <limits>

namespace mcts {

template<class StateNode>
class ExplorationTreePolicy {
 protected:
  ~ExplorationTreePolicy() = default;

  StateNode* bestChildNode(StateNode& parent_node) {
    assert(!parent_node.child_nodes.empty());

    float min_visit_count = std::numeric_limits<float>::max();
    StateNode* selected_node = nullptr;

    for (auto& child_node : parent_node.child_nodes) {
      if (child_node.visit_count < min_visit_count) {
        min_visit_count = child_node.visit_count;
        selected_node = &child_node;
      }
    }

    return selected_node;
  }
};

template<class S>
std::ostream& operator<<(std::ostream& out, const ExplorationTreePolicy<S>& /*policy*/) {
  return out << "Exploration\n";
}

}  // namespace mcts

#endif  // SMCTS_MCTS_TREE_POLICY_EXPLORATION_TREE_POLICY_H_
