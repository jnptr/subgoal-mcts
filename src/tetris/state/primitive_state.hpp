#ifndef SMCTS_TETRIS_STATE_PRIMITVE_STATE_H_
#define SMCTS_TETRIS_STATE_PRIMITVE_STATE_H_

#include <array>
#include <boost/functional/hash.hpp>
#include <iostream>
#include "../../util/random_util.hpp"
#include "../rotation_model.hpp"
#include "../tetrimino.hpp"
#include "../tetrimino_list.hpp"
#include "../tetrimino_rotation.hpp"

namespace tetris {

enum class PrimitiveAction : char {
  NO_OP = 0,
  LEFT,
  RIGHT,
  ROTATE_CW,
  ROTATE_CCW,
  First = NO_OP,
  Last = ROTATE_CCW
};
static constexpr std::array<PrimitiveAction, 5> kPrimitiveActions = {PrimitiveAction::NO_OP,
                                                                     PrimitiveAction::LEFT,
                                                                     PrimitiveAction::RIGHT,
                                                                     PrimitiveAction::ROTATE_CW,
                                                                     PrimitiveAction::ROTATE_CCW};

template<class GameState>
class PrimitiveState {
 private:
  using GameTetrimino = typename GameState::GameTetrimino;
  using RModel = RotationModel<GameState::width, GameState::height>;
  static constexpr const RModel& rotation_model = RModel::kTetris;

 public:
  PrimitiveState()
      : game_(TetriminoList::instance().begin()),
        tetrimino_position_(Position::kZero),
        current_tetrimino_(TetriminoRotation(game_.currentTetrimino())) {}

  void evolve(int /*max_steps*/, float /*gamma*/) {
    std::uniform_int_distribution<> dist_actions(static_cast<int>(PrimitiveAction::First),
                                                 static_cast<int>(PrimitiveAction::Last));

    while (true) {
      auto action = static_cast<PrimitiveAction>(dist_actions(random_util::engine));
      if (executeAction(action))
        return;
    }
  }

  void successorStates(std::vector<PrimitiveState>* v) const {
    v->reserve(kPrimitiveActions.size());

    for (const auto& action : kPrimitiveActions) {
      PrimitiveState state_copy = *this;
      if (state_copy.executeAction(action))
        v->push_back(state_copy);
    }
  }

  bool isFinal() const {
    return game_.gameOver();
  }

  bool isSubgoal() const {
    return tetrimino_position_ == Position::kZero;
  }

  float reward() const {
    if (tetrimino_position_ == Position::kZero)
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

  bool operator==(const PrimitiveState& b) const {
    return rotation_model.rotationsEqual(current_tetrimino_, b.current_tetrimino_) &&
           tetrimino_position_ == b.tetrimino_position_ && game_ == b.game_;
  }

 private:
  GameState game_;
  Position tetrimino_position_;
  TetriminoRotation current_tetrimino_;

  bool executeAction(PrimitiveAction a) {
    switch (a) {
      case PrimitiveAction::NO_OP:
        return moveTetrimino(0);
      case PrimitiveAction::LEFT:
        return moveTetrimino(-1);
      case PrimitiveAction::RIGHT:
        return moveTetrimino(1);
      case PrimitiveAction::ROTATE_CW:
        return rotateTetrimino(1);
      case PrimitiveAction::ROTATE_CCW:
        return rotateTetrimino(-1);
    }

    return false;
  }

  bool rotateTetrimino(int rotation) {
    TetriminoRotation updated_tetrimino = current_tetrimino_;
    updated_tetrimino.rotateCW(rotation);
    const GameTetrimino& tetrimino = rotation_model.tetrimino(updated_tetrimino);

    if (!tetrimino.canMove(tetrimino_position_))
      return false;

    GameTetrimino moved_tetrimino = tetrimino.movedTetrimino(tetrimino_position_);

    if (game_.board().collides(moved_tetrimino))
      return false;

    current_tetrimino_ = updated_tetrimino;
    completeMove(moved_tetrimino);
    return true;
  }

  bool moveTetrimino(int movement_x) {
    GameTetrimino moved_tetrimino =
        rotation_model.tetrimino(current_tetrimino_).movedTetrimino(tetrimino_position_);

    if (!moved_tetrimino.canMoveLeftRight(movement_x))
      return false;

    moved_tetrimino.moveLeftRight(movement_x);

    if (game_.board().collides(moved_tetrimino))
      return false;

    tetrimino_position_.x += movement_x;
    completeMove(moved_tetrimino);
    return true;
  }

  void completeMove(GameTetrimino& moved_tetrimino) {
    if (!moved_tetrimino.canMoveDown()) {
      placeTetrimino(moved_tetrimino);
      return;
    }

    moved_tetrimino.moveDown();

    if (game_.board().collides(moved_tetrimino)) {
      moved_tetrimino.moveUp();
      placeTetrimino(moved_tetrimino);
    }
    else {
      ++tetrimino_position_.y;
    }
  }

  void placeTetrimino(const GameTetrimino& tetrimino) {
    game_.placeAndScore(tetrimino);
    current_tetrimino_ = TetriminoRotation(game_.currentTetrimino());
    tetrimino_position_ = Position::kZero;
  }

  friend std::ostream& operator<<(std::ostream& out, const PrimitiveState& s) {
    out << "width = " << GameState::width << ", height = " << GameState::height << "\n";
    out << static_cast<const typename GameState::ScorePolicy&>(s.game_);
    out << ", next tetrimino: " << s.game_.nextTetrimino() << "\n";
    out << "current tetrimino: " << s.current_tetrimino_ << ", position: " << s.tetrimino_position_
        << "\n";

    auto moved_tetrimino =
        rotation_model.tetrimino(s.current_tetrimino_).movedTetrimino(s.tetrimino_position_);
    auto board = s.game_.board();
    board.placeTetrimino(moved_tetrimino);
    out << board << "\n";

    s.game_.printRewardInfo(out);
    return out;
  }

  friend struct std::hash<PrimitiveState>;
};

}  // namespace tetris

namespace std {
template<class GameState>
struct hash<tetris::PrimitiveState<GameState>> {
  size_t operator()(const tetris::PrimitiveState<GameState>& s) const noexcept {
    std::size_t seed = 0;
    boost::hash_combine(seed, std::hash<decltype(s.game_)>{}(s.game_));
    boost::hash_combine(seed, s.tetrimino_position_.x);
    boost::hash_combine(seed, s.tetrimino_position_.y);
    boost::hash_combine(
        seed, tetris::PrimitiveState<GameState>::rotation_model.rotationHash(s.current_tetrimino_));
    return seed;
  }
};
}  // namespace std

#endif  // SMCTS_TETRIS_STATE_PRIMITVE_STATE_H_
