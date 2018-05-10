#ifndef SMCTS_MCTS_STATE_NODE_H_
#define SMCTS_MCTS_STATE_NODE_H_

#include <iostream>
#include <limits>
#include <vector>

namespace mcts {

template<class S>
struct StateNode {
  S state;
  std::vector<StateNode<S>> child_nodes;
  int visit_count = 0;
  float reward = 0.0;
  float total_q = 0.0;
  float max_q = std::numeric_limits<float>::lowest();
  bool is_final = false;
  bool is_fully_expanded = false;

  explicit StateNode(const S& state_) {
    state = state_;
    reward = state.reward();
    is_final = state.isFinal();
  }

  explicit StateNode(S&& state_) {
    state = std::move(state_);
    reward = state.reward();
    is_final = state.isFinal();
  }

  StateNode(const StateNode& b) = delete;

  StateNode(StateNode&& b) noexcept {
    *this = std::move(b);
  };

  StateNode& operator=(const StateNode& b) = delete;

  StateNode& operator=(StateNode&& b) noexcept {
    if (this != &b) {
      state = std::move(b.state);
      is_final = b.is_final;
      visit_count = b.visit_count;
      reward = b.reward;
      total_q = b.total_q;
      max_q = b.max_q;
      is_fully_expanded = b.is_fully_expanded;

      // Needs to come last because b might be a child node of this node
      // and gets deallocated with the following move assignment
      child_nodes = std::move(b.child_nodes);
    }
    return *this;
  }

  void createChildNodes() {
    std::vector<S> successors;
    state.successorStates(&successors);

    for (auto& successor : successors)
      child_nodes.emplace_back(successor);
  }

  bool fully_expanded() {
    return is_fully_expanded;
  }
};

}  // namespace mcts

#endif  // SMCTS_MCTS_STATE_NODE_H_
