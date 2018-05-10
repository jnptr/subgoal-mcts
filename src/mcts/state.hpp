#ifndef SMCTS_MCTS_STATE_H_
#define SMCTS_MCTS_STATE_H_

#include <iostream>

namespace mcts {

class State {
 public:
  virtual void evolve() = 0;
  virtual bool isFinal() = 0;
  virtual float reward() = 0;
};

}  // namespace mcts

#endif  // SMCTS_MCTS_STATE_H_
