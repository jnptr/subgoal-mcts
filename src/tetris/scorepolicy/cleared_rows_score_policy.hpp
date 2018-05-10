#ifndef SMCTS_TETRIS_SCORE_POLICY_CLEARED_ROWS_SCORE_POLICY_H_
#define SMCTS_TETRIS_SCORE_POLICY_CLEARED_ROWS_SCORE_POLICY_H_

#include <iostream>

namespace tetris {

class ClearedRowsScorePolicy {
 public:
  void scoreClearedRows(int cleared_rows) {
    cleared_rows_ += cleared_rows;
  }

  int clearedRows() const {
    return cleared_rows_;
  }

  bool operator==(const ClearedRowsScorePolicy& b) const noexcept {
    return cleared_rows_ == b.cleared_rows_;
  }

 protected:
  ~ClearedRowsScorePolicy() = default;

 private:
  int cleared_rows_ = 0;
};

inline std::ostream& operator<<(std::ostream& out, const ClearedRowsScorePolicy& policy) {
  return out << "cleared rows = " << policy.clearedRows();
}

}  // namespace tetris

#endif  // SMCTS_TETRIS_SCORE_POLICY_CLEARED_ROWS_SCORE_POLICY_H_
