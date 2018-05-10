#ifndef SMCTS_MATH_UTIL_H_
#define SMCTS_MATH_UTIL_H_

#include <array>
#include <cmath>

namespace math_util {

extern float fastLog(int x);

class PowLookupTable {
 private:
  static constexpr int kSize = 1000;
  float base_ = 1.0;
  std::array<float, kSize> table_ = {1.0};

 public:
  void updateBase(float base);
  float fastPow(int exponent) const;
};

}  // namespace math_util

#endif  // SMCTS_MATH_UTIL_H_
