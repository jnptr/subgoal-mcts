#ifndef SMCTS_UTIL_GRIDWORLD_DOMAIN_CONTEXT_H_
#define SMCTS_UTIL_GRIDWORLD_DOMAIN_CONTEXT_H_

#include <boost/program_options.hpp>
#include <iostream>
#include "../gridworld/examples.hpp"
#include "../gridworld/state.hpp"
#include "csv_util.hpp"

namespace main_util {

struct GridworldDomainContext {
  using State = GRIDWORLD_STATE(gridworld::examples::eightRoomsBai);
  std::string tetrimino_file;

  void addProgramOptions(boost::program_options::options_description* /*desc*/) {}

  State initialState(const boost::program_options::variables_map& /*vm*/) {
    return {};
  }

  void logResult(const State& /*state*/, csv::Data* /*log_data*/) {}
};

}  // namespace main_util

#endif  // SMCTS_UTIL_GRIDWORLD_DOMAIN_CONTEXT_H_
