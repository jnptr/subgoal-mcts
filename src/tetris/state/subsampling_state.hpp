#ifndef SMCTS_TETRIS_STATE_SUBSAMPLING_STATE_H_
#define SMCTS_TETRIS_STATE_SUBSAMPLING_STATE_H_

#include <iostream>
#include "../../util/random_util.hpp"
#include "../rotation_model.hpp"
#include "../tetrimino_list.hpp"
#include "../tetrimino_rotation.hpp"

namespace tetris {

template<class GameState>
class SubsamplingState {
 private:
  using GameTetrimino = typename GameState::GameTetrimino;
  using RModel = RotationModel<GameState::width, GameState::height>;
  static constexpr const RModel& rotation_model = RModel::kTetris;
  enum class Action : char { NO_OP = 0, LEFT, RIGHT, First = NO_OP, Last = RIGHT };

  GameState game_;

 public:
  SubsamplingState() : game_(TetriminoList::instance().begin()) {}

  void evolve(int /*max_steps*/, float /*gamma*/) {
    auto tetrimino_rotation = TetriminoRotation(game_.currentTetrimino());
    tetrimino_rotation.rotateRandomly();
    GameTetrimino moved_tetrimino = rotation_model.tetrimino(tetrimino_rotation);

    std::uniform_int_distribution<> dist_actions(static_cast<int>(Action::First),
                                                 static_cast<int>(Action::Last));
    moved_tetrimino.chooseStartingPoint();

    while (true) {
      auto action = static_cast<Action>(dist_actions(random_util::engine));

      if (action == Action::LEFT && moved_tetrimino.canMoveLeft()) {
        moved_tetrimino.moveLeft();

        if (game_.board().collides(moved_tetrimino))
          moved_tetrimino.moveRight();
      }
      else if (action == Action::RIGHT && moved_tetrimino.canMoveRight()) {
        moved_tetrimino.moveRight();

        if (game_.board().collides(moved_tetrimino))
          moved_tetrimino.moveLeft();
      }

      if (!moved_tetrimino.canMoveDown())
        break;

      moved_tetrimino.moveDown();

      if (game_.board().collides(moved_tetrimino)) {
        moved_tetrimino.moveUp();
        break;
      }
    }

    game_.placeAndScore(moved_tetrimino);
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

  bool operator==(const SubsamplingState& b) const {
    return game_.boardsEqual(b.game_);
  }

  friend std::ostream& operator<<(std::ostream& out, const SubsamplingState& s) {
    return out << s.game_;
  }

  friend struct std::hash<SubsamplingState>;
};

}  // namespace tetris

namespace std {
template<class GameState>
struct hash<tetris::SubsamplingState<GameState>> {
  size_t operator()(const tetris::SubsamplingState<GameState>& s) const noexcept {
    return std::hash<decltype(s.game_)>{}(s.game_);
  }
};
}  // namespace std

#endif  // SMCTS_TETRIS_STATE_SUBSAMPLING_STATE_H_
