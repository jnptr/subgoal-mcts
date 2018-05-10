#include <boost/program_options.hpp>
#include <cstdlib>
#include <iostream>
#include "mcts/expansionpolicy/subsampling_expansion_policy.hpp"
#include "mcts/planner.hpp"
#include "mcts/subsampling_history_wrapper.hpp"
#include "mcts/subsampling_short_history_wrapper.hpp"
#include "mcts/subsampling_state_wrapper.hpp"
#include "util/csv_util.hpp"
#include "util/main_util.hpp"
#include "util/tetris_domain_context.hpp"

template<class State>
struct SmctsPlannerContext {
  using PlanningState = mcts::SubsamplingShortHistoryWrapper<State>;
  using Planner = mcts::Planner<PlanningState, mcts::StateNode, mcts::SubsamplingExpansionPolicy>;
  Planner planner;
  int iterations = 10000;
  float c = 1.0;
  float gamma = 1.0;
  int trees_per_thread = 1;
  int number_of_threads = 4;
  float action_coverage = 0.6;
  float tolerated_error = 0.001;
  bool hierarchical_control_mode = false;

  PlanningState initialState(const State& state) {
    return PlanningState(state);
  }

  void processState(PlanningState* state) {
    if (!hierarchical_control_mode)
      state->truncate();
  }

  std::ostream& printMetadata(std::ostream& out) const {
    return out << planner.treeMetadata();
  }

  const State& domainState(const PlanningState& planning_state) {
    return planning_state.state();
  }

  void addProgramOptions(boost::program_options::options_description* desc) {
    namespace po = boost::program_options;
    // clang-format off
    desc->add_options()
      ("iterations,i", po::value<int>(&iterations)->default_value(iterations), "mcts iterations")
      ("gamma,g", po::value<float>(&gamma)->default_value(gamma), "discount factor")
      ("exploration,c", po::value<float>(&c)->default_value(c), "uct exploration constant")
      ("trees,n", po::value<int>(&trees_per_thread)->default_value(trees_per_thread),
          "trees per thread")
      ("threads,t", po::value<int>(&number_of_threads)->default_value(number_of_threads),
          "number of threads")
      ("reuse,r", "reuse decision trees")
      ("action-coverage,p", po::value<float>(&action_coverage)->default_value(action_coverage),
          "action coverage")
      ("tolerated-error,e", po::value<float>(&tolerated_error)->default_value(tolerated_error),
          "tolerated error")
      ("hierarchical,z", "use hierarchical control mode");
    // clang-format on
  }

  void configurePlanner(const boost::program_options::variables_map& vm) {
    planner.set_iterations(iterations);
    planner.set_c(c);
    planner.set_gamma(gamma);
    planner.set_trees_per_thread(trees_per_thread);
    planner.set_number_of_threads(number_of_threads);
    planner.set_reuse_trees(vm.count("reuse"));
    planner.set_action_coverage(action_coverage);
    planner.set_tolerated_error(tolerated_error);
    hierarchical_control_mode = vm.count("hierarchical");
  }

  void logConfiguration(csv::Data* log_data) {
    log_data->values.push_back({"iterations", planner.iterations()});
    log_data->values.push_back({"c", planner.c()});
    log_data->values.push_back({"gamma", planner.gamma()});
    log_data->values.push_back({"trees_per_thread", planner.trees_per_thread()});
    log_data->values.push_back({"number_of_threads", planner.number_of_threads()});
    log_data->values.push_back({"reuse_trees", planner.reuse_trees()});
    log_data->values.push_back({"action_coverage", planner.action_coverage()});
    log_data->values.push_back({"tolerated_error", planner.tolerated_error()});
    log_data->values.push_back({"tree_max_depth", planner.treeMetadata().maxDepth()});
    log_data->values.push_back({"tree_max_node_depth", planner.treeMetadata().maxNodeDepth()});
    log_data->values.push_back({"tree_avg_depth", planner.treeMetadata().averageDepth()});
    log_data->values.push_back({"tree_avg_node_depth", planner.treeMetadata().averageNodeDepth()});
    log_data->values.push_back({"iteration_sum", planner.treeMetadata().iterationCount()});
  }
};

int main(int argc, char** argv) {
  return main_util::main<SmctsPlannerContext>(argc, argv);
}
