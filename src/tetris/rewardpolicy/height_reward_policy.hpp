#ifndef SMCTS_TETRIS_REWARD_POLICY_HEIGHT_REWARD_POLICY_H_
#define SMCTS_TETRIS_REWARD_POLICY_HEIGHT_REWARD_POLICY_H_

#include <boost/container/static_vector.hpp>
#include <iostream>
#include "../tetrimino.hpp"

namespace tetris {

template<class GameBoard, class GameTetrimino>
class HeightRewardPolicy {
  static constexpr bool kSquareHeight = false;

  float heightPenalty(const GameBoard& board) const {
    float current_height = board.currentHeight();
    if (kSquareHeight)
      return (current_height * current_height) / (GameBoard::height * GameBoard::height);
    else
      return current_height / GameBoard::height;
  }

 public:
  float reward(const GameBoard& board) const {
    if (!board.currentHeightLessOrEqual(GameBoard::height))
      return -1.0;

    return 1 - heightPenalty(board);
  }

  void rewardClearedRows(
      const boost::container::static_vector<int, kMaxTetriminoHeight>& /*cleared_rows*/,
      const GameTetrimino& /*tetrimino*/) {
    // Do nothing
  }

  std::ostream& printMetrics(const GameBoard& board, std::ostream& out) const {
    out << "Current height = " << board.currentHeight() << "\n";
    return out;
  }

  friend std::ostream& operator<<(std::ostream& out, const HeightRewardPolicy& /*s*/) {
    return out << "Height Penalty [square = " << (kSquareHeight ? "true" : "false") << "]";
  }
};

}  // namespace tetris

#endif  // SMCTS_TETRIS_REWARD_POLICY_HEIGHT_REWARD_POLICY_H_
