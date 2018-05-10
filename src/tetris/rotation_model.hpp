#ifndef SMCTS_TETRIS_ROTATION_MODEL_H_
#define SMCTS_TETRIS_ROTATION_MODEL_H_

#include <boost/container/static_vector.hpp>
#include <boost/functional/hash.hpp>
#include <cassert>
#include <iostream>
#include "tetrimino.hpp"
#include "tetrimino_rotation.hpp"

namespace tetris {

template<size_t w, size_t h>
class RotationModel {
  using T = Tetrimino<w, h>;
  using RotationVector = boost::container::static_vector<T, TetriminoRotation::kRotationCount>;

 private:
  RotationVector o_rotations_ = {};
  RotationVector i_rotations_ = {};
  RotationVector s_rotations_ = {};
  RotationVector z_rotations_ = {};
  RotationVector t_rotations_ = {};
  RotationVector l_rotations_ = {};
  RotationVector j_rotations_ = {};

  const RotationVector& rotations(TetriminoType type) const {
    switch (type) {
      case TetriminoType::I:
        return i_rotations_;

      case TetriminoType::L:
        return l_rotations_;

      case TetriminoType::J:
        return j_rotations_;

      case TetriminoType::O:
        return o_rotations_;

      case TetriminoType::S:
        return s_rotations_;

      case TetriminoType::Z:
        return z_rotations_;

      case TetriminoType::T:
        return t_rotations_;
    }

    return o_rotations_;
  }

 public:
  const T& tetrimino(const TetriminoRotation& r) const {
    const RotationVector& rotation_vector = rotations(r.type_);
    return rotation_vector[r.rotation_ % rotation_vector.size()];
  }

  int numberOfDistinctRotations(const TetriminoRotation& r) const {
    return rotations(r.type_).size();
  }

  bool rotationsEqual(const TetriminoRotation& a, const TetriminoRotation& b) const {
    if (a.type_ != b.type_)
      return false;

    int rotation_count = numberOfDistinctRotations(a);
    return (a.rotation_ % rotation_count) == (b.rotation_ % rotation_count);
  }

  size_t rotationHash(const TetriminoRotation& r) const {
    std::size_t seed = 0;
    boost::hash_combine(seed, r.type_);
    boost::hash_combine(seed, r.rotation_ % numberOfDistinctRotations(r));
    return seed;
  }

  static RotationModel createLegacy() {
    RotationModel rotation_model;
    rotation_model.o_rotations_ = {{{{0, 0}, {0, 1}, {0, 2}, {1, 2}}}};
    rotation_model.i_rotations_ = {
        {{{0, 0}, {0, 1}, {0, 2}, {0, 3}}, {{0, 0}, {1, 0}, {2, 0}, {3, 0}}}};
    rotation_model.s_rotations_ = {
        {{{0, 0}, {1, 0}, {1, 1}, {2, 1}}, {{0, 0}, {0, 1}, {-1, 1}, {-1, 2}}}};
    rotation_model.z_rotations_ = {
        {{{0, 0}, {1, 0}, {0, 1}, {-1, 1}}, {{0, 0}, {0, 1}, {1, 1}, {1, 2}}}};
    rotation_model.t_rotations_ = {{{{0, 0}, {-1, 1}, {0, 1}, {1, 1}},
                                    {{0, 0}, {0, 1}, {0, 2}, {-1, 1}},
                                    {{0, 0}, {1, 0}, {2, 0}, {1, 1}},
                                    {{0, 0}, {0, 1}, {0, 2}, {1, 1}}}};
    rotation_model.l_rotations_ = {{{{0, 0}, {1, 0}, {0, 1}, {0, 2}},
                                    {{0, 0}, {0, 1}, {1, 1}, {2, 1}},
                                    {{0, 0}, {0, 1}, {0, 2}, {-1, 2}},
                                    {{0, 0}, {1, 0}, {2, 0}, {2, 1}}}};
    rotation_model.j_rotations_ = {{{{0, 0}, {0, 1}, {0, 2}, {1, 2}},
                                    {{0, 0}, {0, 1}, {-1, 1}, {-2, 1}},
                                    {{0, 0}, {1, 0}, {1, 1}, {1, 2}},
                                    {{0, 0}, {1, 0}, {2, 0}, {0, 1}}}};
    return rotation_model;
  };

