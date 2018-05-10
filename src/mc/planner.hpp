#ifndef SMCTS_MC_PLANNER_H_
#define SMCTS_MC_PLANNER_H_

#include <algorithm>
#include <atomic>
#include <cassert>
#include <cmath>
#include <iostream>
#include <limits>
#include <vector>
#include "../mcts/evaluationpolicy/max_q_evaluation_policy.hpp"
#include "../mcts/evaluationpolicy/mean_q_evaluation_policy.hpp"
#include "../util/random_util.hpp"
#include "state_node.hpp"

namespace mc {

template<class S, template<class> class EvaluationPolicy = mcts::MaxQEvaluationPolicy>
class Planner : public EvaluationPolicy<StateNode<S>> {
 public:
  int horizon() const {
    return horizon_;
  }

  void set_horizon(int horizon) {
    assert(horizon > 0);
    horizon_ = horizon;
  }

  int iterations() const {
    return iterations_;
  }

  void set_iterations(int iterations) {
    assert(iterations > 0);
    iterations_ = iterations;
  }

  int number_of_threads() const {
    return number_of_threads_;
  }

  void set_number_of_threads(int number_of_threads) {
    assert(number_of_threads > 0);
    number_of_threads_ = number_of_threads;
  }

  S bestAction(const S& state) {
    StateNode<S> root(state);
    root.expand();

    std::vector<StateNode<S>> roots(number_of_threads_, root);
    std::uniform_int_distribution<> dist_indices(0, root.child_nodes.size() - 1);
    std::atomic<bool> stop(false);

#pragma omp parallel for schedule(static) num_threads(number_of_threads_)
    for (int k = 0; k < number_of_threads_; k++) {
      StateNode<S>& current_root = roots[k];

      for (int i = 0; i < iterations_ && !stop.load(std::memory_order_relaxed); ++i) {
        StateNode<S>& current_node = current_root.child_nodes[dist_indices(random_util::engine)];
        S current_state = current_node.state;
        float q_value = current_node.reward;

        for (int d = 1; d < horizon_ && !current_state.isFinal(); ++d) {
          current_state.evolve(1, 1.0);
          q_value += current_state.reward();
        }

        current_node.total_q += q_value;
        current_node.visit_count += 1;

        if (q_value > current_node.max_q)
          current_node.max_q = q_value;
      }

      stop.store(true, std::memory_order_seq_cst);
    }

    return EvaluationPolicy<StateNode<S>>::evaluate(roots).state;
  }

 private:
  int horizon_ = 1;
  int iterations_ = 1000;
  int number_of_threads_ = 4;

  template<class S_, template<class> class EvaluationPolicy_>
  friend std::ostream& operator<<(std::ostream& out, const Planner<S_, EvaluationPolicy_>& planner);
};

template<class S, template<class> class EvaluationPolicy>
std::ostream& operator<<(std::ostream& out, const Planner<S, EvaluationPolicy>& planner) {
  out << "Monte Carlo Planner\n";
  out << "Horizon: " << planner.horizon() << "\n";
  out << "Iterations: " << planner.iterations() << "\n";
  out << "Number of Threads: " << planner.number_of_threads() << "\n";
  out << "Evaluation Policy: " << static_cast<const EvaluationPolicy<StateNode<S>>&>(planner);
  return out;
}

}  // namespace mc

#endif  // SMCTS_MC_PLANNER_H_
