#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <iostream>
#include "../tetris/game.hpp"
#include "../tetris/rewardpolicy/holes_height_reward_policy.hpp"
#include "../tetris/state/traditional_state.hpp"

struct TetrisFixture {
  TetrisFixture() {
    tetris::TetriminoList::instance().createStaticList();
  }
};

BOOST_FIXTURE_TEST_SUITE(tetris, TetrisFixture)

BOOST_AUTO_TEST_SUITE(traditional_state)

BOOST_AUTO_TEST_CASE(evolve) {
  using State = TraditionalState<tetris::Game<10, 10, tetris::HolesHeightRewardPolicy>>;
  State state;

  for (int i = 0; i < 5; i++) {
    std::vector<State> v;
    state.successorStates(&v);
    state.evolve(1, 1.0);
    BOOST_CHECK(std::find(v.begin(), v.end(), state) != v.end());
  }
}

BOOST_AUTO_TEST_SUITE_END()  // traditional_state

BOOST_AUTO_TEST_SUITE_END()  // tetris
