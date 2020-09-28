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

#include "costutils.h"
#include "panda.h"

// Read transactions from a file given path, line by line and space separated
TransactionList readTransactions(const std::string &path) {
  TransactionList result;

  std::ifstream infile(path);
  std::string line;
  while (std::getline(infile, line)) {
    std::stringstream ss(line);
    int val;
    Transaction tr;

    while (ss >> val) {
      tr.insert(val);
      if (ss.peek() == ' ') ss.ignore();
    }

    result.push_back(tr);
  }

  return result;
}

float standardCostFunction(const PatternList &patterns,
                           const TransactionList &dataset) {
  // Noise from patterns vs dataset (Ja)
  int noise = calcNoise(patterns, dataset);

  // Pattern set complexity (Jh)
  int complexity = calcPatternsComplexity(patterns);

  // Weight the two measures (Jp)
  return 0.8 * complexity + noise;
}

int main(int argc, char *argv[]) {
  TransactionList dataset = readTransactions(argv[1]);

  auto patterns = panda<standardCostFunction>(8, dataset, 1.0, 1.0);

  std::cout << "Patterns:" << std::endl;
  for (auto &&p : patterns) {
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
