#ifndef SMCTS_UTIL_MAIN_UTIL_H_
#define SMCTS_UTIL_MAIN_UTIL_H_

#include <boost/optional.hpp>
#include <boost/program_options.hpp>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include "csv_util.hpp"
#include "gridworld_domain_context.hpp"
#include "tetris_domain_context.hpp"

namespace main_util {

template<template<class> class PlannerContext_, class DomainContext_ = TetrisDomainContext<>>
int main(int argc, char** argv) {
  using DomainContext = DomainContext_;
  using PlannerContext = PlannerContext_<typename DomainContext::State>;
  namespace po = boost::program_options;

  PlannerContext planner_context;
  DomainContext domain_context;
  int horizon = 1;
  int number_of_steps = 1000;
  std::string csv_file;

  // Declare the supported options.
  po::options_description desc("Allowed options");
  // clang-format off
  desc.add_options()
      ("help", "produce help message")
      ("horizon,h", po::value<int>(&horizon)->default_value(horizon), "planning horizon")
      ("steps,s", po::value<int>(&number_of_steps)->default_value(number_of_steps),
          "number of time steps")
      ("verbose,v", "show each step")
      ("log-file,l", po::value<std::string>(&csv_file), "csv log file");
  // clang-format on

  planner_context.addProgramOptions(&desc);
  domain_context.addProgramOptions(&desc);
  po::variables_map vm;

  try {
    po::store(po::parse_command_line(argc, argv, desc), vm);

    if (vm.count("help")) {
      std::cout << desc << "\n";
      return 1;
    }

    po::notify(vm);
  }
  catch (boost::program_options::error& e) {
    std::cerr << "ERROR: " << e.what() << "\n\n";
    return 1;
  }

  bool verbose = vm.count("verbose");

  planner_context.planner.set_horizon(horizon);
  planner_context.configurePlanner(vm);
  std::cout << planner_context.planner << "\n";

  auto state = planner_context.initialState(domain_context.initialState(vm));
  boost::optional<int> game_over_step;
  double reward_sum = state.reward();
  int current_step = 0;
  int number_of_subgoals = 0;

  if (planner_context.domainState(state).isSubgoal())
    number_of_subgoals = 1;

  auto t1 = std::chrono::high_resolution_clock::now();

  while (current_step < number_of_steps) {
    state = planner_context.planner.bestAction(state);
    planner_context.processState(&state);
    reward_sum += state.reward();
    current_step += state.numberOfSteps();

    if (planner_context.domainState(state).isSubgoal())
      ++number_of_subgoals;

    if (verbose) {
      std::cout << "Step: " << current_step << "\n\n";
      std::cout << state;
      std::cout << "State reward: " << state.reward() << "\n";
      std::cout << "Reward sum: " << reward_sum << "\n\n";
      planner_context.printMetadata(std::cout) << "\n";
    }

    if (state.isFinal()) {
      game_over_step = current_step;
      break;
    }
  }

  auto t2 = std::chrono::high_resolution_clock::now();

  if (!verbose) {
    std::cout << state << "\n";
    std::cout << "Reward sum: " << reward_sum << "\n\n";
    planner_context.printMetadata(std::cout) << "\n";
  }

  if (game_over_step)
    std::cout << "*** Final state at step " << *game_over_step << " *** \n\n";
  else
    std::cout << "Steps: " << number_of_steps << "\n";

  std::chrono::duration<double> duration = t2 - t1;
  std::cout << "Time: " << duration.count() << std::endl;

  if (vm.count("log-file")) {
    csv::Data log_data;
    log_data.values.push_back({"number_of_steps", number_of_steps});
    log_data.values.push_back({"steps", current_step});
    log_data.values.push_back({"elapsed_time", duration.count()});
    log_data.values.push_back({"final_state", game_over_step != boost::none});
    log_data.values.push_back({"reward_sum", reward_sum});
    log_data.values.push_back({"horizon", horizon});
    log_data.values.push_back({"final_reward", planner_context.domainState(state).reward()});
    log_data.values.push_back({"number_of_subgoals", number_of_subgoals});

    planner_context.logConfiguration(&log_data);
    domain_context.logResult(planner_context.domainState(state), &log_data);
    log_data.write(csv_file);
  }

  return 0;
}

}  // namespace main_util

#endif  // SMCTS_UTIL_MAIN_UTIL_H_
