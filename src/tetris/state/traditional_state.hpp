#ifndef SMCTS_TETRIS_STATE_TRADITIONAL_STATE_H_
#define SMCTS_TETRIS_STATE_TRADITIONAL_STATE_H_

#include <boost/functional/hash.hpp>
#include <boost/optional.hpp>
#include <boost/optional/optional_io.hpp>
#include <iostream>
#include "../../util/random_util.hpp"
#include "../rotation_model.hpp"
#include "../tetrimino.hpp"
#include "../tetrimino_list.hpp"
#include "../tetrimino_rotation.hpp"

namespace tetris {

template<class GameState>
class TraditionalState {
 private:
  using GameTetrimino = typename GameState::GameTetrimino;
  using RModel = RotationModel<GameState::width, GameState::height>;
  static constexpr const RModel& rotation_model = RModel::kTetris;

 public:
  TraditionalState() : game_(TetriminoList::instance().begin()) {}

  void evolve(int /*max_steps*/, float /*gamma*/) {
    if (current_tetrimino_ == boost::none) {
      current_tetrimino_ = TetriminoRotation(game_.currentTetrimino());
      (*current_tetrimino_).rotateRandomly();
      tetrimino_position_ = {0, 0};
    }

    GameTetrimino moved_tetrimino =
        rotation_model.tetrimino(*current_tetrimino_).movedTetrimino(tetrimino_position_);

    std::array<int, 3> actions = {-1, 0, 1};
    std::shuffle(actions.begin(), actions.end(), random_util::engine);

    for (auto& action : actions) {
      if (moveTetrimino(&moved_tetrimino, action))
        break;
    }
  }

  void successorStates(std::vector<TraditionalState>* v) const {
    if (current_tetrimino_ == boost::none) {
      auto tetrimino_rotation = TetriminoRotation(game_.currentTetrimino());
      int rotation_count = rotation_model.numberOfDistinctRotations(tetrimino_rotation);

      for (int i = 0; i < rotation_count; ++i) {
        TraditionalState state_copy = *this;
        state_copy.current_tetrimino_ = tetrimino_rotation;
        state_copy.tetrimino_position_ = {0, 0};
        state_copy.validMoves(rotation_model.tetrimino(tetrimino_rotation), v);
        tetrimino_rotation.rotateCW();
      }
    }
    else {
      validMoves(rotation_model.tetrimino(*current_tetrimino_), v);
    }
  }

  bool isFinal() const {
    return game_.gameOver();
  }

  bool isSubgoal() const {
    return current_tetrimino_ == boost::none;
  }

  float reward() const {
    if (current_tetrimino_ == boost::none)
      return game_.reward();
    else
      return 0.0;
  }

  int numberOfSteps() const {
    return 1;
  }

  const GameState& game() const {
    return game_;
  }

  bool operator==(const TraditionalState& b) const {
    if (current_tetrimino_ && b.current_tetrimino_) {
      return rotation_model.rotationsEqual(*current_tetrimino_, *b.current_tetrimino_) &&
             tetrimino_position_ == b.tetrimino_position_ && game_ == b.game_;
    }
    else {
      return static_cast<bool>(current_tetrimino_) == static_cast<bool>(b.current_tetrimino_) &&
             game_ == b.game_;
    }
  }

 private:
  GameState game_;
  Position tetrimino_position_ = {0, 0};
  boost::optional<TetriminoRotation> current_tetrimino_ = boost::none;

  void completeMove(const GameTetrimino& tetrimino) {
    current_tetrimino_ = boost::none;
    game_.placeAndScore(tetrimino);
  }

  bool moveTetrimino(GameTetrimino* tetrimino, int movement_x) {
    if (tetrimino->canMoveLeftRight(movement_x)) {
      tetrimino->moveLeftRight(movement_x);

      if (game_.board().collides(*tetrimino)) {
        tetrimino->moveLeftRight(-movement_x);
        return false;
      }

      tetrimino_position_.x += movement_x;

      if (tetrimino->canMoveDown()) {
        tetrimino->moveDown();

        if (game_.board().collides(*tetrimino)) {
          tetrimino->moveUp();
          completeMove(*tetrimino);
        }
        else {
          ++tetrimino_position_.y;

          // Revert tetrimino movement so that it can be reused
          tetrimino->moveUp();
        }
      }
      else {
        completeMove(*tetrimino);
      }

      // Revert tetrimino movement so that it can be reused
      tetrimino->moveLeftRight(-movement_x);

      return true;
    }

    return false;
  }

  void validMoves(const GameTetrimino& tetrimino, std::vector<TraditionalState>* v) const {
    GameTetrimino moved_tetrimino = tetrimino.movedTetrimino(tetrimino_position_);

    for (int i = -1; i <= 1; ++i) {
      TraditionalState state_copy = *this;
      if (state_copy.moveTetrimino(&moved_tetrimino, i))
        v->push_back(state_copy);
    }
  }

  friend std::ostream& operator<<(std::ostream& out, const TraditionalState& s) {
    out << "width = " << GameState::width << ", height = " << GameState::height << "\n";
    out << static_cast<const typename GameState::ScorePolicy&>(s.game_);
    out << ", next tetrimino: " << s.game_.nextTetrimino() << "\n";
    out << "current tetrimino: " << s.current_tetrimino_ << ", position: " << s.tetrimino_position_
        << "\n";

    if (s.current_tetrimino_ == boost::none) {
      out << s.game_.board() << "\n";
    }
    else {
      auto moved_tetrimino =
          rotation_model.tetrimino(*s.current_tetrimino_).movedTetrimino(s.tetrimino_position_);
      auto board = s.game_.board();
      board.placeTetrimino(moved_tetrimino);
      out << board << "\n";
    }

    s.game_.printRewardInfo(out);
    return out;
  }

  friend struct std::hash<TraditionalState>;
};

}  // namespace tetris

namespace std {
template<class GameState>
struct hash<tetris::TraditionalState<GameState>> {
  size_t operator()(const tetris::TraditionalState<GameState>& s) const noexcept {
    std::size_t seed = 0;
    boost::hash_combine(seed, std::hash<decltype(s.game_)>{}(s.game_));
    if (s.current_tetrimino_) {
      boost::hash_combine(seed, s.tetrimino_position_.x);
      boost::hash_combine(seed, s.tetrimino_position_.y);
      boost::hash_combine(
          seed,
          tetris::TraditionalState<GameState>::rotation_model.rotationHash(*s.current_tetrimino_));
    }
    else {
      boost::hash_combine(seed, false);
    }
    return seed;
  }
};
}  // namespace std

#endif  // SMCTS_TETRIS_STATE_TRADITIONAL_STATE_H_
