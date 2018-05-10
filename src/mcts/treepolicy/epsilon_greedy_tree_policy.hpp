#ifndef SMCTS_MCTS_TREE_POLICY_EPSILON_GREEDY_TREE_POLICY_H_
#define SMCTS_MCTS_TREE_POLICY_EPSILON_GREEDY_TREE_POLICY_H_

#include <boost/container/small_vector.hpp>
#include <cmath>
#include <iostream>
#include <limits>
#include "../../util/random_util.hpp"

namespace mcts {

template<class StateNode>
class EpsilonGreedyTreePolicy {
  static constexpr float kFloatEpsilon = 1e-5;

 public:
  float epsilon() const {
    return epsilon_;
  }

  void set_epsilon(float epsilon) {
    epsilon_ = epsilon;
  }

 protected:
  ~EpsilonGreedyTreePolicy() = default;

  StateNode* bestChildNode(StateNode& parent_node) {
    assert(!parent_node.child_nodes.empty());

    std::uniform_real_distribution<float> dist_epsilon(0.0, 1.0);
    if (dist_epsilon(random_util::engine) <= epsilon_) {
      // Return a random child node
      std::uniform_int_distribution<> dist_indices(0, parent_node.child_nodes.size() - 1);
      return &parent_node.child_nodes[dist_indices(random_util::engine)];
    }

    // Return the best child node
    boost::container::small_vector<StateNode*, 3> best_child_nodes;
    float best_mean_q = std::numeric_limits<float>::lowest();

    for (auto& child_node : parent_node.child_nodes) {
      float mean_q = child_node.total_q / child_node.visit_count;
      if (mean_q > best_mean_q + kFloatEpsilon) {
        best_mean_q = mean_q;
        best_child_nodes.clear();
        best_child_nodes.push_back(&child_node);
      }
      else if (mean_q > best_mean_q - kFloatEpsilon) {
        best_child_nodes.push_back(&child_node);
      }
    }

    std::uniform_int_distribution<> dist_indices(0, best_child_nodes.size() - 1);
    return best_child_nodes[dist_indices(random_util::engine)];
  }

 private:
  float epsilon_ = 0.5;

  template<class S>
  friend std::ostream& operator<<(std::ostream& out, const EpsilonGreedyTreePolicy<S>& policy);
};

template<class S>
std::ostream& operator<<(std::ostream& out, const EpsilonGreedyTreePolicy<S>& policy) {
  return out << "Epsilon Greedy (epsilon = " << policy.epsilon()
             << ", float epsilon = " << EpsilonGreedyTreePolicy<S>::kFloatEpsilon << ")\n";
}

}  // namespace mcts

#endif  // SMCTS_MCTS_TREE_POLICY_EPSILON_GREEDY_TREE_POLICY_H_
