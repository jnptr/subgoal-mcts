#include "position.hpp"

namespace tetris {

const Position Position::kZero = {0, 0};

std::ostream& operator<<(std::ostream& out, const Position& p) {
  return out << "(" << p.x << "," << p.y << ")";
}

}  // namespace tetris
