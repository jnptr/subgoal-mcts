#ifndef SMCTS_MCTS_PATH_H_
#define SMCTS_MCTS_PATH_H_

#include <vector>

namespace mcts {

template<class Node>
class Path {
 private:
  std::vector<Node*> nodes_;
  int depth_;

 public:
  explicit Path(Node* root) {
    nodes_ = {root};
    depth_ = 0;
  }

  void addNode(Node* node) {
    nodes_.push_back(node);
    depth_ += node->state.numberOfSteps();
  }

  const std::vector<Node*>& nodes() const {
    return nodes_;
  }

  const Node& currentNode() const {
    return *nodes_.back();
  }

  int depth() const {
    return depth_;
  }

  int nodeDepth() const {
    return nodes_.size();
  }
};

}  // namespace mcts

#endif  // SMCTS_MCTS_PATH_H_
