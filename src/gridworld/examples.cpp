#include "examples.hpp"

namespace gridworld {
namespace examples {

const World<10, 5> simple(".X........"
                          ".........."
                          "....#....."
                          "....#....."
                          ".........G",
                          -0.01,
                          {1.0});

const World<10, 5> simpleMultiGoal("....X....."
                                   ".........."
                                   ".........."
                                   ".........."
                                   ".G.G.G.G.G",
                                   -0.1,
                                   {0.1, 0.5, 0.2, 0.45, 0.91});

const World<19, 5> twoRooms(".........#........."
                            ".........#........."
                            "X.................."
                            ".........#........."
                            ".........#.......G.",
                            -0.01,
                            {1.0});

const World<19, 11> fourRooms(".........#........."
                              ".X.......#........."
                              "..................."
                              ".........#........."
                              ".........#........."
                              "####.#########.####"
                              ".........#........."
                              ".........#........."
                              "..................."
                              ".........#.......G."
                              ".........#.........",
                              -0.01,
                              {1.0});

const World<15, 15> fourRoomsBai(".......#......G"
                                 ".......#......."
                                 ".......#......."
                                 ".......#......."
                                 "..............."
                                 ".......#......."
                                 ".......#......."
                                 "#.######......."
                                 ".......####.###"
                                 ".......#......."
                                 ".......#......."
                                 ".......#......."
                                 "..............."
                                 ".......#......."
                                 "X......#.......",
                                 -0.01,
                                 {1.0});

const World<23, 11> eightRoomsBai(".....#...........#....G"
                                  ".....#.....#.....#....."
                                  ".....#.....#.....#....."
                                  "...........#.....#....."
                                  ".....#.....#..........."
                                  ".#####.....#####.#....."
                                  ".....####.##.....##.###"
                                  ".....#...........#....."
                                  ".....#.....#.....#....."
                                  ".....#.....#.....#....."
                                  "X..........#...........",
                                  -0.01,
                                  {1.0});

const World<20, 10> maze("X#...####.#......#.G"
                         ".#.#......#.####.#.."
                         ".#.#.####.#.#....#.."
                         "...#.#..#.#.#.####.."
                         ".#.#.#..#.#.#.#....#"
                         ".#.#.#......#.#.#.#G"
                         ".#.#.#.####.#.#.#.#."
                         ".#.#.#....#.#.#.#.#."
                         ".###.####.#.....#.#."
                         "..........#####.#...",
                         -0.001,
                         {0.9, 1.0});

}  // namespace examples
}  // namespace gridworld
