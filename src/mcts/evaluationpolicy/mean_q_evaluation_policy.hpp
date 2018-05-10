#ifndef SMCTS_MCTS_EVALUATION_POLICY_MEAN_Q_EVALUATION_POLICY_H_
#define SMCTS_MCTS_EVALUATION_POLICY_MEAN_Q_EVALUATION_POLICY_H_

#include <cmath>
#include <iostream>
#include <limits>
#include <unordered_map>
#include <vector>

namespace mcts {

template<class StateNode>
class MeanQEvaluationPolicy {
 protected:
  ~MeanQEvaluationPolicy() = default;

  StateNode const& evaluate(const std::vector<StateNode>& trees) {
    std::unordered_map<decltype(StateNode::state), NodeData> root_states;

    for (auto& root : trees) {
      for (auto& child_node : root.child_nodes) {
        root_states[child_node.state] += child_node;
      }
    }

    const StateNode* best_state_node = nullptr;
    float best_mean_q = std::numeric_limits<float>::lowest();

    for (auto& entry : root_states) {
      float mean_q = entry.second.total_q / entry.second.visit_count;
      if (mean_q > best_mean_q) {
        best_mean_q = mean_q;
        best_state_node = entry.second.state_node;
      }
    }

    return *best_state_node;
  }

 private:
  struct NodeData {
    float total_q = 0.0;
    int visit_count = 0;
    const StateNode* state_node = nullptr;

    void operator+=(const StateNode& b) {
      total_q += b.total_q;
      visit_count += b.visit_count;
      state_node = &b;
    }
  };
};

template<class S>
std::ostream& operator<<(std::ostream& out, const MeanQEvaluationPolicy<S>& /*policy*/) {
  return out << "Mean Q\n";
}

}  // namespace mcts

#endif  // SMCTS_MCTS_EVALUATION_POLICY_MEAN_Q_EVALUATION_POLICY_H_
