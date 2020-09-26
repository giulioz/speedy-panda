#include <algorithm>
#include <fstream>
#include <iostream>
#include <set>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

using Transaction = std::set<int>;
using TransactionList = std::vector<Transaction>;
using SortedTransactionList = std::vector<std::pair<int, Transaction>>;

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

SortedTransactionList sortDataset(TransactionList dataset) {
  SortedTransactionList sortDataset;

  for (size_t i = 0; i < dataset.size(); i++) {
    sortDataset.push_back(std::make_pair(i, dataset[i]));
  }

  return sortDataset;
}

template <float (*J)(PatternList, TransactionList)>
std::pair<Pattern, PatternList> findCore(TransactionList residualDataset,
                                         PatternList patterns,
                                         TransactionList dataset) {
  PatternList extensionList;
  SortedTransactionList sortedDataset = sortDataset(residualDataset);
  auto [firstRowId, firstRow] = sortedDataset[0];
  auto firstRowIter = firstRow.begin();
  Pattern core;

  if (firstRowIter == firstRow.end()) {
    return std::make_pair(core, extensionList);
  }

  core.itemIds.insert(*firstRowIter);
  for (size_t trId = 0; trId < sortedDataset.size(); trId++) {
    auto [originalTrId, trData] = sortedDataset[trId];
    if (trData.count(*firstRowIter) > 0) {
      core.transactionIds.insert(originalTrId);
    }
  }
  firstRowIter++;

  while (firstRowIter != firstRow.end()) {
    Pattern candidate = core;
    candidate.itemIds.insert(*firstRowIter);
    for (size_t trId = 0; trId < sortedDataset.size(); trId++) {
      auto [originalTrId, trData] = sortedDataset[trId];
      if (trData.count(*firstRowIter) == 0) {
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
      extensionList.push_back(candidate);
    }

    firstRowIter++;
  }

  return std::make_pair(core, extensionList);
}

bool notTooNoisy(Pattern core, float maxRowNoise, float maxColumnNoise) {
  return true;
}

template <float (*J)(PatternList, TransactionList)>
Pattern extendCore(Pattern core, PatternList extensionList,
                   PatternList patterns, TransactionList dataset,
                   float maxRowNoise, float maxColumnNoise) {
  // bool addedItem = true;
  // int n = 5;

  // while (addedItem) {
  //   for (int i = 0; i < n; i++) {
  //     // auto tr = sortedD[trId];
  //   }
  //   addedItem = false;
  //   while (!extensionList.empty()) {
  //     auto extensionList.pop_back();

  //     addedItem = true;
  //   }
  // }

  return core;
}

TransactionList buildResidualDataset(TransactionList dataset,
                                     PatternList patterns) {
  TransactionList result;

  for (size_t trId = 0; trId < dataset.size(); trId++) {
    Transaction resultRow;

    for (auto &&i : dataset[trId]) {
      bool foundInPattern = false;
      for (auto &&p : patterns) {
        auto [columns, transactions] = p;
        bool inColumns =
            std::find(columns.begin(), columns.end(), i) != columns.end();
        bool inTransactions =
            std::find(transactions.begin(), transactions.end(), trId) !=
            transactions.end();

        if (inColumns && inTransactions) {
          foundInPattern = true;
          break;
        }
      }

      if (!foundInPattern) {
        resultRow.insert(i);
      }
    }

    result.push_back(resultRow);
  }

  return result;
}

template <float (*J)(PatternList, TransactionList)>
PatternList panda(int maxK, TransactionList dataset, float maxRowNoise,
                  float maxColumnNoise) {
  PatternList patterns;
  TransactionList residualDataset = dataset;

  for (int i = 0; i < maxK; i++) {
    auto [core, extensionList] =
        findCore<J>(residualDataset, patterns, dataset);
    auto newCore = core;
    // auto newCore = extendCore<J>(core, extensionList, patterns, dataset,
    //                           maxRowNoise, maxColumnNoise);

    PatternList patternsWithNewCore = patterns;
    patternsWithNewCore.push_back(newCore);
    if (J(patterns, dataset) < J(patternsWithNewCore, dataset)) {
      // J cannot be improved any more
      break;
    }

    patterns = patternsWithNewCore;
    residualDataset = buildResidualDataset(dataset, patterns);
  }

  return patterns;
}

float testErrorFunction(PatternList patterns, TransactionList dataset) {
  return 0;
}

int main(int argc, char *argv[]) {
  TransactionList dataset = readTransactions(argv[1]);

  std::cout << "Dataset:" << std::endl;
  for (auto &&t : dataset) {
    for (auto &&c : t) {
      std::cout << c << " ";
    }
    std::cout << std::endl;
  }
  std::cout << std::endl;

  auto patterns = panda<testErrorFunction>(8, dataset, 1.0, 1.0);
  std::cout << "Patterns:" << std::endl;
  for (auto &&p : patterns) {
    for (auto &&i : p.itemIds) {
      std::cout << i << " ";
    }
    std::cout << "(" << p.transactionIds.size() << ")";
    std::cout << std::endl;
  }

  return 0;
}
