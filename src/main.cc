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

using Transaction = std::set<int>;
using TransactionList = std::vector<Transaction>;
using SortedTransactionList = std::vector<std::pair<size_t, std::vector<int>>>;

struct Pattern {
  std::set<int> itemIds;
  std::set<int> transactionIds;
};
using PatternList = std::vector<Pattern>;

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

template <float (*J)(const PatternList &, const TransactionList &)>
std::pair<Pattern, std::queue<int>> findCore(
    const SortedTransactionList &residualDataset, const PatternList &patterns,
    const TransactionList &dataset) {
  std::queue<int> extensionList;
  SortedTransactionList sortedDataset = sortDataset(residualDataset);
  Pattern core;

  if (sortedDataset.size() == 0 || sortedDataset[0].second.size() == 0) {
    return std::make_pair(core, extensionList);
  }

  // std::cout << "SORTED:" << std::endl;
  // for (auto &&t : sortedDataset) {
  //   std::cout << "(" << t.first << ")  ";
  //   for (auto &&c : t.second) {
  //     std::cout << c << " ";
  //   }
  //   std::cout << std::endl;
  // }

  const auto [firstRowId, firstRow] = sortedDataset[0];
  auto firstRowIter = firstRow.begin();

  core.itemIds.insert(*firstRowIter);
  for (size_t trId = 0; trId < sortedDataset.size(); trId++) {
    const auto [originalTrId, trData] = sortedDataset[trId];
    if (std::find(trData.begin(), trData.end(), *firstRowIter) !=
        trData.end()) {
      core.transactionIds.insert(originalTrId);
    }
  }
  firstRowIter++;

  while (firstRowIter != firstRow.end()) {
    Pattern candidate = core;
    candidate.itemIds.insert(*firstRowIter);
    for (size_t trId = 0; trId < sortedDataset.size(); trId++) {
      const auto [originalTrId, trData] = sortedDataset[trId];
      if (std::find(trData.begin(), trData.end(), *firstRowIter) ==
          trData.end()) {
        candidate.transactionIds.erase(originalTrId);
      }
    }

    PatternList patternsWithCandidate = patterns;
    patternsWithCandidate.push_back(candidate);
    PatternList patternsWithCore = patterns;
    patternsWithCore.push_back(core);
    if (J(patternsWithCandidate, dataset) <= J(patternsWithCore, dataset)) {
      core = candidate;
    } else {
      extensionList.push(*firstRowIter);
    }

    firstRowIter++;
  }

  return std::make_pair(core, extensionList);
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

template <float (*J)(const PatternList &, const TransactionList &)>
Pattern extendCore(Pattern &core, std::queue<int> &extensionList,
                   const PatternList &patterns, const TransactionList &dataset,
                   float maxRowNoise, float maxColumnNoise) {
  bool addedItem = true;

  while (addedItem) {
    for (size_t trId = 0; trId < dataset.size(); trId++) {
      if (core.transactionIds.count(trId) == 0) {
        Pattern candidate = core;
        candidate.transactionIds.insert(trId);

        if (notTooNoisy(dataset, candidate, maxRowNoise, maxColumnNoise)) {
          PatternList patternsWithCandidate = patterns;
          patternsWithCandidate.push_back(candidate);
          PatternList patternsWithCore = patterns;
          patternsWithCore.push_back(core);
          if (J(patternsWithCandidate, dataset) <=
              J(patternsWithCore, dataset)) {
            core = candidate;
          }
        }
      }
    }

    addedItem = false;

    while (extensionList.size() > 0) {
      auto extension = extensionList.front();
      extensionList.pop();
      Pattern candidate = core;
      candidate.itemIds.insert(extension);

      if (notTooNoisy(dataset, candidate, maxRowNoise, maxColumnNoise)) {
        PatternList patternsWithCandidate = patterns;
        patternsWithCandidate.push_back(candidate);
        PatternList patternsWithCore = patterns;
        patternsWithCore.push_back(core);
        if (J(patternsWithCandidate, dataset) <= J(patternsWithCore, dataset)) {
          core = candidate;
          addedItem = true;
          break;
        }
      }
    }
  }

  return core;
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

template <float (*J)(const PatternList &, const TransactionList &)>
PatternList panda(int maxK, const TransactionList &dataset, float maxRowNoise,
                  float maxColumnNoise) {
  PatternList patterns;
  SortedTransactionList residualDataset = setToVectorDataset(dataset);

  for (int i = 0; i < maxK; i++) {
    auto [core, extensionList] =
        findCore<J>(residualDataset, patterns, dataset);
    // auto newCore = core;
    auto newCore = extendCore<J>(core, extensionList, patterns, dataset,
                                 maxRowNoise, maxColumnNoise);

    PatternList patternsWithNewCore = patterns;
    patternsWithNewCore.push_back(newCore);
    if (J(patterns, dataset) < J(patternsWithNewCore, dataset)) {
      // J cannot be improved any more
      // std::cout << "J cannot be improved any more" << std::endl;
      break;
    }

    patterns = patternsWithNewCore;
    residualDataset = buildResidualDataset(dataset, patterns);

    // std::cout << std::endl;
    // std::cout << std::endl;
  }

  return patterns;
}

float testErrorFunction(const PatternList &patterns,
                        const TransactionList &dataset) {
  // std::cout << "Patterns:" << std::endl;
  // for (auto &&p : patterns) {
  //   for (auto &&i : p.itemIds) {
  //     std::cout << i << " ";
  //   }
  //   std::cout << "(";
  //   for (auto &&i : p.transactionIds) {
  //     std::cout << i << " ";
  //   }
  //   std::cout << ")";
  //   std::cout << std::endl;
  // }

  int noise = 0;
  for (size_t trId = 0; trId < dataset.size(); trId++) {
    Transaction falsePositives = dataset[trId];
    Transaction falseNegatives = dataset[trId];

    for (auto &&pattern : patterns) {
      if (pattern.transactionIds.count(trId) > 0) {
        for (auto &&i : pattern.itemIds) {
          if (dataset[trId].count(i) > 0) {
            falsePositives.erase(i);
          }
          falseNegatives.erase(i);
        }
      }
    }

    falsePositives.merge(falseNegatives);
    noise += falsePositives.size();

    // std::cout << "Row: ";
    // for (auto &&i : dataset[trId]) {
    //   std::cout << i << " ";
    // }
    // std::cout << "  Out of place:";
    // for (auto &&i : falsePositives) {
    //   std::cout << i << " ";
    // }
    // std::cout << std::endl;
  }

  int complexity = 0;
  for (auto &&pattern : patterns) {
    complexity += pattern.transactionIds.size();
    complexity += pattern.itemIds.size();
  }

  // std::cout << "COMPLEXITY: " << complexity << " NOISE: " << noise
  //           << " COST: " << 0.5 * complexity + noise << std::endl;

  // std::cout << std::endl;
  // std::cout << std::endl;

  return 0.5 * complexity + noise;
}

int main(int argc, char *argv[]) {
  TransactionList dataset = readTransactions(argv[1]);

  // std::cout << "Dataset:" << std::endl;
  // for (auto &&t : dataset) {
  //   for (auto &&c : t) {
  //     std::cout << c << " ";
  //   }
  //   std::cout << std::endl;
  // }
  // std::cout << std::endl;

  auto patterns = panda<testErrorFunction>(8, dataset, 1.0, 1.0);

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
