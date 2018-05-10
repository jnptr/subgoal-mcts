#ifndef SMCTS_TETRIS_REWARD_POLICY_HOLES_HEIGHT_REWARD_POLICY_H_
#define SMCTS_TETRIS_REWARD_POLICY_HOLES_HEIGHT_REWARD_POLICY_H_

#include <boost/container/static_vector.hpp>
#include <iostream>
#include "../tetrimino.hpp"

namespace tetris {

template<class GameBoard, class GameTetrimino>
class HolesHeightRewardPolicy {
  static constexpr float kHeightPenaltyWeight = 0.5;
  static constexpr float kMaxNumberOfHoles = (GameBoard::width / 4.0f) * (GameBoard::height - 2);

  float holesPenalty(const GameBoard& board) const {
    return std::min(1.0f, board.numberOfHoles() / kMaxNumberOfHoles);
  }

  float heightPenalty(const GameBoard& board) const {
    float current_height = board.currentHeight();
    return (current_height * current_height) / (GameBoard::height * GameBoard::height);
  }

 public:
  float reward(const GameBoard& board) const {
    if (!board.currentHeightLessOrEqual(GameBoard::height))
      return -1.0;

    float penalty = kHeightPenaltyWeight * heightPenalty(board) +
                    (1 - kHeightPenaltyWeight) * holesPenalty(board);

    return 1 - penalty;
  }

  void rewardClearedRows(
      const boost::container::static_vector<int, kMaxTetriminoHeight>& /*cleared_rows*/,
      const GameTetrimino& /*tetrimino*/) {
    // Do nothing
  }

  std::ostream& printMetrics(const GameBoard& board, std::ostream& out) const {
    out << "Number of holes = " << board.numberOfHoles() << "\n";
    out << "Current height = " << board.currentHeight() << "\n";
    return out;
  }

  friend std::ostream& operator<<(std::ostream& out, const HolesHeightRewardPolicy& /*s*/) {
    return out << "Holes Penalty + Height Penalty [" << kHeightPenaltyWeight << "]";
  }
};

}  // namespace tetris

#endif  // SMCTS_TETRIS_REWARD_POLICY_HOLES_HEIGHT_REWARD_POLICY_H_
