#include "costutils.h"

int calcPatternsComplexity(const PatternList &patterns) {
  int complexity = 0;
  for (auto &&pattern : patterns) {
    complexity += pattern.transactionIds.size();
    complexity += pattern.itemIds.size();
  }

  return complexity;
}

int calcNoise(const PatternList &patterns, const TransactionList &dataset) {
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

  return noise;
}
