#include <boost/program_options.hpp>
#include <cstdlib>
#include <iostream>
#include "dls/planner.hpp"
#include "util/csv_util.hpp"
#include "util/main_util.hpp"

template<class State>
struct DlsPlannerContext {
  using PlanningState = State;
  using Planner = dls::Planner<State>;
  Planner planner;

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

  void addProgramOptions(boost::program_options::options_description* /*desc*/) {}
  void configurePlanner(const boost::program_options::variables_map& /*vm*/) {}
  void logConfiguration(csv::Data* /*log_data*/) {}
};

int main(int argc, char** argv) {
  return main_util::main<DlsPlannerContext>(argc, argv);
}
