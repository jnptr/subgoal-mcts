#ifndef SMCTS_MCTS_EXPANSION_POLICY_SUBSAMPLING_EXPANSION_POLICY_H_
#define SMCTS_MCTS_EXPANSION_POLICY_SUBSAMPLING_EXPANSION_POLICY_H_

#include <cassert>
#include <cmath>
#include <iostream>

namespace mcts {

template<class StateNode>
class SubsamplingExpansionPolicy {
  static constexpr bool kUpdateReward = true;

 public:
  int threshold() const {
    return threshold_;
  }

  float action_coverage() const {
    return action_coverage_;
  }

  void set_action_coverage(float action_coverage) {
    threshold_ = threshold(action_coverage, tolerated_error_);
    action_coverage_ = action_coverage;
  }

  float tolerated_error() const {
    return tolerated_error_;
  }

  void set_tolerated_error(float tolerated_error) {
    threshold_ = threshold(action_coverage_, tolerated_error);
    tolerated_error_ = tolerated_error;
  }

 protected:
  ~SubsamplingExpansionPolicy() = default;

  StateNode* expand(StateNode& parent_node, int max_steps, float gamma) {
    for (int i = 0; i < threshold_; i++) {
      auto successor_state = parent_node.state;
      successor_state.evolve(max_steps, gamma);

      if (!updateExistingNode(parent_node, std::move(successor_state))) {
        parent_node.child_nodes.emplace_back(std::move(successor_state));
        return &parent_node.child_nodes.back();
      }
    }

    parent_node.is_fully_expanded = true;
    return nullptr;
  }

 private:
  float action_coverage_ = 0.9;
  float tolerated_error_ = 0.1;
  int threshold_ = threshold(action_coverage_, tolerated_error_);

  constexpr int threshold(float action_coverage, float tolerated_error) {
    assert(action_coverage > 0 && action_coverage < 1);
    assert(tolerated_error > 0 && tolerated_error < 1);
    return std::ceil(std::log(tolerated_error) / std::log(action_coverage));
  }

  bool updateExistingNode(StateNode& parent_node, decltype(parent_node.state) && successor_state) {
    float reward = successor_state.reward();
    for (auto& child_node : parent_node.child_nodes) {
      if (child_node.state == successor_state) {
        if (kUpdateReward && reward > child_node.reward) {
          child_node.state = std::move(successor_state);
          child_node.reward = reward;
        }
        return true;
      }
    }
    return false;
  }

  template<class S>
  friend std::ostream& operator<<(std::ostream& out, const SubsamplingExpansionPolicy<S>& policy);
};

template<class S>
std::ostream& operator<<(std::ostream& out, const SubsamplingExpansionPolicy<S>& policy) {
  out << "Subsampling\n";
  out << "  Update Reward: " << (SubsamplingExpansionPolicy<S>::kUpdateReward ? "Yes" : "No")
      << "\n";
  out << "  Action Coverage: " << policy.action_coverage() << "\n";
  out << "  Tolerated Error: " << policy.tolerated_error() << "\n";
  return out;
}

}  // namespace mcts

#endif  // SMCTS_MCTS_EXPANSION_POLICY_SUBSAMPLING_EXPANSION_POLICY_H_
