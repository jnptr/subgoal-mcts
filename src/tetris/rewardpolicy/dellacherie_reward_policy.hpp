#ifndef SMCTS_TETRIS_REWARD_POLICY_DELLACHERIE_REWARD_POLICY_H_
#define SMCTS_TETRIS_REWARD_POLICY_DELLACHERIE_REWARD_POLICY_H_

#include <boost/container/static_vector.hpp>
#include <iostream>
#include "../tetrimino.hpp"

namespace tetris {

template<class GameBoard, class GameTetrimino>
class DellacherieRewardPolicy {
  static constexpr float kRewardOffset = 500.0;
  static constexpr float kLandingHeightWeight = -1.0;
  static constexpr float kDeletedTetriminoCellsMetricWeight = 1.0;
  static constexpr float kRowTransitionsWeight = -1.0;
  static constexpr float kColumnTranitionsWeight = -1.0;
  static constexpr float kNumberOfHolesWeight = -4.0;
  static constexpr float kWellSumWeight = -1.0;

 private:
  int deleted_tetrimino_cells_metric_ = 0;
  float landing_height_ = 0.0;

 public:
  float reward(const GameBoard& board) const {
    if (!board.currentHeightLessOrEqual(GameBoard::height))
      return -10000.0;

    float reward = 0.0;
    reward += kLandingHeightWeight * landing_height_;
    reward += kDeletedTetriminoCellsMetricWeight * deleted_tetrimino_cells_metric_;
    reward += kRowTransitionsWeight * board.rowTransistions();
    reward += kColumnTranitionsWeight * board.columnTransitions();
    reward += kNumberOfHolesWeight * board.numberOfHoles();
    reward += kWellSumWeight * board.wellSum();
    return kRewardOffset + reward;
  }

  void rewardClearedRows(
      const boost::container::static_vector<int, kMaxTetriminoHeight>& cleared_rows,
      const GameTetrimino& tetrimino) {
    int tetrimino_cells_deleted = 0;

    for (auto& row : cleared_rows) {
      tetrimino_cells_deleted += tetrimino.numberOfCellsInRow(row);
    }

    deleted_tetrimino_cells_metric_ = cleared_rows.size() * tetrimino_cells_deleted;
    landing_height_ = tetrimino.verticalMidpoint();
  }

  std::ostream& printMetrics(const GameBoard& board, std::ostream& out) const {
    out << "Landing height = " << landing_height_ << "\n";
    out << "Deleted tetrimino cells metric = " << deleted_tetrimino_cells_metric_ << "\n";
    out << "Row transitions = " << board.rowTransistions() << "\n";
    out << "Column transitions = " << board.columnTransitions() << "\n";
    out << "Number of holes = " << board.numberOfHoles() << "\n";
    out << "Well sum = " << board.wellSum() << "\n";
    return out;
  }

  friend std::ostream& operator<<(std::ostream& out, const DellacherieRewardPolicy& /*s*/) {
    return out << "Dellacherie (2003)";
  }
};

}  // namespace tetris

#endif  // SMCTS_TETRIS_REWARD_POLICY_DELLACHERIE_REWARD_POLICY_H_
