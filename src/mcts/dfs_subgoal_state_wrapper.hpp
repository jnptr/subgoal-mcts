#ifndef SMCTS_MCTS_DFS_SUBGOAL_STATE_WRAPPER_H_
#define SMCTS_MCTS_DFS_SUBGOAL_STATE_WRAPPER_H_

#include <algorithm>
#include <iostream>
#include <unordered_set>
#include <vector>

namespace mcts {

template<class State>
class DfsSubgoalStateWrapper {
 private:
  State state_;
  float reward_;

 public:
  DfsSubgoalStateWrapper() : DfsSubgoalStateWrapper(State()) {}

  explicit DfsSubgoalStateWrapper(State state) : state_(state), reward_(state.reward()) {}

  void evolve(int /*max_steps*/, float /*gamma*/) {
    reward_ = 0.0;

    do {
      state_.evolve(1, 1.0);
      reward_ += state_.reward();
    } while (!state_.isSubgoal() && !state_.isFinal());
  }

  void successorStates(std::vector<DfsSubgoalStateWrapper>* v) const {
    std::unordered_set<State> known_states;
    std::vector<State> stack;
    state_.successorStates(&stack);

    while (!stack.empty()) {
      const State& current_state = stack.back();

      if (known_states.count(current_state) > 0) {
        stack.pop_back();
      }
      else {
        known_states.insert(current_state);

        if (current_state.isSubgoal() || current_state.isFinal()) {
          // TODO: Use correct cumulative reward
          v->emplace_back(current_state);
          stack.pop_back();
        }
        else {
          State current_state_copy = current_state;
          stack.pop_back();
          current_state_copy.successorStates(&stack);
        }
      }
    }
  }

  bool isFinal() const {
    return state_.isFinal();
  }

  float reward() const {
    return reward_;
  }

  int numberOfSteps() const {
    return 1;
  }

  const State& state() const {
    return state_;
  }

  bool operator==(const DfsSubgoalStateWrapper& rhs) const {
    return state_ == rhs.state_;
  }

  friend std::ostream& operator<<(std::ostream& out, const DfsSubgoalStateWrapper& s) {
    out << s.state_ << "\n";
    out << "Subgoal reward: " << s.state_.reward() << "\n";
    return out << "Macro-action reward: " << s.reward_ << "\n";
  }

  friend struct std::hash<DfsSubgoalStateWrapper>;
};

}  // namespace mcts

namespace std {
template<class S>
struct hash<mcts::DfsSubgoalStateWrapper<S>> {
  size_t operator()(const mcts::DfsSubgoalStateWrapper<S>& s) const noexcept {
    return std::hash<S>{}(s.state_);
  }
};
}  // namespace std

#endif  // SMCTS_MCTS_DFS_SUBGOAL_STATE_WRAPPER_H_
