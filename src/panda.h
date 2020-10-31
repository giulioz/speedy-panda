#pragma once

#include <algorithm>
#include <iostream>
#include <map>
#include <queue>
#include <set>
#include <tuple>
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
      columnSum += trIncludeItem(dataset.transactions[i], j);
    }
    ok &= columnSum >= maxColumn;
  }

  const auto maxRow = (1 - maxRowNoise) * core.itemIds.size();
  for (auto &&i : core.transactionIds) {
    int rowSum = 0;
    for (auto &&j : core.itemIds) {
      rowSum += trIncludeItem(dataset.transactions[i], j);
    }
    ok &= rowSum >= maxRow;
  }

  return ok;
}

inline float costFunction(size_t falsePositives, size_t falseNegatives,
                          size_t complexity, float complexityWeight) {
  return complexityWeight * (float)complexity +
         (falsePositives + falseNegatives);
}

/*
 * Core algorithm functions
 */

template <typename T>
std::tuple<Pattern<T>, std::queue<T>, size_t> findCore(
    const PatternList<T> &patterns, const TransactionList<T> &residualDataset,
    size_t currentFalsePositives, float complexityWeight) {
  std::queue<T> extensionList;
  Pattern<T> core;

  size_t falseNegatives = residualDataset.elCount;

  if (residualDataset.size() == 0) {
    return {core, extensionList, falseNegatives};
  }

  const auto sorted = residualDataset.itemsByFreq();
  auto s1 = sorted[0];
  core.addItem(s1);

  for (size_t i = 0; i < residualDataset.size(); i++) {
    if (trIncludeItem(residualDataset.transactions[i], s1)) {
      core.addTransaction(i);
      falseNegatives--;
    }
  }

  float currentCost = costFunction(currentFalsePositives, falseNegatives,
                                   patterns.complexity + core.getComplexity(),
                                   complexityWeight);

  for (size_t i = 1; i < sorted.size(); i++) {
    const auto sh = sorted[i];
    size_t falseNegativesCandidate = falseNegatives;

    Pattern<T> candidate(core.itemIds);
    candidate.addItem(sh);
    for (auto &&trId : core.transactionIds) {
      if (trIncludeItem(residualDataset.transactions[trId], sh)) {
        candidate.addTransaction(trId);
        falseNegativesCandidate--;
      } else {
        falseNegativesCandidate += core.itemIds.size();
      }
    }

    float candidateCost = costFunction(
        currentFalsePositives, falseNegativesCandidate,
        patterns.complexity + candidate.getComplexity(), complexityWeight);
    if (candidateCost <= currentCost) {
      core = std::move(candidate);
      currentCost = candidateCost;
      falseNegatives = falseNegativesCandidate;
    } else {
      extensionList.push(sh);
    }
  }

  return {core, extensionList, falseNegatives};
}

template <typename T>
std::tuple<Pattern<T>, size_t, size_t> extendCore(
    const PatternList<T> &patterns, const TransactionList<T> &dataset,
    const Pattern<T> &core, std::queue<T> &extensionList,
    size_t currentFalseNegatives, size_t currentFalsePositives,
    float maxRowNoise, float maxColumnNoise, float complexityWeight) {
  Pattern<T> currentCore = core;

  size_t falsePositives = currentFalsePositives;
  size_t falseNegatives = currentFalseNegatives;

  float currentCost = costFunction(
      falsePositives, falseNegatives,
      patterns.complexity + currentCore.getComplexity(), complexityWeight);

  bool addedItem = true;
  while (addedItem) {
    const auto uncoveredTransactions =
        currentCore.transactionsUncovered(dataset.size());

    for (auto &&trId : uncoveredTransactions) {
      size_t falsePositivesCandidate = falsePositives;
      size_t falseNegativesCandidate = falseNegatives;
      for (auto &&item : currentCore.itemIds) {
        bool covered = patterns.covers(trId, item);
        // bool covered = !trIncludeItem(residualDataset.transactions[trId],
        // item);
        bool on = trIncludeItem(dataset.transactions[trId], item);
        if (!on) {
          falsePositivesCandidate++;
        } else if (!covered) {
          falseNegativesCandidate--;
        }
      }

      float candidateCost =
          costFunction(falsePositivesCandidate, falseNegativesCandidate,
                       patterns.complexity + currentCore.getComplexity() + 1,
                       complexityWeight);

      if (candidateCost <= currentCost) {
        currentCore.addTransaction(trId);
        currentCost = candidateCost;
        falsePositives = falsePositivesCandidate;
        falseNegatives = falseNegativesCandidate;
      }
    }

    addedItem = false;

    while (!extensionList.empty()) {
      const auto extension = extensionList.front();
      extensionList.pop();

      size_t falsePositivesCandidate = falsePositives;
      size_t falseNegativesCandidate = falseNegatives;

      for (auto &&trId : currentCore.transactionIds) {
        bool covered = patterns.covers(trId, extension);
        // bool covered =
        //     !trIncludeItem(residualDataset.transactions[trId], extension);
        bool on = trIncludeItem(dataset.transactions[trId], extension);
        if (!on) {
          falsePositivesCandidate++;
        } else if (!covered) {
          falseNegativesCandidate--;
        }
      }

      float candidateCost =
          costFunction(falsePositivesCandidate, falseNegativesCandidate,
                       patterns.complexity + currentCore.getComplexity() + 1,
                       complexityWeight);
      if (candidateCost <= currentCost) {
        currentCore.addItem(extension);
        currentCost = candidateCost;
        falsePositives = falsePositivesCandidate;
        falseNegatives = falseNegativesCandidate;

        addedItem = true;
        break;
      }
    }
  }

  return {currentCore, falsePositives, falseNegatives};
}

template <typename T>
PatternList<T> panda(int maxK, const TransactionList<T> &dataset,
                     float maxRowNoise, float maxColumnNoise,
                     float complexityWeight) {
  PatternList<T> patterns(maxK);
  TransactionList<T> residualDataset = dataset;

  size_t falsePositives = 0;
  size_t falseNegatives = residualDataset.elCount;

  float prevCost = costFunction(falsePositives, falseNegatives,
                                patterns.complexity, complexityWeight);

  for (int i = 0; i < maxK; i++) {
    auto [core, extensionList, resultFalseNegatives] =
        findCore(patterns, residualDataset, falsePositives, complexityWeight);
    falseNegatives = resultFalseNegatives;

    auto [extendedCore, resultFalsePositives2, resultFalseNegatives2] =
        extendCore(patterns, dataset, core, extensionList, falseNegatives,
                   falsePositives, maxRowNoise, maxColumnNoise,
                   complexityWeight);
    falsePositives = resultFalsePositives2;
    falseNegatives = resultFalseNegatives2;

    float candidateCost = costFunction(falsePositives, falseNegatives,
                                       patterns.complexity, complexityWeight);

    if (prevCost < candidateCost) {
      // J cannot be improved any more
      break;
    }

    patterns.addPattern(extendedCore);
    residualDataset.removePattern(extendedCore);
    prevCost = candidateCost;

    if (residualDataset.elCount == 0) {
      // No more data to explain
      break;
    }
  }

  return patterns;
}
