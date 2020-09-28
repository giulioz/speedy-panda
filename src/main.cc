#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <queue>
#include <set>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "panda.h"

// Read transactions from a file given path, line by line and space separated
TransactionList<int> readIntTransactions(const std::string &path) {
  TransactionList<int> result;

  std::ifstream infile(path);
  std::string line;
  while (std::getline(infile, line)) {
    std::stringstream ss(line);
    int val;
    std::set<int> items;

    while (ss >> val) {
      items.insert(val);
      if (ss.peek() == ' ') ss.ignore();
    }

    result.addTransaction(items);
  }

  return result;
}

int main(int argc, char *argv[]) {
  auto dataset = readIntTransactions(argv[1]);

  auto patterns = panda(8, dataset, 1.0, 1.0);

  std::cout << "Patterns:" << std::endl;
  for (auto &&p : patterns.patterns) {
    for (auto &&i : p.itemIds) {
      std::cout << i << " ";
    }
    // std::cout << "(";
    // for (auto &&i : p.transactionIds) {
    //   std::cout << i << " ";
    // }
    // std::cout << ")";
    std::cout << " (" << p.transactionIds.size() << ")";
    std::cout << std::endl;
  }

  return 0;
}
