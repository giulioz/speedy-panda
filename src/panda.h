#pragma once

#include <algorithm>
#include <iostream>
#include <map>
#include <queue>
#include <set>
#include <utility>
#include <vector>

#include "PatternList.h"
#include "TransactionList.h"

/*
 * Utility functions
 */

template <typename T>
void sortDataset(TransactionList<T> &dataset) {
  const auto itemsFreqMap = dataset.getItemsFreq();

  for (size_t i = 0; i < dataset.size(); i++) {
    std::vector<T> &items = dataset.transactions[i].items;
    std::sort(items.begin(), items.end(),
              [&itemsFreqMap](const T &a, const T &b) {
                return itemsFreqMap.at(a) > itemsFreqMap.at(b);
              });
  }

  std::sort(dataset.transactions.begin(), dataset.transactions.end(),
            [&itemsFreqMap](const Transaction<T> &a, const Transaction<T> &b) {
              size_t aFreq = 0;
              for (auto &&i : a.items) {
                aFreq += itemsFreqMap.at(i);
              }
              size_t bFreq = 0;
              for (auto &&i : b.items) {
                bFreq += itemsFreqMap.at(i);
              }
              return aFreq > bFreq;
            });
}

template <typename T>
bool notTooNoisy(const TransactionList<T> &dataset, const Pattern<T> &core,
                 float maxRowNoise, float maxColumnNoise) {
  bool ok = true;

  const auto maxColumn = (1 - maxColumnNoise) * core.transactionIds.size();
  for (auto &&j : core.itemIds) {
    int columnSum = 0;
    for (auto &&i : core.transactionIds) {
      columnSum += dataset.transactions[i].includes(j);
    }
    ok &= columnSum >= maxColumn;
  }

  const auto maxRow = (1 - maxRowNoise) * core.itemIds.size();
  for (auto &&i : core.transactionIds) {
    int rowSum = 0;
    for (auto &&j : core.itemIds) {
      rowSum += dataset.transactions[i].includes(j);
    }
    ok &= rowSum >= maxRow;
  }

  return ok;
}

template <typename T>
TransactionList<T> buildResidualDataset(const TransactionList<T> &dataset,
                                        const PatternList<T> &patterns) {
  TransactionList<T> result;

  for (size_t trId = 0; trId < dataset.size(); trId++) {
    std::vector<T> resultRow;

    for (const auto &i : dataset.transactions[trId].items) {
      bool foundInPattern = false;
      for (const auto &p : patterns.patterns) {
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
      result.addTransaction(resultRow);
    }
  }

  return result;
}

/*
 * Core algorithm functions
 */

template <typename T,
          float (*J)(const PatternList<T> &, const TransactionList<T> &)>
std::pair<Pattern<T>, std::queue<T>> findCore(
    TransactionList<T> &residualDataset, const PatternList<T> &patterns,
    const TransactionList<T> &dataset) {
  std::queue<int> extensionList;
  sortDataset(residualDataset);
  Pattern<T> core;

  if (residualDataset.size() == 0 ||
      residualDataset.transactions[0].items.size() == 0) {
    return std::make_pair(core, extensionList);
  }

  const auto firstRow = residualDataset.transactions[0].items;
  auto firstRowIter = firstRow.cbegin();
  auto firstRowEnd = firstRow.cend();

  core.itemIds.insert(*firstRowIter);
  for (size_t i = 0; i < residualDataset.size(); i++) {
    if (residualDataset.transactions[i].includes(*firstRowIter)) {
      core.transactionIds.insert(residualDataset.transactions[i].trId);
    }
  }
  firstRowIter++;

  while (firstRowIter != firstRowEnd) {
    Pattern<T> candidate = core;
    candidate.itemIds.insert(*firstRowIter);
    for (size_t i = 0; i < residualDataset.size(); i++) {
      if (residualDataset.transactions[i].includes(*firstRowIter)) {
        candidate.transactionIds.erase(residualDataset.transactions[i].trId);
      }
    }

    PatternList<T> patternsWithCandidate = patterns;
    patternsWithCandidate.addPattern(candidate);
    PatternList<T> patternsWithCore = patterns;
    patternsWithCore.addPattern(core);
    if (J(patternsWithCandidate, dataset) <= J(patternsWithCore, dataset)) {
      core = candidate;
    } else {
      extensionList.push(*firstRowIter);
    }

    firstRowIter++;
  }

  return std::make_pair(core, extensionList);
}

template <typename T,
          float (*J)(const PatternList<T> &, const TransactionList<T> &)>
Pattern<T> extendCore(Pattern<T> &core, std::queue<int> &extensionList,
                      const PatternList<T> &patterns,
                      const TransactionList<T> &dataset, float maxRowNoise,
                      float maxColumnNoise) {
  bool addedItem = true;

  while (addedItem) {
    for (size_t trId = 0; trId < dataset.size(); trId++) {
      if (core.transactionIds.count(trId) == 0) {
        Pattern<T> candidate = core;
        candidate.transactionIds.insert(trId);

        if (notTooNoisy(dataset, candidate, maxRowNoise, maxColumnNoise)) {
          PatternList<T> patternsWithCandidate = patterns;
          patternsWithCandidate.addPattern(candidate);
          PatternList<T> patternsWithCore = patterns;
          patternsWithCore.addPattern(core);
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
      Pattern<T> candidate = core;
      candidate.itemIds.insert(extension);

      if (notTooNoisy(dataset, candidate, maxRowNoise, maxColumnNoise)) {
        PatternList<T> patternsWithCandidate = patterns;
        patternsWithCandidate.addPattern(candidate);
        PatternList<T> patternsWithCore = patterns;
        patternsWithCore.addPattern(core);
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

template <typename T,
          float (*J)(const PatternList<T> &, const TransactionList<T> &)>
PatternList<T> panda(int maxK, const TransactionList<T> &dataset,
                     float maxRowNoise, float maxColumnNoise) {
  PatternList<T> patterns;
  TransactionList<T> residualDataset = dataset;

  for (int i = 0; i < maxK; i++) {
    auto [core, extensionList] =
        findCore<T, J>(residualDataset, patterns, dataset);
    // auto newCore = core;
    auto newCore = extendCore<T, J>(core, extensionList, patterns, dataset,
                                    maxRowNoise, maxColumnNoise);

    PatternList<T> patternsWithNewCore = patterns;
    patternsWithNewCore.addPattern(newCore);
    if (J(patterns, dataset) < J(patternsWithNewCore, dataset)) {
      // J cannot be improved any more
      break;
    }

    patterns = patternsWithNewCore;
    residualDataset = buildResidualDataset(dataset, patterns);
  }

  return patterns;
}
