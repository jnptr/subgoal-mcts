#ifndef SMCTS_TETRIS_REWARD_POLICY_EXP_HOLES_REWARD_POLICY_H_
#define SMCTS_TETRIS_REWARD_POLICY_EXP_HOLES_REWARD_POLICY_H_

#include <boost/container/static_vector.hpp>
#include <cmath>
#include <iostream>
#include "../tetrimino.hpp"

namespace tetris {

template<class GameBoard, class GameTetrimino>
class ExpHolesRewardPolicy {
  static constexpr float kHolesFraction = 33.0 * (static_cast<float>(GameBoard::height) / 20.0);

 public:
  float reward(const GameBoard& board) const {
    if (!board.currentHeightLessOrEqual(GameBoard::height))
      return -1.0;

    return std::exp(-board.numberOfHoles() / kHolesFraction);
  }

  void rewardClearedRows(
      const boost::container::static_vector<int, kMaxTetriminoHeight>& /*cleared_rows*/,
      const GameTetrimino& /*tetrimino*/) {
    // Do nothing
  }

  std::ostream& printMetrics(const GameBoard& board, std::ostream& out) const {
    out << "Number of holes = " << board.numberOfHoles() << "\n";
    return out;
  }

  friend std::ostream& operator<<(std::ostream& out, const ExpHolesRewardPolicy& /*s*/) {
    return out << "Exp. Holes (Faußer and Schwenker, 2013)";
  }
};

}  // namespace tetris

#endif  // SMCTS_TETRIS_REWARD_POLICY_EXP_HOLES_REWARD_POLICY_H_
