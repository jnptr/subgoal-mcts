#ifndef SMCTS_TETRIS_POSITION_H_
#define SMCTS_TETRIS_POSITION_H_

#include <algorithm>
#include <boost/functional/hash.hpp>
#include <iostream>
#include <vector>

namespace tetris {

struct Position {
  int x;
  int y;

  bool operator==(const Position& rhs) const noexcept {
    return x == rhs.x && y == rhs.y;
  }

  static const Position kZero;
};

std::ostream& operator<<(std::ostream& out, const Position& p);

}  // namespace tetris

namespace std {
template<>
struct hash<tetris::Position> {
  size_t operator()(const tetris::Position& p) const noexcept {
    std::size_t seed = 0;
    boost::hash_combine(seed, p.x);
    boost::hash_combine(seed, p.y);
    return seed;
  }
};
}  // namespace std

#endif  // SMCTS_TETRIS_POSITION_H_
