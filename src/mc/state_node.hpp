#ifndef SMCTS_MC__STATE_NODE_H_
#define SMCTS_MC__STATE_NODE_H_

#include <algorithm>
#include <iostream>
#include <limits>
#include <vector>
#include "../util/random_util.hpp"

namespace mc {

template<class S>
struct StateNode {
  S state;
  std::vector<StateNode<S>> child_nodes;
  int visit_count = 0;
  float reward = 0.0;
  float total_q = 0.0;
  float max_q = std::numeric_limits<float>::lowest();

  explicit StateNode(const S& state_) {
    state = state_;
    reward = state.reward();
  }

  void expand() {
    std::vector<S> successors;
    state.successorStates(&successors);
    std::shuffle(successors.begin(), successors.end(), random_util::engine);

    for (auto& successor : successors)
      child_nodes.emplace_back(successor);
  }
};

}  // namespace mc

#endif  // SMCTS_MC__STATE_NODE_H_
