#include "math_util.hpp"

namespace math_util {

static constexpr int kLogLookupTableSize = 50000;
using LogLookupTable = std::array<float, kLogLookupTableSize>;

static LogLookupTable createLogLookupTable() {
  LogLookupTable table;
  for (int x = 0; x < kLogLookupTableSize; ++x) {
    table[x] = std::log(static_cast<float>(x));
  }
  return table;
}

static const LogLookupTable kLogLookupTable = createLogLookupTable();

float fastLog(int x) {
  if (x < kLogLookupTableSize)
    return kLogLookupTable[x];
  else
    return std::log(static_cast<float>(x));
}

void PowLookupTable::updateBase(float base) {
  float value = 1.0;
  for (int i = 0; i < kSize; ++i) {
    table_[i] = value;
    value *= base;
  }
  base_ = base;
}

float PowLookupTable::fastPow(int exponent) const {
  if (exponent < kSize)
    return table_[exponent];
  else
    return std::pow(base_, exponent);
}

}  // namespace math_util
