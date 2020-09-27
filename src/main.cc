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

// General cost function from the paper
float standardCostFunction(const PatternList &patterns,
                           const TransactionList &dataset) {
  // Noise from patterns vs dataset (Ja)
  int noise = 0;
  for (size_t trId = 0; trId < dataset.size(); trId++) {
    std::set<int> falsePositives;
    std::set<int> falseNegatives = dataset[trId];

    for (auto &&pattern : patterns) {
      if (pattern.transactionIds.count(trId) > 0) {
        for (auto &&i : pattern.itemIds) {
          if (dataset[trId].count(i) == 0) {
            falsePositives.insert(i);
          }
          falseNegatives.erase(i);
        }
      }
    }

    falsePositives.merge(falseNegatives);
    noise += falsePositives.size();
  }

  // Pattern set complexity (Jh)
  int complexity = 0;
  for (auto &&pattern : patterns) {
    complexity += pattern.transactionIds.size();
    complexity += pattern.itemIds.size();
  }

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
