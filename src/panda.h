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
size_t calcNoise(const PatternList<T> &patterns,
                 const TransactionList<T> &dataset) {
  size_t noise = 0;
  for (size_t trId = 0; trId < dataset.size(); trId++) {
    std::set<T> falsePositives;
    std::set<T> falseNegatives(dataset.transactions[trId].items.cbegin(),
                               dataset.transactions[trId].items.cend());

    for (auto &&pattern : patterns.patterns) {
      if (pattern.transactionIds.count(trId) > 0) {
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

template <typename T>
float costFunction(const TransactionList<T> &dataset,
                   const PatternList<T> &patterns) {
  // Noise from patterns vs dataset (Ja)
  size_t noise = calcNoise(patterns, dataset);

  // Pattern set complexity (Jh)
  size_t complexity = patterns.complexity;

  // Weight the two measures (Jp)
  return 0.5 * complexity + noise;
}

template <typename T>
float costFunctionWithPattern(const TransactionList<T> &dataset,
                              const PatternList<T> &patterns,
                              const Pattern<T> &pattern) {
  PatternList<T> p2 = patterns;
  p2.addPattern(pattern);

  // Noise from patterns vs dataset (Ja)
  size_t noise = calcNoise(p2, dataset);

  // Pattern set complexity (Jh)
  size_t complexity = p2.complexity;

  // Weight the two measures (Jp)
  return 0.5 * complexity + noise;
}

/*
 * Core algorithm functions
 */

template <typename T>
std::pair<Pattern<T>, std::queue<T>> findCore(
    TransactionList<T> &residualDataset, const PatternList<T> &patterns,
    float patternsCost, const TransactionList<T> &dataset) {
  std::queue<int> extensionList;
  residualDataset.sortByFreq();
  Pattern<T> core;

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

  float coreCost =
      costFunctionWithPattern(dataset, patterns, core);

  while (firstRowIter != firstRowEnd) {
    Pattern<T> candidate = core;
    candidate.itemIds.insert(*firstRowIter);
    for (size_t i = 0; i < residualDataset.size(); i++) {
      if (residualDataset.transactions[i].includes(*firstRowIter)) {
        candidate.transactionIds.erase(residualDataset.transactions[i].trId);
      }
    }

    float candidateCost =
        costFunctionWithPattern(dataset, patterns, candidate);

    if (candidateCost <= coreCost) {
      core = candidate;
      coreCost = candidateCost;
    } else {
      extensionList.push(*firstRowIter);
    }

    firstRowIter++;
  }

  return std::make_pair(core, extensionList);
}

template <typename T>
Pattern<T> extendCore(Pattern<T> &core, std::queue<int> &extensionList,
                      const PatternList<T> &patterns, float patternsCost,
                      const TransactionList<T> &dataset, float maxRowNoise,
                      float maxColumnNoise) {
  bool addedItem = true;

  float coreCost =
      costFunctionWithPattern(dataset, patterns, core);

  while (addedItem) {
    for (size_t trId = 0; trId < dataset.size(); trId++) {
      if (core.transactionIds.count(trId) == 0) {
        Pattern<T> candidate = core;
        candidate.transactionIds.insert(trId);

        if (notTooNoisy(dataset, candidate, maxRowNoise, maxColumnNoise)) {
          float candidateCost = costFunctionWithPattern(
              dataset, patterns, candidate);

          if (candidateCost <= coreCost) {
            core = candidate;
            coreCost = candidateCost;
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
        float candidateCost =
            costFunctionWithPattern(dataset, patterns, candidate);

        if (candidateCost <= coreCost) {
          core = candidate;
          coreCost = candidateCost;
          addedItem = true;
          break;
        }
      }
    }
  }

  return core;
}

template <typename T>
PatternList<T> panda(int maxK, const TransactionList<T> &dataset,
                     float maxRowNoise, float maxColumnNoise) {
  PatternList<T> patterns;
  TransactionList<T> residualDataset = dataset;

  for (int i = 0; i < maxK; i++) {
    float patternsCost = costFunction(dataset, patterns);

    auto [core, extensionList] =
        findCore(residualDataset, patterns, patternsCost, dataset);
    core = extendCore(core, extensionList, patterns, patternsCost, dataset,
                      maxRowNoise, maxColumnNoise);

    if (patternsCost < costFunctionWithPattern(dataset, patterns, core)) {
      // J cannot be improved any more
      break;
    }

    patterns.addPattern(core);
    residualDataset.removePattern(core);

    if (residualDataset.elCount == 0) {
      break;
    }
  }

  return patterns;
}
