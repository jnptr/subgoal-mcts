#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include "../gridworld/examples.hpp"
#include "../gridworld/position.hpp"
#include "../gridworld/state.hpp"
#include "../gridworld/world.hpp"

BOOST_AUTO_TEST_SUITE(gridworld)

struct FixturePosition {
  gridworld::Position p = {0, 0};
};

BOOST_FIXTURE_TEST_SUITE(position, FixturePosition)

BOOST_AUTO_TEST_CASE(north) {
  BOOST_CHECK(p.north().x == 0);
  BOOST_CHECK(p.north().y == 1);
}

BOOST_AUTO_TEST_CASE(south) {
  BOOST_CHECK(p.south().x == 0);
  BOOST_CHECK(p.south().y == -1);
}

BOOST_AUTO_TEST_CASE(east) {
  BOOST_CHECK(p.east().x == 1);
  BOOST_CHECK(p.east().y == 0);
}

BOOST_AUTO_TEST_CASE(west) {
  BOOST_CHECK(p.west().x == -1);
  BOOST_CHECK(p.west().y == 0);
}

BOOST_AUTO_TEST_SUITE_END()  // Position

struct FixtureWorld {
  static const std::string kWorldString;
  static constexpr float kDefaultReward = -0.5;
  static constexpr float kGoalReward = 1.0;

  gridworld::World<5, 5> world;
  float default_reward = kDefaultReward;
  float goal_reward = kGoalReward;

  FixtureWorld() : world(kWorldString, kDefaultReward, {kGoalReward}) {}
};

const std::string FixtureWorld::kWorldString = "...X."
                                               "....."
                                               "..###"
                                               "....G"
                                               ".....";

BOOST_FIXTURE_TEST_SUITE(world, FixtureWorld)

BOOST_AUTO_TEST_CASE(start_position) {
  gridworld::Position start_position = {3, 0};
  BOOST_CHECK_EQUAL(world.startPosition(), start_position);
}

BOOST_AUTO_TEST_CASE(reward_at_position) {
  BOOST_CHECK_EQUAL(world.rewardAtPosition({4, 4}), default_reward);
  BOOST_CHECK_EQUAL(world.rewardAtPosition({0, 4}), default_reward);
  BOOST_CHECK_EQUAL(world.rewardAtPosition({0, 0}), default_reward);
  BOOST_CHECK_EQUAL(world.rewardAtPosition({4, 3}), goal_reward);
}

BOOST_AUTO_TEST_CASE(is_legal_position) {
  BOOST_CHECK_EQUAL(world.isLegalPosition({1, 1}), true);
  BOOST_CHECK_EQUAL(world.isLegalPosition({4, 3}), true);
  BOOST_CHECK_EQUAL(world.isLegalPosition({2, 2}), false);
  BOOST_CHECK_EQUAL(world.isLegalPosition({4, 2}), false);
  BOOST_CHECK_EQUAL(world.isLegalPosition({-1, 0}), false);
  BOOST_CHECK_EQUAL(world.isLegalPosition({0, 7}), false);
  BOOST_CHECK_EQUAL(world.isLegalPosition({5, 5}), false);
}

BOOST_AUTO_TEST_CASE(is_final_position) {
  BOOST_CHECK_EQUAL(world.isFinalPosition({4, 3}), true);
  BOOST_CHECK_EQUAL(world.isFinalPosition({1, 2}), false);
}

BOOST_AUTO_TEST_SUITE_END()  // world

struct FixtureState {
  static const gridworld::World<5, 5> kWorld;
  gridworld::World<5, 5> world = kWorld;
  GRIDWORLD_STATE(kWorld) state;
};

const gridworld::World<5, 5> FixtureState::kWorld(FixtureWorld::kWorldString,
                                                  FixtureWorld::kDefaultReward,
                                                  {FixtureWorld::kGoalReward});

BOOST_FIXTURE_TEST_SUITE(state, FixtureState)

BOOST_AUTO_TEST_CASE(start_position) {
  BOOST_CHECK_EQUAL(state.position(), world.startPosition());
}

BOOST_AUTO_TEST_CASE(is_final) {
  BOOST_CHECK_EQUAL(state.isFinal(), false);
}

BOOST_AUTO_TEST_CASE(successor_states) {
  std::vector<decltype(state)> successor_states;
  state.successorStates(&successor_states);
  BOOST_CHECK_EQUAL(successor_states.size(), 3);
}

BOOST_AUTO_TEST_SUITE_END()  // state

BOOST_AUTO_TEST_SUITE_END()  // gridworld
