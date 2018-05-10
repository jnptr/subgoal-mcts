#include <boost/program_options.hpp>
#include <cstdlib>
#include <iostream>
#include "mc/planner.hpp"
#include "util/csv_util.hpp"
#include "util/main_util.hpp"

template<class State>
struct McPlannerContext {
  using PlanningState = State;
  using Planner = mc::Planner<State>;
  Planner planner;
  int iterations = 1000;
  int number_of_threads = 4;

  PlanningState initialState(const State& state) {
    return state;
  }

  void processState(PlanningState* /*state*/) {}

  std::ostream& printMetadata(std::ostream& out) const {
    return out;
  }

  const State& domainState(const PlanningState& planning_state) {
    return planning_state;
  }

  void addProgramOptions(boost::program_options::options_description* desc) {
    namespace po = boost::program_options;
    // clang-format off
    desc->add_options()
        ("iterations,i", po::value<int>(&iterations)->default_value(iterations), "mc iterations")
        ("threads,t", po::value<int>(&number_of_threads)->default_value(number_of_threads),
            "number of threads");
    // clang-format on
  }

  void configurePlanner(const boost::program_options::variables_map& /*vm*/) {
    planner.set_iterations(iterations);
    planner.set_number_of_threads(number_of_threads);
  }

  void logConfiguration(csv::Data* log_data) {
    log_data->values.push_back({"iterations", planner.iterations()});
    log_data->values.push_back({"number_of_threads", planner.number_of_threads()});
  }
};

int main(int argc, char** argv) {
  return main_util::main<McPlannerContext>(argc, argv);
}
