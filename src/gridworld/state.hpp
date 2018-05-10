#ifndef SMCTS_GRIDWORLD_STATE_H_
#define SMCTS_GRIDWORLD_STATE_H_

#include <algorithm>
#include <array>
#include <iostream>
#include <vector>
#include "../util/random_util.hpp"
#include "world.hpp"

// Workaround to avoid having to explicitly specify world type in pre-C++17
// https://stackoverflow.com/questions/15148749/pointer-to-class-member-as-a-template-parameter
#define GRIDWORLD_STATE(WorldRef) gridworld::State<decltype(WorldRef), WorldRef>
#define GRIDWORLD_STATE_INF(WorldRef) gridworld::State<decltype(WorldRef), WorldRef, true>

namespace gridworld {
enum class Action : char { North = 0, South, East, West, First = North, Last = West };
static constexpr std::array<Action, 4> kActions = {
    Action::North, Action::South, Action::East, Action::West};

template<class W, const W& world, bool infinite = false>
class State {
 private:
  Position position_;

  Position positionForAction(Action a) const {
    switch (a) {
      case Action::North:
        return position_.north();
      case Action::South:
        return position_.south();
      case Action::East:
        return position_.east();
      case Action::West:
        return position_.west();
      default:
        return position_;
    }
  }

 public:
  State() {
    position_ = world.startPosition();
  }

  Position position() const {
    return position_;
  }

  void evolve(int /*max_steps*/, float /*gamma*/) {
    if (infinite && world.isFinalPosition(position_)) {
      position_ = world.startPosition();
      return;
    }

    std::uniform_int_distribution<> dist_actions(static_cast<int>(Action::First),
                                                 static_cast<int>(Action::Last));

    while (true) {
      auto action = static_cast<Action>(dist_actions(random_util::engine));
      Position p = positionForAction(action);
      if (world.isLegalPosition(p)) {
        position_ = p;
        return;
      }
    }
  }

  void successorStates(std::vector<State>* v) const {
    if (infinite && world.isFinalPosition(position_)) {
      v->emplace_back(*this);
      v->back().position_ = world.startPosition();
      return;
    }

    v->reserve(kActions.size());

    for (const auto& action : kActions) {
      Position p = positionForAction(action);
      if (world.isLegalPosition(p)) {
        v->emplace_back(*this);
        v->back().position_ = p;
      }
    }
  }

  bool isFinal() const {
    if (infinite)
      return false;
    else
      return world.isFinalPosition(position_);
  }

  bool isSubgoal() const {
    int num_legal_positions = 0;
    for (const auto& action : kActions) {
      Position p = positionForAction(action);
      if (world.isLegalPosition(p))
        ++num_legal_positions;
    }
    return num_legal_positions < 3 || (infinite && world.isFinalPosition(position_));
  }

  float reward() const {
    return world.rewardAtPosition(position_);
  }

  int numberOfSteps() const {
    return 1;
  }

  bool operator==(const State& rhs) const {
    return position_ == rhs.position_;
  }

  friend std::ostream& operator<<(std::ostream& out, const State& s) {
    return world.print(s.position_, out);
  }

  friend struct std::hash<State>;
};

}  // namespace gridworld

namespace std {
template<class W, const W& world, bool infinite>
struct hash<gridworld::State<W, world, infinite>> {
  size_t operator()(const gridworld::State<W, world, infinite>& s) const noexcept {
    return std::hash<gridworld::Position>{}(s.position_);
  }
};
}  // namespace std

#endif  // SMCTS_GRIDWORLD_STATE_H_
