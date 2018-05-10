#ifndef SMCTS_TETRIS_TETRIMINO_LIST_H_
#define SMCTS_TETRIS_TETRIMINO_LIST_H_

#include <iostream>
#include <vector>
#include "tetrimino_type.hpp"

namespace tetris {

class TetriminoList {
 private:
  std::vector<TetriminoType> tetriminos;

 public:
  struct Iterator {
   private:
    size_t index_ = 0;

   public:
    const TetriminoType& operator*() const {
      return TetriminoList::instance().at(index_);
    }

    bool operator==(const Iterator& rhs) const {
      return index_ == rhs.index_;
    }

    Iterator& operator++() {
      ++index_;
      return *this;
    }

    Iterator& operator+=(size_t i) {
      index_ += i;
      return *this;
    }

    friend Iterator operator+(Iterator a, size_t i) {
      a += i;
      return a;
    }
  };

  static TetriminoList& instance() {
    static TetriminoList instance;
    return instance;
  }

  const TetriminoType& at(size_t index) const {
    return tetriminos[index % tetriminos.size()];
  }

  const TetriminoType& operator[](size_t index) const {
    return at(index);
  }

  Iterator begin() const {
    return Iterator{};
  }

  size_t count() const {
    return tetriminos.size();
  }

  void createRandomList(size_t size);
  void createStaticList();
  void writeToFile(const std::string& path);
  void writeToMdpTetrisFile(const std::string& path);
  void readFromFile(const std::string& path);
  double entropy();
};

}  // namespace tetris

#endif  // SMCTS_TETRIS_TETRIMINO_LIST_H_
