#ifndef SMCTS_MCTS_EXPANSION_POLICY_TRADITIONAL_EXPANSION_POLICY_H_
#define SMCTS_MCTS_EXPANSION_POLICY_TRADITIONAL_EXPANSION_POLICY_H_

#include <algorithm>
#include <iostream>
#include "../../util/random_util.hpp"

namespace mcts {

template<class StateNode>
class TraditionalExpansionPolicy {
 protected:
  ~TraditionalExpansionPolicy() = default;

  StateNode* expand(StateNode& parent_node, int /*max_steps*/, float /*gamma*/) {
    if (parent_node.child_nodes.empty()) {
      parent_node.createChildNodes();
      std::shuffle(
          parent_node.child_nodes.begin(), parent_node.child_nodes.end(), random_util::engine);
    }

    for (auto& child_node : parent_node.child_nodes) {
      if (child_node.visit_count == 0)
        return &child_node;
    }

    parent_node.is_fully_expanded = true;
    return nullptr;
  }
};

template<class S>
std::ostream& operator<<(std::ostream& out, const TraditionalExpansionPolicy<S>& /*policy*/) {
  out << "Traditional\n";
  return out;
}

}  // namespace mcts

#endif  // SMCTS_MCTS_EXPANSION_POLICY_TRADITIONAL_EXPANSION_POLICY_H_
