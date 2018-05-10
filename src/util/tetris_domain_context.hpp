#ifndef SMCTS_UTIL_TETRIS_DOMAIN_CONTEXT_H_
#define SMCTS_UTIL_TETRIS_DOMAIN_CONTEXT_H_

#include <boost/program_options.hpp>
#include <iostream>
#include "../tetris/game.hpp"
#include "../tetris/rewardpolicy/dellacherie_reward_policy.hpp"
#include "../tetris/rewardpolicy/exp_holes_reward_policy.hpp"
#include "../tetris/rewardpolicy/height_reward_policy.hpp"
#include "../tetris/rewardpolicy/holes_height_reward_policy.hpp"
#include "../tetris/rewardpolicy/misalignment_height_reward_policy.hpp"
#include "../tetris/rewardpolicy/scherrer_reward_policy.hpp"
#include "../tetris/state/primitive_state.hpp"
#include "../tetris/state/subsampling_state.hpp"
#include "../tetris/state/traditional_macro_state.hpp"
#include "../tetris/state/traditional_state.hpp"
#include "csv_util.hpp"

namespace main_util {

template<template<class> class StateWrapper = tetris::PrimitiveState>
struct TetrisDomainContext {
  using State = StateWrapper<tetris::Game<10, 10, tetris::HolesHeightRewardPolicy>>;
  std::string tetrimino_file;

  void addProgramOptions(boost::program_options::options_description* desc) {
    namespace po = boost::program_options;
    // clang-format off
    desc->add_options()
        ("tetrimino-file,f", po::value<std::string>(&tetrimino_file), "tetrimino file");
    // clang-format on
  }

  State initialState(const boost::program_options::variables_map& vm) {
    if (vm.count("tetrimino-file")) {
      tetris::TetriminoList::instance().readFromFile(tetrimino_file);
      std::cout << "Tetrimino List: " << tetrimino_file << " ("
                << tetris::TetriminoList::instance().count() << " pieces)\n\n";
    }
    else {
      tetris::TetriminoList::instance().createRandomList(1000000);
      std::cout << "Tetrimino List: Random (" << tetris::TetriminoList::instance().count()
                << " pieces)\n\n";
    }

    return State();
  }

  void logResult(const State& state, csv::Data* log_data) {
    log_data->values.push_back({"cleared_rows", state.game().clearedRows()});
    log_data->values.push_back({"game_over", state.isFinal()});
  }
};

}  // namespace main_util

#endif  // SMCTS_UTIL_TETRIS_DOMAIN_CONTEXT_H_
