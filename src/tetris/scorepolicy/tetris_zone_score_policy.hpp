#ifndef SMCTS_TETRIS_SCORE_POLICY_TETRIS_ZONE_SCORE_POLICY_H_
#define SMCTS_TETRIS_SCORE_POLICY_TETRIS_ZONE_SCORE_POLICY_H_

#include <iostream>

namespace tetris {

class TetrisZoneScorePolicy {
 public:
  void scoreClearedRows(int cleared_rows) {
    if (cleared_rows < 4) {
      score_ += cleared_rows * 100;
      tetris_ = false;
    }
    else if (!tetris_) {
      score_ += 800;
      tetris_ = true;
    }
    else {
      score_ += 1200;
    }
  }

  int score() const {
    return score_;
  }

  bool tetris() const {
    return tetris_;
  }

  bool operator==(const TetrisZoneScorePolicy& b) const noexcept {
    return (score_ == b.score_) && (tetris_ == b.tetris_);
  }

 protected:
  ~TetrisZoneScorePolicy() = default;

 private:
  int score_ = 0;
  bool tetris_ = false;
};

inline std::ostream& operator<<(std::ostream& out, const TetrisZoneScorePolicy& policy) {
  return out << "score = " << policy.score() << ", tetris = " << policy.tetris();
}

}  // namespace tetris

#endif  // SMCTS_TETRIS_SCORE_POLICY_TETRIS_ZONE_SCORE_POLICY_H_
