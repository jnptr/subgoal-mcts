#ifndef SMCTS_TETRIS_STATE_TRADITIONAL_MACRO_STATE_H_
#define SMCTS_TETRIS_STATE_TRADITIONAL_MACRO_STATE_H_

#include <iostream>
#include "../../util/random_util.hpp"
#include "../rotation_model.hpp"
#include "../tetrimino_list.hpp"
#include "../tetrimino_rotation.hpp"

namespace tetris {

template<class GameState>
class TraditionalMacroState {
 private:
  using GameTetrimino = typename GameState::GameTetrimino;
  using RModel = RotationModel<GameState::width, GameState::height>;
  static constexpr const RModel& rotation_model = RModel::kTetris;

 public:
  TraditionalMacroState() : game_(TetriminoList::instance().begin()) {}

  void evolve(int /*max_steps*/, float /*gamma*/) {
    auto tetrimino_rotation = TetriminoRotation(game_.currentTetrimino());
    tetrimino_rotation.rotateRandomly();
    const GameTetrimino& rotated_tetrimino = rotation_model.tetrimino(tetrimino_rotation);
    int min_shift = -rotated_tetrimino.maxLeftMovement();
    int max_shift = rotated_tetrimino.maxRightMovement();
    std::uniform_int_distribution<> dist_shift(min_shift, max_shift);
    int shift = dist_shift(random_util::engine);
    dropAndScore(rotated_tetrimino, shift);
  }

  void successorStates(std::vector<TraditionalMacroState>* v) const {
    auto tetrimino_rotation = TetriminoRotation(game_.currentTetrimino());
    int rotation_count = rotation_model.numberOfDistinctRotations(tetrimino_rotation);

    for (int i = 0; i < rotation_count; ++i) {
      const GameTetrimino& rotated_tetrimino = rotation_model.tetrimino(tetrimino_rotation);
      int min_shift = -rotated_tetrimino.maxLeftMovement();
      int max_shift = rotated_tetrimino.maxRightMovement();
      for (int shift = min_shift; shift <= max_shift; ++shift) {
        v->emplace_back(*this);
        v->back().dropAndScore(rotated_tetrimino, shift);
      }
      tetrimino_rotation.rotateCW();
    }
  }

  bool isFinal() const {
    return game_.gameOver();
  }

  bool isSubgoal() const {
    return true;
  }

  float reward() const {
    return game_.reward();
  }

  int numberOfSteps() const {
    return 1;
  }

  const GameState& game() const {
    return game_;
  }

  bool operator==(const TraditionalMacroState& b) const {
    return game_.boardsEqual(b.game_);
  }

  friend std::ostream& operator<<(std::ostream& out, const TraditionalMacroState& s) {
    return out << s.game_;
  }

 private:
  GameState game_;

  void dropAndScore(GameTetrimino tetrimino, int shift) {
    tetrimino.moveLeftRight(shift);

    while (true) {
      if (!tetrimino.canMoveDown())
        break;

      tetrimino.moveDown();

      if (game_.board().collides(tetrimino)) {
        tetrimino.moveUp();
        break;
      }
    }

    game_.placeAndScore(tetrimino);
  }

  friend struct std::hash<TraditionalMacroState>;
};

}  // namespace tetris

namespace std {
template<class GameState>
struct hash<tetris::TraditionalMacroState<GameState>> {
  size_t operator()(const tetris::TraditionalMacroState<GameState>& s) const noexcept {
    return std::hash<decltype(s.game_)>{}(s.game_);
  }
};
}  // namespace std

#endif  // SMCTS_TETRIS_STATE_TRADITIONAL_MACRO_STATE_H_
