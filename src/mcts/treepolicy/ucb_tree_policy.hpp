#ifndef SMCTS_MCTS_TREE_POLICY_UCB_TREE_POLICY_H_
#define SMCTS_MCTS_TREE_POLICY_UCB_TREE_POLICY_H_

#include <boost/container/small_vector.hpp>
#include <cmath>
#include <iostream>
#include <limits>
#include "../../util/math_util.hpp"
#include "../../util/random_util.hpp"

namespace mcts {

template<class StateNode>
class UcbTreePolicy {
  static constexpr float kEpsilon = 1e-6;

 public:
  float c() const {
    return c_;
  }

  void set_c(float c) {
    c_ = c;
  }

 protected:
  ~UcbTreePolicy() = default;

  StateNode* bestChildNode(StateNode& parent_node) const {
    assert(!parent_node.child_nodes.empty());

    boost::container::small_vector<StateNode*, 3> best_child_nodes;
    float max_ucb = std::numeric_limits<float>::lowest();

    for (auto& child_node : parent_node.child_nodes) {
      float ucb_child = ucb(parent_node, child_node);
      if (ucb_child > max_ucb + kEpsilon) {
        max_ucb = ucb_child;
        best_child_nodes.clear();
        best_child_nodes.push_back(&child_node);
      }
      else if (ucb_child > max_ucb - kEpsilon) {
        best_child_nodes.push_back(&child_node);
      }
    }

    std::uniform_int_distribution<> dist_indices(0, best_child_nodes.size() - 1);
    return best_child_nodes[dist_indices(random_util::engine)];
  }

 private:
  float c_ = 0.1;

  constexpr float ucb(const StateNode& parent_node, const StateNode& child_node) const {
    return ucb(child_node.total_q, child_node.visit_count, parent_node.visit_count);
  }

  constexpr float ucb(float child_total_q, int child_visit_count, int parent_visit_count) const {
    float mean_q = child_total_q / child_visit_count;
    return mean_q +
           c_ * std::sqrt((2 * math_util::fastLog(parent_visit_count)) / child_visit_count);
  }

  template<class S>
  friend std::ostream& operator<<(std::ostream& out, const UcbTreePolicy<S>& policy);
};

template<class S>
std::ostream& operator<<(std::ostream& out, const UcbTreePolicy<S>& policy) {
  return out << "UCT (c = " << policy.c() << ", float epsilon = " << UcbTreePolicy<S>::kEpsilon
             << ")\n";
}

}  // namespace mcts

#endif  // SMCTS_MCTS_TREE_POLICY_UCB_TREE_POLICY_H_
