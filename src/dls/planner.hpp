#ifndef SMCTS_DLS_PLANNER_H_
#define SMCTS_DLS_PLANNER_H_

#include <cassert>
#include <cmath>
#include <iostream>
#include <limits>
#include <unordered_set>
#include <vector>
#include "../util/random_util.hpp"

namespace dls {

template<class S>
class Planner {
  static constexpr float kEpsilon = 1e-6;

 public:
  int horizon() const {
    return horizon_;
  }

  void set_horizon(int horizon) {
    assert(horizon > 0);
    horizon_ = horizon;
  }

  S bestAction(const S& state) {
    float best_reward_sum = std::numeric_limits<float>::lowest();
    std::unordered_set<S> best_states = {state};

    StateNode root{state, 0.0};
    root.expand();
    std::vector<StateNode*> path;
    path.push_back(&root);

    while (true) {
      StateNode& current_node = *path.back();

      if (current_node.child_nodes.empty()) {
        if (current_node.reward_sum > best_reward_sum + kEpsilon) {
          best_reward_sum = current_node.reward_sum;
          best_states.clear();
          best_states.insert(path[1]->state);
        }
        else if (current_node.reward_sum > best_reward_sum - kEpsilon) {
          best_states.insert(path[1]->state);
        }

        do {
          path.pop_back();

          if (path.empty()) {
            std::uniform_int_distribution<> dist_indices(0, best_states.size() - 1);
            return *std::next(best_states.begin(), dist_indices(random_util::engine));
          }

          path.back()->child_nodes.pop_back();
        } while (path.back()->child_nodes.empty());
      }
      else {
        StateNode& next_node = current_node.child_nodes.back();
        path.push_back(&next_node);

        if (static_cast<int>(path.size()) < (horizon_ + 1) && !current_node.state.isFinal())
          next_node.expand();
      }
    }
  }

 private:
  int horizon_ = 1;

  struct StateNode {
    S state;
    std::vector<StateNode> child_nodes;
    float reward_sum;

    explicit StateNode(S _state, float _reward_sum) : state(_state), reward_sum(_reward_sum) {}

    void expand() {
      std::vector<S> successors;
      state.successorStates(&successors);
      child_nodes.reserve(successors.size());

      for (auto& successor : successors)
        child_nodes.emplace_back(successor, reward_sum + successor.reward());
    }
  };

  template<class S_>
  friend std::ostream& operator<<(std::ostream& out, const Planner<S_>& planner);
};

template<class S>
std::ostream& operator<<(std::ostream& out, const Planner<S>& planner) {
  out << "Depth-limited Search Planner\n";
  out << "Horizon: " << planner.horizon() << "\n";
  out << "Float Epsilon: " << Planner<S>::kEpsilon << "\n";
  return out;
}

}  // namespace dls

#endif  // SMCTS_DLS_PLANNER_H_
