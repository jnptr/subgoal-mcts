#ifndef SMCTS_GRIDWORLD_EXAMPLES_H_
#define SMCTS_GRIDWORLD_EXAMPLES_H_

#include "world.hpp"

namespace gridworld {
namespace examples {

extern const World<10, 5> simple;
extern const World<10, 5> simpleMultiGoal;
extern const World<19, 5> twoRooms;
extern const World<19, 11> fourRooms;
extern const World<15, 15> fourRoomsBai;
extern const World<23, 11> eightRoomsBai;
extern const World<20, 10> maze;

}  // namespace examples
}  // namespace gridworld

#endif  // SMCTS_GRIDWORLD_EXAMPLES_H_
