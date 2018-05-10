#ifndef SMCTS_MCTS_SUBSAMPLING_SHORT_HISTORY_WRAPPER_H_
#define SMCTS_MCTS_SUBSAMPLING_SHORT_HISTORY_WRAPPER_H_

#include <algorithm>
#include <boost/container/static_vector.hpp>
#include <iostream>

namespace mcts {

template<class State>
class SubsamplingShortHistoryWrapper {
 private:
  boost::container::static_vector<State, 2> states_;
  int number_of_steps_;
  float reward_;

 public:
  SubsamplingShortHistoryWrapper() : SubsamplingShortHistoryWrapper(State()) {}

  explicit SubsamplingShortHistoryWrapper(State state)
      : number_of_steps_(1), reward_(state.reward()) {
    states_.push_back(state);
  }

  void evolve(int max_steps, float gamma) {
    reward_ = 0.0;
    number_of_steps_ = 0;
    State state = states_.back();
    states_.clear();
    float current_discount = 1.0;

    do {
      state.evolve(1, 1.0);

      // Only save first state in history
      if (number_of_steps_ == 0)
        states_.push_back(state);

      ++number_of_steps_;
      reward_ += current_discount * state.reward();
      current_discount *= gamma;
    } while (!state.isSubgoal() && !state.isFinal() && number_of_steps_ < max_steps);

    // Save subgoal state
    states_.push_back(state);
  }

  void truncate() {
    reward_ = states_.front().reward();
    number_of_steps_ = 1;
    states_.erase(states_.begin() + 1, states_.end());
  }

  bool isFinal() const {
    return states_.back().isFinal();
  }

  float reward() const {
    return reward_;
  }

  int numberOfSteps() const {
    return number_of_steps_;
  }

  const State& state() const {
    return states_.back();
  }

  bool operator==(const SubsamplingShortHistoryWrapper& rhs) const {
    return states_.back() == rhs.states_.back();
  }

  friend std::ostream& operator<<(std::ostream& out, const SubsamplingShortHistoryWrapper& s) {
    out << s.state() << "\n";
    out << "Subgoal reward: " << s.state().reward() << "\n";
    out << "Macro-action length: " << s.numberOfSteps() << "\n";
    return out << "Macro-action reward: " << s.reward_ << "\n";
  }

  friend struct std::hash<SubsamplingShortHistoryWrapper>;
};

}  // namespace mcts

namespace std {
template<class S>
struct hash<mcts::SubsamplingShortHistoryWrapper<S>> {
  size_t operator()(const mcts::SubsamplingShortHistoryWrapper<S>& s) const noexcept {
    return std::hash<S>{}(s.states_.back());
  }
};
}  // namespace std

#endif  // SMCTS_MCTS_SUBSAMPLING_SHORT_HISTORY_WRAPPER_H_
