#ifndef SMCTS_MCTS_SUBSAMPLING_HISTORY_WRAPPER_H_
#define SMCTS_MCTS_SUBSAMPLING_HISTORY_WRAPPER_H_

#include <algorithm>
#include <iostream>
#include <vector>

namespace mcts {

template<class State>
class SubsamplingHistoryWrapper {
 private:
  std::vector<State> states_;
  float reward_;

 public:
  SubsamplingHistoryWrapper() : SubsamplingHistoryWrapper(State()) {}

  explicit SubsamplingHistoryWrapper(State state) : reward_(state.reward()) {
    states_.push_back(state);
  }

  void evolve(int max_steps, float gamma) {
    reward_ = 0.0;
    State state = states_.back();
    states_.clear();
    float current_discount = 1.0;

    do {
      state.evolve(1, 1.0);
      states_.push_back(state);
      reward_ += current_discount * state.reward();
      current_discount *= gamma;
    } while (!state.isSubgoal() && !state.isFinal() &&
             static_cast<int>(states_.size()) < max_steps);
  }

  void truncate() {
    reward_ = states_.front().reward();
    states_.erase(states_.begin() + 1, states_.end());
  }

  bool isFinal() const {
    return states_.back().isFinal();
  }

  float reward() const {
    return reward_;
  }

  int numberOfSteps() const {
    return states_.size();
  }

  const State& state() const {
    return states_.back();
  }

  bool operator==(const SubsamplingHistoryWrapper& rhs) const {
    return states_.back() == rhs.states_.back();
  }

  friend std::ostream& operator<<(std::ostream& out, const SubsamplingHistoryWrapper& s) {
    for (const auto& state : s.states_) {
      out << state;
      out << "State reward: " << state.reward() << "\n\n";
    }
    out << "Macro-action length: " << s.numberOfSteps() << "\n";
    return out << "Macro-action reward: " << s.reward_ << "\n";
  }

  friend struct std::hash<SubsamplingHistoryWrapper>;
};

}  // namespace mcts

namespace std {
template<class S>
struct hash<mcts::SubsamplingHistoryWrapper<S>> {
  size_t operator()(const mcts::SubsamplingHistoryWrapper<S>& s) const noexcept {
    return std::hash<S>{}(s.states_.back());
  }
};
}  // namespace std

#endif  // SMCTS_MCTS_SUBSAMPLING_HISTORY_WRAPPER_H_
