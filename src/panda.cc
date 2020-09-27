#include "panda.h"

SortedTransactionList setToVectorDataset(const TransactionList &dataset) {
  SortedTransactionList result;
  for (size_t i = 0; i < dataset.size(); i++) {
    std::vector<int> elements(dataset[i].begin(), dataset[i].end());
    result.push_back(std::make_pair(i, elements));
  }
  return result;
}

SortedTransactionList sortDataset(const SortedTransactionList &dataset) {
  std::map<int, size_t> itemsFreqMap;
  for (size_t trId = 0; trId < dataset.size(); trId++) {
    for (auto &&i : dataset[trId].second) {
      if (itemsFreqMap.find(i) == itemsFreqMap.end()) {
        itemsFreqMap[i] = 1;
      } else {
        itemsFreqMap[i]++;
      }
    }
  }

  SortedTransactionList sortedDataset;
  for (size_t i = 0; i < dataset.size(); i++) {
    std::vector<int> elements(dataset[i].second.begin(),
                              dataset[i].second.end());
    std::sort(elements.begin(), elements.end(), [&itemsFreqMap](int a, int b) {
      return itemsFreqMap[a] > itemsFreqMap[b];
    });
    sortedDataset.push_back(std::make_pair(dataset[i].first, elements));
  }

  std::sort(sortedDataset.begin(), sortedDataset.end(),
            [&itemsFreqMap](std::pair<int, std::vector<int>> a,
                            std::pair<int, std::vector<int>> b) {
              size_t aFreq = 0;
              for (auto &&i : a.second) {
                aFreq += itemsFreqMap[i];
              }
              size_t bFreq = 0;
              for (auto &&i : b.second) {
                bFreq += itemsFreqMap[i];
              }
              return aFreq > bFreq;
            });

  return sortedDataset;
}

bool notTooNoisy(const TransactionList &dataset, const Pattern &core,
                 float maxRowNoise, float maxColumnNoise) {
  bool ok = true;

  const auto maxColumn = (1 - maxColumnNoise) * core.transactionIds.size();
  for (auto &&j : core.itemIds) {
    int columnSum = 0;
    for (auto &&i : core.transactionIds) {
      columnSum += dataset[i].count(j);
    }
    ok &= columnSum >= maxColumn;
  }

  const auto maxRow = (1 - maxRowNoise) * core.itemIds.size();
  for (auto &&i : core.transactionIds) {
    int rowSum = 0;
    for (auto &&j : core.itemIds) {
      rowSum += dataset[i].count(j);
    }
    ok &= rowSum >= maxRow;
  }

  return ok;
}

SortedTransactionList buildResidualDataset(const TransactionList &dataset,
                                           const PatternList &patterns) {
  SortedTransactionList result;

  for (size_t trId = 0; trId < dataset.size(); trId++) {
    std::vector<int> resultRow;

    for (const auto &i : dataset[trId]) {
      bool foundInPattern = false;
      for (const auto &p : patterns) {
        bool inColumns = p.itemIds.count(i) > 0;
        bool inTransactions = p.transactionIds.count(trId) > 0;

        if (inColumns && inTransactions) {
          foundInPattern = true;
          break;
        }
      }

      if (!foundInPattern) {
        resultRow.push_back(i);
      }
    }

    if (resultRow.size() > 0) {
      result.push_back(std::make_pair(trId, resultRow));
    }
  }

  return result;
}
