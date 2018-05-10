#ifndef SMCTS_TETRIS_BITSET_2D_H_
#define SMCTS_TETRIS_BITSET_2D_H_

#include <bitset>
#include <cassert>
#include <iostream>

namespace tetris {

template<size_t w, size_t h>
class Bitset2D {
 private:
  std::bitset<h * w> bitset_;

 public:
  static constexpr size_t width = w;
  static constexpr size_t height = h;

  inline size_t index(size_t x, size_t y) const noexcept {
    assert(x < width);
    assert(y < height);
    return y * width + (width - 1 - x);
  }

  bool test(size_t x, size_t y) const noexcept {
    return bitset_[index(x, y)];
  }

  Bitset2D& set(size_t x, size_t y) noexcept {
    bitset_.set(index(x, y));
    return *this;
  }

  Bitset2D& reset(size_t x, size_t y) noexcept {
    bitset_.reset(index(x, y));
    return *this;
  }

  Bitset2D& operator&=(const Bitset2D& rhs) noexcept {
    bitset_ &= rhs.bitset_;
    return *this;
  }

  Bitset2D& operator|=(const Bitset2D& rhs) noexcept {
    bitset_ |= rhs.bitset_;
    return *this;
  }

  Bitset2D& operator^=(const Bitset2D& rhs) noexcept {
    bitset_ ^= rhs.bitset_;
    return *this;
  }

  Bitset2D& operator<<=(size_t pos) noexcept {
    bitset_ <<= pos;
    return *this;
  }

  Bitset2D& operator>>=(size_t pos) noexcept {
    bitset_ >>= pos;
    return *this;
  }

  Bitset2D& set() noexcept {
    bitset_.set();
    return *this;
  }

  Bitset2D& reset() noexcept {
    bitset_.reset();
    return *this;
  }

  Bitset2D operator~() const noexcept {
    Bitset2D result = (*this);
    return result.flip();
  }

  Bitset2D& flip() noexcept {
    bitset_.flip();
    return *this;
  }

  Bitset2D& isolateRow(size_t row) noexcept {
    *this >>= row * width;
    *this <<= (height - 1) * width;
    return *this;
  }

  Bitset2D isolatedRow(size_t row) const noexcept {
    Bitset2D result = *this;
    return result.isolateRow(row);
  }

  size_t count() const noexcept {
    return bitset_.count();
  }

  constexpr size_t size() const noexcept {
    return bitset_.size();
  }

  bool operator==(const Bitset2D& rhs) const noexcept {
    return bitset_ == rhs.bitset_;
  }

  bool operator!=(const Bitset2D& rhs) const noexcept {
    return bitset_ != rhs.bitset_;
  }

  bool all() const noexcept {
    return bitset_.all();
  }

  bool any() const noexcept {
    return bitset_.any();
  }

  bool none() const noexcept {
    return bitset_.none();
  }

  Bitset2D operator<<(size_t pos) const noexcept {
    Bitset2D result = *this;
    return result <<= pos;
  }

  Bitset2D operator>>(size_t pos) const noexcept {
    Bitset2D result = *this;
    return result >>= pos;
  }

  template<size_t w_, size_t h_>
  friend std::ostream& operator<<(std::ostream& out, const Bitset2D<w_, h_>& bitset_2d);

  friend struct std::hash<Bitset2D>;
};

template<size_t width, size_t height>
Bitset2D<width, height> operator&(Bitset2D<width, height> lhs,
                                  const Bitset2D<width, height>& rhs) noexcept {
  return lhs &= rhs;
}

template<size_t width, size_t height>
Bitset2D<width, height> operator|(Bitset2D<width, height> lhs,
                                  const Bitset2D<width, height>& rhs) noexcept {
  return lhs |= rhs;
}

template<size_t width, size_t height>
Bitset2D<width, height> operator^(Bitset2D<width, height> lhs,
                                  const Bitset2D<width, height>& rhs) noexcept {
  return lhs ^= rhs;
}

template<size_t width, size_t height>
std::ostream& operator<<(std::ostream& out, const Bitset2D<width, height>& bitset_2d) {
  auto board_string = bitset_2d.bitset_.to_string('.', 'X');
  for (size_t i = 0; i < board_string.size(); i += width)
    out << board_string.substr(i, width) << "\n";

  return out;
}

}  // namespace tetris

namespace std {
template<size_t width, size_t height>
struct hash<tetris::Bitset2D<width, height>> {
  size_t operator()(const tetris::Bitset2D<width, height>& b) const noexcept {
    return std::hash<decltype(b.bitset_)>{}(b.bitset_);
  }
};
}  // namespace std

#endif  // SMCTS_TETRIS_BITSET_2D_H_
