#ifndef SMCTS_MCTS_EVALUATION_POLICY_MAX_Q_EVALUATION_POLICY_H_
#define SMCTS_MCTS_EVALUATION_POLICY_MAX_Q_EVALUATION_POLICY_H_

#include <cmath>
#include <iostream>
#include <limits>
#include <unordered_map>
#include <vector>

namespace mcts {

template<class StateNode>
class MaxQEvaluationPolicy {
 protected:
  ~MaxQEvaluationPolicy() = default;

  StateNode const& evaluate(const std::vector<StateNode>& trees) {
    const StateNode* best_state_node = nullptr;
    float best_value = std::numeric_limits<float>::lowest();

    for (auto& root : trees) {
      for (auto& child_node : root.child_nodes) {
        if (child_node.max_q > best_value) {
          best_value = child_node.max_q;
          best_state_node = &child_node;
        }
      }
    }

    return *best_state_node;
  }
};

template<class S>
std::ostream& operator<<(std::ostream& out, const MaxQEvaluationPolicy<S>& /*policy*/) {
  return out << "Max Q\n";
}

}  // namespace mcts

#endif  // SMCTS_MCTS_EVALUATION_POLICY_MAX_Q_EVALUATION_POLICY_H_
