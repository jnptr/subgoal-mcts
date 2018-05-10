#ifndef SMCTS_RANDOM_UTIL_H_
#define SMCTS_RANDOM_UTIL_H_

#include <random>

namespace random_util {

// Using internal linkage here because of non-optimizable access overhead
// for thread local storage when defined in a different compilation unit
// See:
// http://stackoverflow.com/questions/13106049/what-is-the-performance-penalty-of-c11-thread-local-variables-in-gcc-4-8#answer-13123870

static thread_local std::random_device rd;
static thread_local std::mt19937_64 engine(rd());

}  // namespace random_util
#endif  // SMCTS_RANDOM_UTIL_H_
