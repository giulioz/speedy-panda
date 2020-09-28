#pragma once

#include <set>

#include "panda.h"

template <typename T>
size_t calcNoise(const PatternList<T> &patterns,
                 const TransactionList<T> &dataset) {
  size_t noise = 0;
  for (size_t trId = 0; trId < dataset.size(); trId++) {
    std::set<T> falsePositives;
    std::set<T> falseNegatives(dataset.transactions[trId].items.cbegin(),
                               dataset.transactions[trId].items.cend());

    for (auto &&pattern : patterns.patterns) {
      if (pattern.transactionIds.count(trId) >0) {
        for (auto &&i : pattern.itemIds) {
          if (!dataset.transactions[trId].includes(i)) {
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
