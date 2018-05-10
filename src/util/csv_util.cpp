#include "csv_util.hpp"

namespace csv {

bool fileExists(const std::string& file) {
  std::ifstream infile(file);
  return infile.good();
}

void Data::write(const std::string& file) {
  bool file_exists = fileExists(file);

  std::ofstream outfile(file, std::ios_base::app | std::ios_base::out);

  if (!file_exists) {
    for (const auto& value : values) {
      outfile << value.name;

      if (&value != &values.back())
        outfile << ", ";
    }

    outfile << "\n\n";
  }

  for (const auto& value : values) {
    outfile << value.v;

    if (&value != &values.back())
      outfile << ", ";
  }

  outfile << "\n";
  outfile.close();
}

}  // namespace csv