  static RotationModel createInstantaneous() {
    RotationModel rotation_model;
    rotation_model.o_rotations_ = {T{"....."
                                     "..XX."
                                     "..XX."
                                     "....."
                                     "....."}};

    rotation_model.i_rotations_ = {T{"....."
                                     "....."
                                     ".XXXX"
                                     "....."
                                     "....."},
                                   T{"....."
                                     "..X.."
                                     "..X.."
                                     "..X.."
                                     "..X.."},
                                   T{"....."
                                     "....."
                                     "XXXX."
                                     "....."
                                     "....."},
                                   T{"..X.."
                                     "..X.."
                                     "..X.."
                                     "..X.."
                                     "....."}};

    rotation_model.s_rotations_ = {T{"....."
                                     "....."
                                     "..XX."
                                     ".XX.."
                                     "....."},
                                   T{"....."
                                     ".X..."
                                     ".XX.."
                                     "..X.."
                                     "....."},
                                   T{"....."
                                     "..XX."
                                     ".XX.."
                                     "....."
                                     "....."},
                                   T{"....."
                                     "..X.."
                                     "..XX."
                                     "...X."
                                     "....."}};

    rotation_model.z_rotations_ = {T{"....."
                                     "....."
                                     ".XX.."
                                     "..XX."
                                     "....."},
                                   T{"....."
                                     "..X.."
                                     ".XX.."
                                     ".X..."
                                     "....."},
                                   T{"....."
                                     ".XX.."
                                     "..XX."
                                     "....."
                                     "....."},
                                   T{"....."
                                     "...X."
                                     "..XX."
                                     "..X.."
                                     "....."}};

    rotation_model.t_rotations_ = {T{"....."
                                     "..X.."
                                     ".XXX."
                                     "....."
                                     "....."},
                                   T{"....."
                                     "..X.."
                                     "..XX."
                                     "..X.."
                                     "....."},
                                   T{"....."
                                     "....."
                                     ".XXX."
                                     "..X.."
                                     "....."},
                                   T{"....."
                                     "..X.."
                                     ".XX.."
                                     "..X.."
                                     "....."}};

    rotation_model.l_rotations_ = {T{"....."
                                     "....."
                                     ".XXX."
                                     ".X..."
                                     "....."},
                                   T{"....."
                                     ".XX.."
                                     "..X.."
                                     "..X.."
                                     "....."},
                                   T{"....."
                                     "...X."
                                     ".XXX."
                                     "....."
                                     "....."},
                                   T{"....."
                                     "..X.."
                                     "..X.."
                                     "..XX."
                                     "....."}};

    rotation_model.j_rotations_ = {T{"....."
                                     "....."
                                     ".XXX."
                                     "...X."
                                     "....."},
                                   T{"....."
                                     "..X.."
                                     "..X.."
                                     ".XX.."
                                     "....."},
                                   T{"....."
                                     ".X..."
                                     ".XXX."
                                     "....."
                                     "....."},
                                   T{"....."
                                     "..XX."
                                     "..X.."
                                     "..X.."
                                     "....."}};
    return rotation_model;
  };

  static RotationModel createTetris() {
    RotationModel rotation_model;
    rotation_model.o_rotations_ = {T{"....."
                                     "..XX."
                                     "..XX."
                                     "....."
                                     "....."}};

    rotation_model.i_rotations_ = {T{"....."
                                     "....."
                                     ".XXXX"
                                     "....."
                                     "....."},
                                   T{"....."
                                     "..X.."
                                     "..X.."
                                     "..X.."
                                     "..X.."}};

    rotation_model.s_rotations_ = {T{"....."
                                     "...XX"
                                     "..XX."
                                     "....."
                                     "....."},
                                   T{"....."
                                     "..X.."
                                     "..XX."
                                     "...X."
                                     "....."}};

    rotation_model.z_rotations_ = {T{"....."
                                     "..XX."
                                     "...XX"
                                     "....."
                                     "....."},
                                   T{"....."
                                     "...X."
                                     "..XX."
                                     "..X.."
                                     "....."}};

    rotation_model.t_rotations_ = {T{"....."
                                     "...X."
                                     "..XXX"
                                     "....."
                                     "....."},
                                   T{"....."
                                     "..X.."
                                     "..XX."
                                     "..X.."
                                     "....."},
                                   T{"....."
                                     "..XXX"
                                     "...X."
                                     "....."
                                     "....."},
                                   T{"....."
                                     "...X."
                                     "..XX."
                                     "...X."
                                     "....."}};

    rotation_model.l_rotations_ = {T{"....."
                                     "..XXX"
                                     "..X.."
                                     "....."
                                     "....."},
                                   T{"....."
                                     "..XX."
                                     "...X."
                                     "...X."
                                     "....."},
                                   T{"....."
                                     "....X"
                                     "..XXX"
                                     "....."
                                     "....."},
                                   T{"....."
                                     "..X.."
                                     "..X.."
                                     "..XX."
                                     "....."}};

    rotation_model.j_rotations_ = {T{"....."
                                     "..XXX"
                                     "....X"
                                     "....."
                                     "....."},
                                   T{"....."
                                     "...X."
                                     "...X."
                                     "..XX."
                                     "....."},
                                   T{"....."
                                     "..X.."
                                     "..XXX"
                                     "....."
                                     "....."},
                                   T{"....."
                                     "..XX."
                                     "..X.."
                                     "..X.."
                                     "....."}};
    return rotation_model;
  };

  static const RotationModel kLegacy;

  // The "instantaneous rotation model" as defined by Demaine, Hohenberger, Liben-Nowell (2002)
  static const RotationModel kInstantaneous;

  // The "Tetris rotation model" as defined by Demaine, Hohenberger, Liben-Nowell (2002)
  static const RotationModel kTetris;
};

template<size_t w, size_t h>
const RotationModel<w, h> RotationModel<w, h>::kLegacy = RotationModel<w, h>::createLegacy();

template<size_t w, size_t h>
const RotationModel<w, h> RotationModel<w, h>::kInstantaneous =
    RotationModel<w, h>::createInstantaneous();

template<size_t w, size_t h>
const RotationModel<w, h> RotationModel<w, h>::kTetris = RotationModel<w, h>::createTetris();

}  // namespace tetris

#endif  // SMCTS_TETRIS_ROTATION_MODEL_H_
