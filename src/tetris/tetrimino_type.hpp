#ifndef SMCTS_TETRIS_TETRIMINO_TYPE_H_
#define SMCTS_TETRIS_TETRIMINO_TYPE_H_

#include <array>
#include <cassert>
#include <fstream>
#include <iostream>

namespace tetris {

enum class TetriminoType : char { I = 0, L, J, O, S, Z, T, First = I, Last = T };

inline std::ostream& operator<<(std::ostream& out, const TetriminoType& type) {
  switch (type) {
    case TetriminoType::I:
      return out << "I";

    case TetriminoType::L:
      return out << "L";

    case TetriminoType::J:
      return out << "J";

    case TetriminoType::O:
      return out << "O";

    case TetriminoType::S:
      return out << "S";

    case TetriminoType::Z:
      return out << "Z";

    case TetriminoType::T:
      return out << "T";

    default:
      assert(!"Invalid tetrimino type");
      return out;
  }
}

}  // namespace tetris

#endif  // SMCTS_TETRIS_TETRIMINO_TYPE_H_
