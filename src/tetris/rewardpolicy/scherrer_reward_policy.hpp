#ifndef SMCTS_TETRIS_REWARD_POLICY_SCHERRER_REWARD_POLICY_H_
#define SMCTS_TETRIS_REWARD_POLICY_SCHERRER_REWARD_POLICY_H_

#include <boost/container/static_vector.hpp>
#include <iostream>
#include "../tetrimino.hpp"

namespace tetris {

template<class GameBoard, class GameTetrimino>
class ScherrerRewardPolicy {
  struct WeightsDt10 {
    static constexpr float kLandingHeight = -2.18;
    static constexpr float kDeletedTetriminoCellsMetric = 2.42;
    static constexpr float kRowTransitions = -2.17;
    static constexpr float kColumnTranitions = -3.31;
    static constexpr float kNumberOfHoles = 0.95;
    static constexpr float kWellSum = -2.22;
    static constexpr float kHoleDepthSum = -0.81;
    static constexpr float kRowsWithHoles = -9.65;
    static constexpr float kDiversityMetric = 1.27;
    static constexpr bool kUseEfficientWellSum = true;

    static constexpr const char* description = "DT-10";
  };

  struct WeightsDt20 {
    static constexpr float kLandingHeight = -2.68;
    static constexpr float kDeletedTetriminoCellsMetric = 1.38;
    static constexpr float kRowTransitions = -2.41;
    static constexpr float kColumnTranitions = -6.32;
    static constexpr float kNumberOfHoles = 2.03;
    static constexpr float kWellSum = -2.71;
    static constexpr float kHoleDepthSum = -0.43;
    static constexpr float kRowsWithHoles = -9.48;
    static constexpr float kDiversityMetric = 0.89;
    static constexpr bool kUseEfficientWellSum = true;

    static constexpr const char* description = "DT-20";
  };

  struct WeightsDu {
    static constexpr float kLandingHeight = -12.629;
    static constexpr float kDeletedTetriminoCellsMetric = 6.601974;
    static constexpr float kRowTransitions = -9.215815;
    static constexpr float kColumnTranitions = -19.77356;
    static constexpr float kNumberOfHoles = -13.08335;
    static constexpr float kWellSum = -10.48747;
    static constexpr float kHoleDepthSum = -1.611863;
    static constexpr float kRowsWithHoles = -24.04087;
    static constexpr float kDiversityMetric = 0.0;
    static constexpr bool kUseEfficientWellSum = false;

    static constexpr const char* description = "DU";
  };

 private:
  using Weights = WeightsDt10;
  int deleted_tetrimino_cells_metric_ = 0;
  float landing_height_ = 0.0;

 public:
  float reward(const GameBoard& board) const {
    if (!board.currentHeightLessOrEqual(GameBoard::height))
      return -10000.0;

    float reward = 0.0;
    reward += Weights::kLandingHeight * landing_height_;
    reward += Weights::kDeletedTetriminoCellsMetric * deleted_tetrimino_cells_metric_;
    reward += Weights::kRowTransitions * board.rowTransistions();
    reward += Weights::kColumnTranitions * board.columnTransitions();
    reward += Weights::kNumberOfHoles * board.numberOfHoles();
    reward += Weights::kWellSum * board.template wellSum<Weights::kUseEfficientWellSum>();
    reward += Weights::kHoleDepthSum * board.holeDepthSum();
    reward += Weights::kRowsWithHoles * board.numberOfRowsWithHoles();
    reward += Weights::kDiversityMetric * board.diversityMetric();
    return reward;
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
    out << "Well sum = " << board.template wellSum<Weights::kUseEfficientWellSum>() << "\n";
    out << "Hole depth sum = " << board.holeDepthSum() << "\n";
    out << "Number of rows with holes = " << board.numberOfRowsWithHoles() << "\n";
    out << "Diversity metric = " << board.diversityMetric() << "\n";
    return out;
  }

  friend std::ostream& operator<<(std::ostream& out, const ScherrerRewardPolicy& /*s*/) {
    return out << "Scherrer et al. (2015) [" << Weights::description << "]";
  }
};

}  // namespace tetris

#endif  // SMCTS_TETRIS_REWARD_POLICY_SCHERRER_REWARD_POLICY_H_
