#ifndef SMCTS_MCTS_PLANNER_H_
#define SMCTS_MCTS_PLANNER_H_

#include <algorithm>
#include <atomic>
#include <cassert>
#include <cmath>
#include <limits>
#include <unordered_set>
#include "../util/math_util.hpp"
#include "evaluationpolicy/max_q_evaluation_policy.hpp"
#include "evaluationpolicy/mean_q_evaluation_policy.hpp"
#include "expansionpolicy/subsampling_expansion_policy.hpp"
#include "path.hpp"
#include "state_node.hpp"
#include "tree_metadata.hpp"
#include "treepolicy/epsilon_greedy_tree_policy.hpp"
#include "treepolicy/exploration_tree_policy.hpp"
#include "treepolicy/random_tree_policy.hpp"
#include "treepolicy/ucb_tree_policy.hpp"

namespace mcts {

template<class S,
         template<class> class StateNode = StateNode,
         template<class> class ExpansionPolicy = SubsamplingExpansionPolicy,
         template<class> class TreePolicy = UcbTreePolicy,
         template<class> class EvaluationPolicy = MaxQEvaluationPolicy>
class Planner : public ExpansionPolicy<StateNode<S>>,
                public TreePolicy<StateNode<S>>,
                public EvaluationPolicy<StateNode<S>> {
  using NodePath = Path<StateNode<S>>;

 public:
  int iterations() const {
    return iterations_;
  }

  void set_iterations(int iterations) {
    assert(iterations > 0);
    iterations_ = iterations;
  }

  int horizon() const {
    return horizon_;
  }

  void set_horizon(int horizon) {
    assert(horizon > 0);
    horizon_ = horizon;
  }

  int trees_per_thread() const {
    return trees_per_thread_;
  }

  void set_trees_per_thread(int trees_per_thread) {
    assert(trees_per_thread > 0);
    trees_per_thread_ = trees_per_thread;
    number_of_trees_ = number_of_trees();
  }

  int number_of_threads() const {
    return number_of_threads_;
  }

  void set_number_of_threads(int number_of_threads) {
    assert(number_of_threads > 0);
    number_of_threads_ = number_of_threads;
    number_of_trees_ = number_of_trees();
  }

  int number_of_trees() const {
    return number_of_threads_ * trees_per_thread_;
  }

  float gamma() const {
    return gamma_;
  }

  void set_gamma(float gamma) {
    assert(gamma > 0);
    gamma_ = gamma;
    gamma_lookup_table_.updateBase(gamma_);
  }

  bool reuse_trees() const {
    return reuse_trees_;
  }

  void set_reuse_trees(bool reuse_trees) {
    reuse_trees_ = reuse_trees;
  }

  const TreeMetadata& treeMetadata() const {
    return tree_metadata_;
  }

  void resetTreeMetadata() {
    tree_metadata_ = TreeMetadata();
  }

  S bestAction(const S& state) {
    if (reuse_trees_ && trees_.size() == static_cast<size_t>(number_of_trees_))
      reuseTrees(state);
    else
      initializeTrees(state);

    std::vector<TreeMetadata> tree_metadata_vector(static_cast<size_t>(number_of_threads_));
    std::atomic<bool> stop(false);

#pragma omp parallel for schedule(static) num_threads(number_of_threads_)
    for (int k = 0; k < number_of_threads_; k++) {
      TreeMetadata& current_metadata = tree_metadata_vector[k];

      for (int j = 0; j < trees_per_thread_; j++) {
        StateNode<S>& current_root = trees_[k * trees_per_thread_ + j];

        for (int i = 0; i < iterations_ && !stop.load(std::memory_order_relaxed); i++) {
          NodePath path = selectNode(current_root);
          current_metadata.updateFromPath(path);
          float q_value = simulate(path.currentNode().state, path.depth());
          backup(path, q_value);
        }
      }

      stop.store(true, std::memory_order_seq_cst);
    }

    for (const auto& current_metadata : tree_metadata_vector)
      tree_metadata_ += current_metadata;

    return EvaluationPolicy<StateNode<S>>::evaluate(trees_).state;
  }

 private:
  int iterations_ = 1;
  int horizon_ = 1;
  float gamma_ = 1.0;
  int trees_per_thread_ = 1;
  int number_of_threads_ = 4;
  int number_of_trees_ = number_of_trees();
  bool reuse_trees_ = false;
  std::vector<StateNode<S>> trees_;
  math_util::PowLookupTable gamma_lookup_table_;
  TreeMetadata tree_metadata_ = TreeMetadata();

  void initializeTrees(const S& state) {
    trees_.clear();
    trees_.reserve(number_of_trees_);

    for (int i = 0; i < number_of_trees_; i++) {
      trees_.emplace_back(state);
    }
  }

  void reuseTrees(const S& state) {
    for (auto& root : trees_) {
      bool found = false;
      for (auto& child : root.child_nodes) {
        if (state == child.state) {
          root = std::move(child);
          found = true;
          break;
        }
      }

      if (!found)
        root = StateNode<S>{state};
    }
  }

  NodePath selectNode(StateNode<S>& root) {
    StateNode<S>* current_node = &root;
    NodePath path(&root);

    while (!current_node->is_final && path.depth() < horizon_) {
      if (!current_node->fully_expanded()) {
        StateNode<S>* new_state_node =
            ExpansionPolicy<StateNode<S>>::expand(*current_node, horizon_ - path.depth(), gamma_);
        if (new_state_node) {
          path.addNode(new_state_node);
          return path;
        }
      }
      else {
        current_node = TreePolicy<StateNode<S>>::bestChildNode(*current_node);
        path.addNode(current_node);
      }
    }

    assert(path.depth() <= horizon_);
    return path;
  }

  constexpr float rewardAtDepth(float reward, int depth) const {
    return (gamma_ == 1.0 ? reward : gamma_lookup_table_.fastPow(depth) * reward);
  }

  float simulate(S state, int current_depth) {
    float q_value = 0.0;

    while (current_depth < horizon_ && !state.isFinal()) {
      state.evolve(horizon_ - current_depth, gamma_);
      q_value += rewardAtDepth(state.reward(), current_depth + 1);
      current_depth += state.numberOfSteps();
    }

    return q_value;
  }

  void backup(const NodePath& path, float q_value) {
    int current_depth = path.depth();
    // Iterate from leaf to root
    for (auto i = path.nodes().rbegin(); i != path.nodes().rend(); ++i) {
      StateNode<S>& node = **i;
      current_depth -= node.state.numberOfSteps();
      q_value += rewardAtDepth(node.reward, current_depth + 1);
      node.total_q += q_value;
      ++node.visit_count;

      if (q_value > node.max_q)
        node.max_q = q_value;
    }

    assert(current_depth <= 0);
  }

  template<class S_,
           template<class> class StateNode_,
           template<class> class ExpansionPolicy_,
           template<class> class TreePolicy_,
           template<class> class EvaluationPolicy_>
  friend std::ostream& operator<<(
      std::ostream& out,
      const Planner<S_, StateNode_, ExpansionPolicy_, TreePolicy_, EvaluationPolicy_>& planner);
};

template<class S,
         template<class> class StateNode,
         template<class> class ExpansionPolicy,
         template<class> class TreePolicy,
         template<class> class EvaluationPolicy>
std::ostream& operator<<(
    std::ostream& out,
    const Planner<S, StateNode, ExpansionPolicy, TreePolicy, EvaluationPolicy>& planner) {
  out << "MCTS Planner\n";
  out << "Iterations: " << planner.iterations() << "\n";
  out << "Horizon: " << planner.horizon() << "\n";
  out << "Gamma: " << planner.gamma() << "\n";
  out << "Tree Policy: " << static_cast<const TreePolicy<StateNode<S>>&>(planner);
  out << "Evaluation Policy: " << static_cast<const EvaluationPolicy<StateNode<S>>&>(planner);
  out << "Expansion Policy: " << static_cast<const ExpansionPolicy<StateNode<S>>&>(planner);
  out << "Trees per Thread: " << planner.trees_per_thread() << "\n";
  out << "Number of Threads: " << planner.number_of_threads() << "\n";
  out << "Number of Trees: " << planner.number_of_trees() << "\n";
  out << "Reuse Trees: " << (planner.reuse_trees() ? "Yes" : "No") << "\n";
  return out;
}

}  // namespace mcts

#endif  // SMCTS_MCTS_PLANNER_H_
