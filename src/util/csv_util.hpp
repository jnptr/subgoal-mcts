#ifndef SMCTS_CSV_UTIL_H_
#define SMCTS_CSV_UTIL_H_

#include <boost/variant.hpp>
#include <fstream>
#include <iostream>
#include <vector>

namespace csv {

struct Value {
  std::string name;
  boost::variant<double, float, int, bool> v;
};

struct Data {
  std::vector<Value> values;

  void write(const std::string& file);
};

}  // namespace csv

#endif  // SMCTS_CSV_UTIL_H_
