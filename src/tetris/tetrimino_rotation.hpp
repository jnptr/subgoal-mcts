#ifndef SMCTS_TETRIS_TETRIMINO_ROTATION_H_
#define SMCTS_TETRIS_TETRIMINO_ROTATION_H_

#include <cassert>
#include <iostream>
#include "../util/random_util.hpp"
#include "tetrimino_type.hpp"

namespace tetris {

class TetriminoRotation {
 private:
  static constexpr int kRotationCount = 4;
  TetriminoType type_;
  char rotation_;

 public:
  explicit TetriminoRotation(TetriminoType type) : type_(type), rotation_(0) {}

  void rotateCW(int steps = 1) {
    assert(steps < kRotationCount && steps > -kRotationCount);
    rotation_ = (rotation_ + steps + kRotationCount) % kRotationCount;
  }

  void rotateRandomly() {
    std::uniform_int_distribution<> dist_rotations(0, kRotationCount - 1);
    rotation_ = dist_rotations(random_util::engine);
  }

  friend std::ostream& operator<<(std::ostream& out, const TetriminoRotation& r);

  template<size_t w, size_t h>
  friend class RotationModel;
};

inline std::ostream& operator<<(std::ostream& out, const TetriminoRotation& r) {
  return out << "(" << r.type_ << "," << +r.rotation_ << ")";
}

}  // namespace tetris

#endif  // SMCTS_TETRIS_TETRIMINO_ROTATION_H_
