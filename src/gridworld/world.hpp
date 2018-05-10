#ifndef SMCTS_GRIDWORLD_WORLD_H_
#define SMCTS_GRIDWORLD_WORLD_H_

#include <algorithm>
#include <array>
#include <cassert>
#include <iostream>
#include <vector>
#include "position.hpp"

namespace gridworld {

template<size_t w, size_t h>
class World {
 public:
  static constexpr size_t width = w;
  static constexpr size_t height = h;
  static constexpr char kWallChar = '#';
  static constexpr char kGoalChar = 'G';
  static constexpr char kAgentChar = 'X';
  static constexpr char kEmptyChar = ' ';

  World(std::string world_string, float default_reward, std::vector<float> goal_rewards) {
    int current_goal = 0;

    for (int y = 0; y < static_cast<int>(height); ++y) {
      for (int x = 0; x < static_cast<int>(width); ++x) {
        char current_char = world_string.at(y * width + x);

        switch (current_char) {
          case kWallChar:
            cells_[y][x] = {CellType::Wall, 0.0};
            break;
          case kGoalChar:
            cells_[y][x] = {CellType::Goal, goal_rewards.at(current_goal)};
            ++current_goal;
            break;
          case kAgentChar:
            start_position_ = {x, y};
            /* fallthrough */
          default:
            cells_[y][x] = {CellType::Empty, default_reward};
            break;
        }
      }
    }
  }

  const Position& startPosition() const {
    return start_position_;
  }

  float rewardAtPosition(Position p) const {
    return cell(p).reward;
  }

  bool isLegalPosition(Position p) const {
    if (p.x < 0 || p.y < 0 || p.x >= static_cast<int>(width) || p.y >= static_cast<int>(height))
      return false;
    return cell(p).type != CellType::Wall;
  }

  bool isFinalPosition(Position p) const {
    return cell(p).type == CellType::Goal;
  }

  std::ostream& print(Position agent_position, std::ostream& out = std::cout) const {
    static const std::string border = std::string(width + 2, kWallChar);
    out << border << "\n";

    for (int y = 0; y < static_cast<int>(height); ++y) {
      out << kWallChar;
      for (int x = 0; x < static_cast<int>(width); ++x) {
        Position p = {x, y};
        if (p == agent_position) {
          out << kAgentChar;
        }
        else {
          const Cell& current_cell = cell(p);
          out << charForCell(current_cell);
        }
      }
      out << kWallChar << "\n";
    }

    return out << border << "\n";
  }

 private:
  enum class CellType : char { Empty, Wall, Goal };

  struct Cell {
    CellType type;
    float reward;
  };

  std::array<std::array<Cell, w>, h> cells_;
  Position start_position_;

  const Cell& cell(Position p) const {
    assert(p.x >= 0);
    assert(p.y >= 0);
    assert(p.x < static_cast<int>(width));
    assert(p.y < static_cast<int>(height));
    return cells_[p.y][p.x];
  }

  char charForCell(const Cell& c) const {
    switch (c.type) {
      case CellType::Wall:
        return kWallChar;
      case CellType::Goal:
        return kGoalChar;
      default:
        return kEmptyChar;
    }
  }

  template<size_t w_, size_t h_>
  friend std::ostream& operator<<(std::ostream& out, const World<w_, h_>& world);
};

template<size_t width, size_t height>
std::ostream& operator<<(std::ostream& out, const World<width, height>& world) {
  return world.print(world.start_position_, out);
}

}  // namespace gridworld

#endif  // SMCTS_GRIDWORLD_WORLD_H_
