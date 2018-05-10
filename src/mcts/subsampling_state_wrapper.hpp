#ifndef SMCTS_MCTS_SUBSAMPLING_STATE_WRAPPER_H_
#define SMCTS_MCTS_SUBSAMPLING_STATE_WRAPPER_H_

#include <algorithm>
#include <iostream>

namespace mcts {

template<class State>
class SubsamplingStateWrapper {
 private:
  State state_;
  int number_of_steps_;
  float reward_;

 public:
  SubsamplingStateWrapper() : SubsamplingStateWrapper(State()) {}

  explicit SubsamplingStateWrapper(State state)
      : state_(state), number_of_steps_(1), reward_(state.reward()) {}

  void evolve(int max_steps, float gamma) {
    number_of_steps_ = 0;
    reward_ = 0.0;
    float current_discount = 1.0;

    do {
      state_.evolve(1, 1.0);
      ++number_of_steps_;
      reward_ += current_discount * state_.reward();
      current_discount *= gamma;
    } while (!state_.isSubgoal() && !state_.isFinal() && number_of_steps_ < max_steps);
  }

  void truncate() {}

  bool isFinal() const {
    return state_.isFinal();
  }

  float reward() const {
    return reward_;
  }

  int numberOfSteps() const {
    return number_of_steps_;
  }

  const State& state() const {
    return state_;
  }

  bool operator==(const SubsamplingStateWrapper& rhs) const {
    return state_ == rhs.state_;
  }

  friend std::ostream& operator<<(std::ostream& out, const SubsamplingStateWrapper& s) {
    out << s.state_ << "\n";
    out << "Subgoal reward: " << s.state_.reward() << "\n";
    out << "Macro-action length: " << s.number_of_steps_ << "\n";
    return out << "Macro-action reward: " << s.reward_ << "\n";
  }

  friend struct std::hash<SubsamplingStateWrapper>;
};

}  // namespace mcts

namespace std {
template<class S>
struct hash<mcts::SubsamplingStateWrapper<S>> {
  size_t operator()(const mcts::SubsamplingStateWrapper<S>& s) const noexcept {
    return std::hash<S>{}(s.state_);
  }
};
}  // namespace std

#endif  // SMCTS_MCTS_SUBSAMPLING_STATE_WRAPPER_H_
