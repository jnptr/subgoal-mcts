#ifndef SMCTS_TETRIS_BOARD_H_
#define SMCTS_TETRIS_BOARD_H_

#include <array>
#include <cassert>
#include <cmath>
#include <iostream>
#include <numeric>
#include "bitset_2d.hpp"
#include "tetrimino.hpp"

namespace tetris {

template<size_t w, size_t h>
class Board {
  using Bitset = Bitset2D<w, h + kTetriminoMapSize>;

 private:
  Bitset board_bits_;

 public:
  static constexpr size_t width = w;
  static constexpr size_t height = h;

  void deleteRow(size_t row) {
    Bitset previous = board_bits_;
    Bitset mask;
    mask.flip();
    mask <<= row * Bitset::width;
    board_bits_ >>= Bitset::width;
    board_bits_ &= mask;
    mask.flip();
    previous &= mask;
    board_bits_ |= previous;
  }

  int currentHeight() const {
    int i = 0;
    Bitset b = board_bits_;

    while (b.any()) {
      b >>= Bitset::width;
      i++;
    }

    return i;
  }

  bool currentHeightLessOrEqual(size_t input_height) const {
    Bitset b = board_bits_;
    b >>= input_height * Bitset::width;
    return b.none();
  }

  bool rowCompleted(size_t row) const {
    Bitset b = board_bits_;
    b.flip();
    b.isolateRow(row);
    return b.none();
  }

  int rowTransistions() const {
    int transitions = 0;

    for (size_t y = 0; y < height; ++y) {
      bool previous_cell = true;

      for (size_t x = 0; x < width; ++x) {
        bool current_cell = board_bits_.test(x, y);
        if (previous_cell != current_cell)
          transitions++;

        previous_cell = current_cell;
      }

      if (!previous_cell)
        ++transitions;
    }

    return transitions;
  }

  int columnTransitions() const {
    int column_transitions = ((board_bits_ >> Bitset::width) ^ board_bits_).count();
    Bitset bottom_row = board_bits_.isolatedRow(0);
    column_transitions += width - bottom_row.count();
    Bitset top_row = board_bits_.isolatedRow(height - 1);
    column_transitions += top_row.count();
    return column_transitions;
  }

  void placeTetrimino(const Tetrimino<width, height>& tetrimino) {
    board_bits_ |= tetrimino.tetrimino_bits_;
  }

  bool collides(Tetrimino<width, height>& tetrimino) const {
    Bitset intersection = board_bits_ & tetrimino.tetrimino_bits_;
    return intersection.any();
  }

  int numberOfMisalignments() const {
    Bitset b = ((board_bits_ >> width) | board_bits_) ^ board_bits_;
    return b.count();
  }

  int numberOfHoles() const {
    const Bitset b = boardHoles();
    return b.count();
  }

  // Returns the sum of all wells as specified by Fahey (2003)
  // If 'fast' is set to true, uses a modified version by Scherrer et al. (2015)
  template<bool fast = false>
  int wellSum() const {
    const int top_row = currentHeight() - 1;
    int well_sum = 0;

    for (size_t x = 0; x < width; ++x) {
      for (int y = top_row; y >= 0; --y) {
        const int left_x = x - 1;
        const int right_x = x + 1;

        const bool left_cell = (left_x >= 0) ? board_bits_.test(left_x, y) : true;
        const bool right_cell =
            (right_x < static_cast<int>(width)) ? board_bits_.test(right_x, y) : true;

        if (left_cell && right_cell) {
          const int well_depth = wellDepth(x, y);

          if (fast && well_depth > 0) {
            well_sum += well_depth * (well_depth + 1) / 2;
            y -= (well_depth - 1);
          }
          else {
            well_sum += well_depth;
          }
        }
      }
    }

    return well_sum;
  }

  // Returns the sum of all hole depths as specified by Scherrer et al. (2015)
  int holeDepthSum() const {
    Bitset b = ((board_bits_ >> width) | board_bits_) ^ board_bits_;
    static const int kNumberOfVerticalShiftsToCoverBoard = numberOfVerticalShiftsToCoverBoard();
    int shift = 1;

    for (int i = 0; i < kNumberOfVerticalShiftsToCoverBoard; ++i) {
      b |= (b << Bitset::width * shift);
      shift *= 2;
    }

    b &= board_bits_;
    return b.count();
  }

  // Returns the number row that have at least one hole as specified by Scherrer et al. (2015)
  int numberOfRowsWithHoles() const {
    const Bitset holes = boardHoles();
    int number_of_rows_with_holes = 0;

    for (int i = 0; i < static_cast<int>(height); ++i) {
      Bitset current_row = holes.isolatedRow(i);

      if (!current_row.none())
        ++number_of_rows_with_holes;
    }

    return number_of_rows_with_holes;
  }

  // Returns a metric for the diversity of the top of the occupied
  // board as specified by Scherrer et al. (2015)
  int diversityMetric() const {
    // Stores if the following differences have occured: -2, -1, 0, 1 ,2
    std::array<bool, 5> difference_occurences{};

    const auto column_heights = columnHeights();

    for (int i = 0; i < static_cast<int>(column_heights.size()) - 1; ++i) {
      const int difference = column_heights[i + 1] - column_heights[i];
      const int difference_index = difference + (difference_occurences.size() / 2);

      if (difference_index >= 0 &&
          difference_index < static_cast<int>(difference_occurences.size()))
        difference_occurences[difference_index] = true;
    }

    return std::accumulate(difference_occurences.begin(), difference_occurences.end(), 0);
  }

  void clear() {
    board_bits_.reset();
  }

  bool operator==(const Board& b) const noexcept {
    return board_bits_ == b.board_bits_;
  }

 private:
  static int numberOfVerticalShiftsToCoverBoard() {
    return std::floor(std::log2(height)) + 1;
  }

  std::array<int, width> columnHeights() const {
    std::array<int, width> column_heights{};

    for (int x = 0; x < static_cast<int>(width); ++x) {
      for (int y = static_cast<int>(height) - 1; y >= 0; --y) {
        if (board_bits_.test(x, y)) {
          column_heights[x] = y + 1;
          break;
        }
      }
    }

    return column_heights;
  }

  Bitset boardHoles() const {
    Bitset b = board_bits_;
    static const int kNumberOfVerticalShiftsToCoverBoard = numberOfVerticalShiftsToCoverBoard();
    int shift = 1;

    for (int i = 0; i < kNumberOfVerticalShiftsToCoverBoard; ++i) {
      b |= (b >> Bitset::width * shift);
      shift *= 2;
    }

    b ^= board_bits_;
    return b;
  }

  int wellDepth(int x, int y) const {
    int well_depth = 0;

    for (int y_w = y; y_w >= 0; --y_w) {
      if (!board_bits_.test(x, y_w))
        ++well_depth;
      else
        break;
    }

    return well_depth;
  }

  template<size_t width_, size_t height_>
  friend std::ostream& operator<<(std::ostream& out, const Board<width_, height_>& board);

  friend struct std::hash<Board>;
};

template<size_t width, size_t height>
std::ostream& operator<<(std::ostream& out, const Board<width, height>& board) {
  return out << board.board_bits_;
}

}  // namespace tetris

namespace std {
template<size_t width, size_t height>
struct hash<tetris::Board<width, height>> {
  size_t operator()(const tetris::Board<width, height>& b) const noexcept {
    return std::hash<decltype(b.board_bits_)>{}(b.board_bits_);
  }
};
}  // namespace std

#endif  // SMCTS_TETRIS_BOARD_H_
