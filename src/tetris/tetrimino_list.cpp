#include "tetrimino_list.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace tetris {

void TetriminoList::createRandomList(size_t size) {
  static std::random_device rd;
  static std::mt19937 mersenne_engine(rd());
  tetriminos.reserve(size);
  std::uniform_int_distribution<> dist_types(static_cast<int>(TetriminoType::First),
                                             static_cast<int>(TetriminoType::Last));

  for (size_t i = 0; i < size; i++) {
    tetriminos.emplace_back(static_cast<TetriminoType>(dist_types(mersenne_engine)));
  }
}

void TetriminoList::createStaticList() {
  for (auto i = static_cast<int>(TetriminoType::First); i <= static_cast<int>(TetriminoType::Last);
       i++) {
    tetriminos.emplace_back(static_cast<TetriminoType>(i));
  }
}

void TetriminoList::writeToFile(const std::string& path) {
  std::ofstream outfile(path, std::ios::out | std::ios::binary);
  outfile.write(reinterpret_cast<char*>(&tetriminos[0]), tetriminos.size());
  outfile.close();
}

void TetriminoList::writeToMdpTetrisFile(const std::string& path) {
  static const std::array<int, 7> kMdpTetisIndices = {0, 6, 5, 1, 4, 3, 2};

  std::ofstream outfile(path, std::ios::out);
  outfile << tetriminos.size() << "\n";

  for (auto& type : tetriminos) {
    outfile << kMdpTetisIndices[static_cast<size_t>(type)] << "\n";
  }

  outfile.close();
}

void TetriminoList::readFromFile(const std::string& path) {
  tetriminos.clear();
  std::ifstream infile(path, std::ios::in | std::ifstream::binary);
  std::istreambuf_iterator<char> iter(infile);
  std::istreambuf_iterator<char> end{};
  std::transform(iter, end, std::back_inserter(tetriminos), [](const char& input_char) {
    return static_cast<TetriminoType>(input_char);
  });
  infile.close();
}

double TetriminoList::entropy() {
  std::array<size_t, static_cast<size_t>(TetriminoType::Last) + 1> counts{};

  for (auto& type : tetriminos) {
    counts[static_cast<int>(type)] += 1;
  }

  double entropy = 0;

  for (auto& count : counts) {
    double freq = static_cast<double>(count) / tetriminos.size();
    entropy += freq * (log(freq) / log(2));
  }
  entropy *= -1;

  return entropy;
}

}  // namespace tetris
