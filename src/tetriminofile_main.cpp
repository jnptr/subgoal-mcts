#include <boost/optional.hpp>
#include <boost/program_options.hpp>
#include <cstdlib>
#include <iostream>
#include "tetris/tetrimino_list.hpp"

int main(int argc, char** argv) {
  namespace po = boost::program_options;

  int count = 100000;
  std::string tetrimino_file;

  // Declare the supported options.
  po::options_description desc("Allowed options");
  // clang-format off
  desc.add_options()
      ("help", "produce help message")
      ("count,c", po::value<int>(&count)->default_value(count), "tetrimino count")
      ("mdptetris,m", "create MdpTetris tetrimino file")
      ("tetrimino-file,f", po::value<std::string>(&tetrimino_file), "output tetrimino file");
  // clang-format on

  po::positional_options_description p;
  p.add("tetrimino-file", -1);

  po::variables_map vm;

  try {
    po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);

    if (vm.count("help")) {
      std::cout << desc << "\n";
      return 1;
    }

    po::notify(vm);
  }
  catch (boost::program_options::error& e) {
    std::cerr << "ERROR: " << e.what() << "\n\n";
    return 1;
  }

  tetris::TetriminoList::instance().createRandomList(count);

  if (vm.count("tetrimino-file"))
    tetris::TetriminoList::instance().writeToFile(tetrimino_file);
  else
    std::cerr << "ERROR: output tetrimino file needs to be specified"
              << "\n\n";

  if (vm.count("mdptetris"))
    tetris::TetriminoList::instance().writeToMdpTetrisFile(tetrimino_file + ".mdptetris");

  return 0;
}
