#ifndef SMCTS_TETRIS_GAME_H_
#define SMCTS_TETRIS_GAME_H_

#include <algorithm>
#include <boost/container/static_vector.hpp>
#include <iostream>
#include <vector>
#include "../util/random_util.hpp"
#include "board.hpp"
#include "scorepolicy/cleared_rows_score_policy.hpp"
#include "tetrimino_list.hpp"

namespace tetris {

template<size_t w,
         size_t h,
         template<class, class> class RewardPolicy_,
         class ScorePolicy_ = ClearedRowsScorePolicy,
         class I = TetriminoList::Iterator>
class Game : public ScorePolicy_ {
 public:
  static constexpr size_t width = w;
  static constexpr size_t height = h;
  static constexpr bool kAllowCompletingRowWithOverflowingTetrimino = false;
  using GameBoard = Board<width, height>;
  using GameTetrimino = Tetrimino<width, height>;
  using ScorePolicy = ScorePolicy_;
  using RewardPolicy = RewardPolicy_<GameBoard, GameTetrimino>;
  using TetriminoIterator = I;

 private:
  GameBoard board_;
  TetriminoIterator tetrimino_iterator_;
  RewardPolicy reward_policy_;

 public:
  explicit Game(I iterator) : tetrimino_iterator_(iterator) {}

  const GameBoard& board() const {
    return board_;
  }

  const TetriminoType& currentTetrimino() const {
    return *tetrimino_iterator_;
  }

  const TetriminoType& nextTetrimino() const {
    return *(tetrimino_iterator_ + 1);
  }

  bool gameOver() const {
    return !board_.currentHeightLessOrEqual(height);
  }

  void clearBoard() {
    board_.clear();
  }

  float reward() const {
    return reward_policy_.reward(board_);
  }

  bool operator==(const Game& b) const noexcept {
    return boardsEqual(b) && (tetrimino_iterator_ == b.tetrimino_iterator_) &&
           ScorePolicy::operator==(b);
  }

  bool boardsEqual(const Game& b) const noexcept {
    return board_ == b.board_;
  }

  void placeAndScore(const GameTetrimino& tetrimino) {
    board_.placeTetrimino(tetrimino);
    ++tetrimino_iterator_;

    int min_row = tetrimino.lowestRow();
    int max_row = min_row + tetrimino.tetriminoHeight();

    boost::container::static_vector<int, kMaxTetriminoHeight> cleared_rows;

    if (kAllowCompletingRowWithOverflowingTetrimino || !gameOver()) {
      // Check each changed row (top to bottom)
      for (int i = max_row; i >= min_row; i--) {
        if (board_.rowCompleted(i)) {
          board_.deleteRow(i);
          cleared_rows.push_back(i);
        }
      }
    }

    reward_policy_.rewardClearedRows(cleared_rows, tetrimino);
    ScorePolicy::scoreClearedRows(cleared_rows.size());
  }

  std::ostream& printRewardInfo(std::ostream& out) const {
    out << "Reward = " << reward_policy_.reward(board_) << "\n";
    out << "Reward policy: " << reward_policy_ << "\n";
    reward_policy_.printMetrics(board_, out);
    return out;
  }

  template<size_t w_, size_t h_, template<class, class> class R_, class S_, typename I_>
  friend std::ostream& operator<<(std::ostream& out, const Game<w_, h_, R_, S_, I_>& game);

  friend struct std::hash<Game>;
};

template<size_t width, size_t height, template<class, class> class R, class S, typename I>
std::ostream& operator<<(std::ostream& out, const Game<width, height, R, S, I>& game) {
  out << "width = " << width << ", height = " << height << "\n";
  out << static_cast<const S&>(game);
  out << ", next tetriminos: (";
  out << game.currentTetrimino() << ", ";
  out << game.nextTetrimino();
  out << ")"
      << "\n";

  out << game.board_ << "\n";

  game.printRewardInfo(out);
  return out;
}

}  // namespace tetris

namespace std {
template<size_t width, size_t height, template<class, class> class R, class S, typename I>
struct hash<tetris::Game<width, height, R, S, I>> {
  size_t operator()(const tetris::Game<width, height, R, S, I>& g) const noexcept {
    return std::hash<decltype(g.board_)>{}(g.board_);
  }
};
}  // namespace std

#endif  // SMCTS_TETRIS_GAME_H_
