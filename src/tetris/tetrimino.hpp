#ifndef SMCTS_TETRIS_TETRIMINO_H_
#define SMCTS_TETRIS_TETRIMINO_H_

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <limits>
#include "../util/random_util.hpp"
#include "bitset_2d.hpp"
#include "position.hpp"
#include "tetrimino_type.hpp"

namespace tetris {

static constexpr size_t kMaxTetriminoHeight = 4;
static constexpr size_t kTetriminoMapSize = 5;

template<size_t w, size_t h>
class Tetrimino {
  using Bitset = Bitset2D<w, h + kTetriminoMapSize>;

 private:
  Bitset tetrimino_bits_;
  int leftmost_column_;
  int rightmost_column_;
  int lowest_row_;
  int tetrimino_height_;

 public:
  static constexpr size_t width = w;
  static constexpr size_t height = h;

  Tetrimino(std::initializer_list<Position> init_list) {
    int min_x = std::numeric_limits<int>::max();
    int max_x = std::numeric_limits<int>::lowest();
    int min_y = std::numeric_limits<int>::max();
    int max_y = std::numeric_limits<int>::lowest();

    for (auto& position : init_list) {
      min_x = std::min(min_x, position.x);
      max_x = std::max(max_x, position.x);
      min_y = std::min(min_y, position.y);
      max_y = std::max(max_y, position.y);
    }

    int mid_x = min_x + (max_x - min_x) / 2;

    for (auto& position : init_list) {
      int y = Bitset::height - 1 - max_y + position.y;
      int x = (Bitset::width / 2) - 1 - mid_x + position.x;
      tetrimino_bits_.set(x, y);
    }

    leftmost_column_ = (Bitset::width / 2) - 1 - mid_x + min_x;
    rightmost_column_ = (Bitset::width / 2) - 1 - mid_x + max_x;
    lowest_row_ = Bitset::height - 1 - max_y + min_y;
    tetrimino_height_ = max_y - min_y;
  }

  explicit Tetrimino(std::string tetrimino_string) {
    static constexpr char kTetriminoChar = 'X';

    assert(tetrimino_string.length() == kTetriminoMapSize * kTetriminoMapSize);
    assert(Bitset::height >= kTetriminoMapSize);
    assert(Bitset::width >= kTetriminoMapSize);

    int x_offset = (Bitset::width - kTetriminoMapSize) / 2;

    leftmost_column_ = std::numeric_limits<int>::max();
    rightmost_column_ = std::numeric_limits<int>::lowest();
    lowest_row_ = std::numeric_limits<int>::max();
    int topmost_row = std::numeric_limits<int>::lowest();

    for (int y = 0; y < static_cast<int>(kTetriminoMapSize); ++y) {
      for (int x = 0; x < static_cast<int>(kTetriminoMapSize); ++x) {
        char current_char = tetrimino_string.at(y * kTetriminoMapSize + x);
        int x_bitset = x_offset + x;
        int y_bitset = (Bitset::height - 1) - y;

        if (current_char == kTetriminoChar) {
          tetrimino_bits_.set(x_bitset, y_bitset);
          leftmost_column_ = std::min(leftmost_column_, x_bitset);
          rightmost_column_ = std::max(rightmost_column_, x_bitset);
          lowest_row_ = std::min(lowest_row_, y_bitset);
          topmost_row = std::max(topmost_row, y_bitset);
        }
      }
    }

    tetrimino_height_ = topmost_row - lowest_row_;
  }

  float verticalMidpoint() const {
    return lowest_row_ + 0.5 * tetrimino_height_;
  }

  int numberOfCellsInRow(size_t row) const {
    return tetrimino_bits_.isolatedRow(row).count();
  }

  int maxLeftMovement() const {
    return leftmost_column_;
  }

  bool canMoveLeft(int n = 1) const {
    return n <= maxLeftMovement();
  }

  void moveLeft(int n = 1) {
    tetrimino_bits_ <<= n;
    leftmost_column_ -= n;
    rightmost_column_ -= n;
  }

  int maxRightMovement() const {
    return static_cast<int>(Bitset::width) - 1 - rightmost_column_;
  }

  bool canMoveRight(int n = 1) const {
    return n <= maxRightMovement();
  }

  void moveRight(int n = 1) {
    tetrimino_bits_ >>= n;
    leftmost_column_ += n;
    rightmost_column_ += n;
  }

  bool canMoveLeftRight(int n) const {
    if (n > 0)
      return canMoveRight(n);
    else if (n < 0)
      return canMoveLeft(-n);
    else
      return true;
  }

  void moveLeftRight(int n) {
    if (n > 0)
      moveRight(n);
    else
      moveLeft(-n);
  }

  bool canMove(const Position& position) const {
    return canMoveLeftRight(position.x) && canMoveDown(position.y);
  }

  Tetrimino movedTetrimino(const Position& position) const {
    Tetrimino moved_tetrimino = *this;
    moved_tetrimino.leftmost_column_ += position.x;
    moved_tetrimino.rightmost_column_ += position.x;
    moved_tetrimino.lowest_row_ -= position.y;

    int shift = position.y * Bitset::width + position.x;

    if (shift > 0)
      moved_tetrimino.tetrimino_bits_ >>= shift;
    else
      moved_tetrimino.tetrimino_bits_ <<= -shift;

    return moved_tetrimino;
  }

  bool canMoveDown(int n = 1) const {
    return lowest_row_ >= n;
  }

  void moveDown() {
    tetrimino_bits_ >>= Bitset::width;
    lowest_row_ -= 1;
  }

  void moveUp() {
    tetrimino_bits_ <<= Bitset::width;
    lowest_row_ += 1;
  }

  int tetriminoHeight() const {
    return tetrimino_height_;
  }

  int lowestRow() const {
    return lowest_row_;
  }

  void chooseStartingPoint() {
    int range = width - (rightmost_column_ - leftmost_column_) - 1;
    std::uniform_int_distribution<> dist_x(0, range);

    int shift = dist_x(random_util::engine);

    if (shift <= leftmost_column_) {
      shift = leftmost_column_ - shift;
      tetrimino_bits_ <<= shift;
      leftmost_column_ -= shift;
      rightmost_column_ -= shift;
    }
    else {
      shift = shift - leftmost_column_;
      tetrimino_bits_ >>= shift;
      leftmost_column_ += shift;
      rightmost_column_ += shift;
    }
  }

  template<size_t width_, size_t height_>
  friend std::ostream& operator<<(std::ostream& out, const Tetrimino<width_, height_>& tetrimino);

  template<size_t width_, size_t height_>
  friend class Board;
};

template<size_t width, size_t height>
std::ostream& operator<<(std::ostream& out, const Tetrimino<width, height>& tetrimino) {
  return out << tetrimino.tetrimino_bits_;
}

}  // namespace tetris

#endif  // SMCTS_TETRIS_TETRIMINO_H_
