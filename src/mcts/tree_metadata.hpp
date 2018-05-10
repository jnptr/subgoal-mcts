#ifndef SMCTS_MCTS_TREE_METADATA_H_
#define SMCTS_MCTS_TREE_METADATA_H_

#include <iostream>

namespace mcts {

class TreeMetadata {
 private:
  double max_depth_ = 0;
  double max_node_depth_ = 0;
  double average_depth_ = 0;
  double average_node_depth_ = 0;
  double iteration_count_ = 0;

 public:
  double maxDepth() const {
    return max_depth_;
  }

  double maxNodeDepth() const {
    return max_node_depth_;
  }

  double averageDepth() const {
    return average_depth_;
  }

  double averageNodeDepth() const {
    return average_node_depth_;
  }

  double iterationCount() const {
    return iteration_count_;
  }

  template<class P>
  void updateFromPath(const P& path) {
    iteration_count_ += 1;

    int current_depth = path.depth();
    int current_node_depth = path.nodeDepth();

    if (current_depth > max_depth_)
      max_depth_ = current_depth;

    if (current_node_depth > max_node_depth_)
      max_node_depth_ = current_node_depth;

    average_depth_ = average_depth_ + (1.0 / iteration_count_) * (current_depth - average_depth_);
    average_node_depth_ =
        average_node_depth_ + (1.0 / iteration_count_) * (current_node_depth - average_node_depth_);
  }

  TreeMetadata& operator+=(const TreeMetadata& rhs) {
    iteration_count_ += rhs.iteration_count_;

    double ratio = rhs.iteration_count_ / iteration_count_;
    max_depth_ = max_depth_ + ratio * (rhs.max_depth_ - max_depth_);
    max_node_depth_ = max_node_depth_ + ratio * (rhs.max_node_depth_ - max_node_depth_);

    average_depth_ = average_depth_ + ratio * (rhs.average_depth_ - average_depth_);
    average_node_depth_ =
        average_node_depth_ + ratio * (rhs.average_node_depth_ - average_node_depth_);
    return *this;
  }
};

inline std::ostream& operator<<(std::ostream& out, const TreeMetadata& m) {
  out << "Max depth: " << m.maxDepth() << " (" << m.maxNodeDepth() << " nodes)\n";
  out << "Avg depth: " << m.averageDepth() << " (" << m.averageNodeDepth() << " nodes)\n";
  out << "Iterations: " << m.iterationCount() << "\n";
  return out;
}

}  // namespace mcts

#endif  // SMCTS_MCTS_TREE_METADATA_H_
