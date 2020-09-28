#pragma once

#include <algorithm>
#include <iostream>
#include <map>
#include <queue>
#include <set>
#include <utility>
#include <vector>

/*
 * Type alias declarations
 */

using Transaction = std::set<int>;
using TransactionList = std::vector<Transaction>;
using SortedTransactionList = std::vector<std::pair<size_t, std::vector<int>>>;
struct Pattern {
  std::set<int> itemIds;
  std::set<int> transactionIds;
};
using PatternList = std::vector<Pattern>;

/*
 * Utility functions declarations
 */

SortedTransactionList setToVectorDataset(const TransactionList &dataset);
SortedTransactionList sortDataset(const SortedTransactionList &dataset);
bool notTooNoisy(const TransactionList &dataset, const Pattern &core,
                 float maxRowNoise, float maxColumnNoise);
SortedTransactionList buildResidualDataset(const TransactionList &dataset,
                                           const PatternList &patterns);

/*
 * Core algorithm functions
 */

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
      break;
    }

    patterns = patternsWithNewCore;
    residualDataset = buildResidualDataset(dataset, patterns);
  }

  return patterns;
}
