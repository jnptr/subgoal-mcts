#ifndef SMCTS_GRIDWORLD_POSITION_H_
#define SMCTS_GRIDWORLD_POSITION_H_

#include <algorithm>
#include <array>
#include <boost/functional/hash.hpp>
#include <iostream>
#include <vector>

namespace gridworld {

struct Position {
  int x;
  int y;

  Position north() const {
    return {x, y + 1};
  }

  Position south() const {
    return {x, y - 1};
  }

  Position east() const {
    return {x + 1, y};
  }

  Position west() const {
    return {x - 1, y};
  }

  bool operator==(const Position& rhs) const noexcept {
    return x == rhs.x && y == rhs.y;
  }

  friend struct std::hash<Position>;
};

inline std::ostream& operator<<(std::ostream& out, const Position& p) {
  return out << "(" << p.x << "," << p.y << ")";
}

}  // namespace gridworld

namespace std {
template<>
struct hash<gridworld::Position> {
  size_t operator()(const gridworld::Position& p) const noexcept {
    std::size_t seed = 0;
    boost::hash_combine(seed, p.x);
    boost::hash_combine(seed, p.y);
    return seed;
  }
};
}  // namespace std

#endif  // SMCTS_GRIDWORLD_POSITION_H_
