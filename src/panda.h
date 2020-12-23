#pragma once

#include <algorithm>
#include <chrono>
#include <iostream>
#include <map>
#include <queue>
#include <set>
#include <tuple>
#include <vector>

#include "PatternList.h"
#include "TransactionList.h"

#define BENCH_START(NAME) auto NAME##start = std::chrono::system_clock::now();
#define BENCH_END(NAME)                                               \
  auto NAME##end = std::chrono::system_clock::now();                  \
  std::cout << #NAME << ": "                                          \
            << std::chrono::duration_cast<std::chrono::milliseconds>( \
                   NAME##end - NAME##start)                           \
                   .count()                                           \
            << std::endl;

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

template <typename T>
inline float costFunction(const TransactionList<T> &dataset,
                          const PatternList<T> &patterns,
                          float complexityWeight) {
  size_t falsePositives = 0;
  for (const auto &pattern : patterns.patterns) {
    for (const auto &trId : pattern.transactionIds) {
      for (const auto &trItem : pattern.itemIds) {
        if (!trIncludeItem(dataset.transactions[trId], trItem)) {
          falsePositives++;
        }
      }
    }
  }

  size_t falseNegatives = 0;
  for (size_t trId = 0; trId < dataset.size(); trId++) {
    for (const auto &trItem : dataset.transactions[trId]) {
      bool covered = false;
      for (const auto &pattern : patterns.patterns) {
        if (pattern.hasItem(trItem) && pattern.hasTransaction(trId)) {
          covered = true;
        }
      }
      falseNegatives += covered ? 0 : 1;
    }
  }

  return complexityWeight * (float)patterns.complexity +
         (falsePositives + falseNegatives);
}

/*
 * Core algorithm functions
 */

template <typename T>
std::tuple<Pattern<T>, std::queue<T>> findCore(
    const PatternList<T> &patterns, const TransactionList<T> &dataset,
    const TransactionList<T> &residualDataset, float complexityWeight) {
  std::queue<T> extensionList;
  Pattern<T> core;

  BENCH_START(sortItems);
  const auto sorted = residualDataset.itemsByFreq();
  BENCH_END(sortItems);

  BENCH_START(firstItem);
  auto s1 = sorted[0];
  core.addItem(s1);
  for (size_t i = 0; i < residualDataset.size(); i++) {
    if (trIncludeItem(residualDataset.transactions.at(i), s1)) {
      core.addTransaction(i);
    }
  }
  BENCH_END(firstItem);

  BENCH_START(otherItems);
  for (size_t i = 1; i < sorted.size(); i++) {
    const auto sh = sorted[i];
    Pattern<T> candidate = core;
    candidate.addItem(sh);
    for (const auto &trId : core.transactionIds) {
      if (!trIncludeItem(residualDataset.transactions.at(trId), sh)) {
        candidate.removeTransaction(trId);
      }
    }

    PatternList<T> withCore = patterns;
    withCore.addPattern(core);
    const float coreCost = costFunction(dataset, withCore, complexityWeight);

    PatternList<T> withCandidate = patterns;
    withCandidate.addPattern(candidate);
    const float candidateCost =
        costFunction(dataset, withCandidate, complexityWeight);

    if (candidateCost <= coreCost) {
      core = candidate;
    } else {
      extensionList.push(sh);
    }
  }
  BENCH_END(otherItems);

  return {core, extensionList};
}

template <typename T>
Pattern<T> extendCore(const PatternList<T> &patterns,
                      const TransactionList<T> &dataset, const Pattern<T> &core,
                      std::queue<T> &extensionList, float maxRowNoise,
                      float maxColumnNoise, float complexityWeight) {
  Pattern<T> currentCore = core;

  size_t timeA = 0;
  size_t timeB = 0;
  std::chrono::system_clock::time_point start, end;

  bool addedItem = true;
  while (addedItem) {
    start = std::chrono::system_clock::now();
    for (size_t trId = 0; trId < dataset.size(); trId++) {
      if (!currentCore.hasTransaction(trId)) {
        Pattern<T> candidate = currentCore;
        candidate.addTransaction(trId);

        PatternList<T> withCore = patterns;
        withCore.addPattern(core);
        const float coreCost =
            costFunction(dataset, withCore, complexityWeight);

        PatternList<T> withCandidate = patterns;
        withCandidate.addPattern(candidate);
        const float candidateCost =
            costFunction(dataset, withCandidate, complexityWeight);

        if (candidateCost <= coreCost) {
          currentCore = candidate;
        }
      }
    }
    end = std::chrono::system_clock::now();
    timeA += std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
                 .count();

    addedItem = false;

    start = std::chrono::system_clock::now();
    while (!extensionList.empty()) {
      const auto extension = extensionList.front();
      extensionList.pop();

      Pattern<T> candidate = currentCore;
      candidate.addItem(extension);

      PatternList<T> withCore = patterns;
      withCore.addPattern(core);
      const float coreCost = costFunction(dataset, withCore, complexityWeight);

      PatternList<T> withCandidate = patterns;
      withCandidate.addPattern(candidate);
      const float candidateCost =
          costFunction(dataset, withCandidate, complexityWeight);

      if (candidateCost <= coreCost) {
        currentCore = candidate;
        addedItem = true;
        break;
      }
    }
    end = std::chrono::system_clock::now();
    timeB += std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
                 .count();
  }

  std::cout << "timeA: " << timeA << std::endl;
  std::cout << "timeB: " << timeB << std::endl;

  return currentCore;
}

template <typename T>
PatternList<T> panda(int maxK, const TransactionList<T> &dataset,
                     float maxRowNoise, float maxColumnNoise,
                     float complexityWeight) {
  PatternList<T> patterns(maxK);
  TransactionList<T> residualDataset = dataset;

  size_t findCoreTime = 0;
  size_t extendCoreTime = 0;
  size_t removePatternTime = 0;
  std::chrono::system_clock::time_point start, end;

  float prevCost = INFINITY;

  for (int i = 0; i < maxK; i++) {
    start = std::chrono::system_clock::now();
    auto [core, extensionList] =
        findCore(patterns, dataset, residualDataset, complexityWeight);
    end = std::chrono::system_clock::now();
    findCoreTime +=
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
            .count();

    start = std::chrono::system_clock::now();
    auto extendedCore =
        extendCore(patterns, dataset, core, extensionList, maxRowNoise,
                   maxColumnNoise, complexityWeight);
    end = std::chrono::system_clock::now();
    extendCoreTime +=
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
            .count();

    PatternList<T> withCandidate = patterns;
    withCandidate.addPattern(extendedCore);
    const float candidateCost =
        costFunction(dataset, withCandidate, complexityWeight);

    if (prevCost < candidateCost) {
      // J cannot be improved any more
      break;
    }

    prevCost = candidateCost;
    patterns.addPattern(extendedCore);

    start = std::chrono::system_clock::now();
    residualDataset.removePattern(extendedCore);
    end = std::chrono::system_clock::now();
    removePatternTime +=
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
            .count();

    prevCost = candidateCost;
  }

  std::cout << "findCoreTime: " << findCoreTime << std::endl;
  std::cout << "extendCoreTime: " << extendCoreTime << std::endl;
  std::cout << "removePatternTime: " << removePatternTime << std::endl;

  return patterns;
}
