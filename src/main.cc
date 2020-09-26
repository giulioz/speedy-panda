#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using transactions = std::vector<std::vector<int>>;

transactions readTransactions(const std::string& path) {
  transactions result;

  std::ifstream infile(path);
  std::string line;
  while (std::getline(infile, line)) {
    std::stringstream ss(line);
    int val;
    std::vector<int> row;

    while (ss >> val) {
      row.push_back(val);
      if (ss.peek() == ' ') ss.ignore();
    }

    result.push_back(row);
  }

  return result;
}

int main(int argc, char *argv[]) {
  transactions tr = readTransactions(argv[1]);
  for (auto &&t : tr) {
    for (auto &&c : t) {
      std::cout << c << " ";
    }
    std::cout << std::endl;
  }

  return 0;
}
