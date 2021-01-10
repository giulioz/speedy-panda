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

  if (maxColumnNoise < 1.0) {
    const auto maxColumn = (1 - maxColumnNoise) * core.transactionIds.size();
    for (auto &&j : core.itemIds) {
      int columnSum = 0;
      for (auto &&i : core.transactionIds) {
        columnSum += trIncludeItem(dataset.transactions[i], j);
      }
      ok &= columnSum >= maxColumn;
    }
  }

  if (maxRowNoise < 1.0) {
    const auto maxRow = (1 - maxRowNoise) * core.itemIds.size();
    for (auto &&i : core.transactionIds) {
      int rowSum = 0;
      for (auto &&j : core.itemIds) {
        rowSum += trIncludeItem(dataset.transactions[i], j);
      }
      ok &= rowSum >= maxRow;
    }
  }

  return ok;
}

template <typename T>
bool notTooNoisyItem(const TransactionList<T> &dataset, const Pattern<T> &core,
                     float maxRowNoise, float maxColumnNoise, const T &item, int &prevColSum) {
  bool ok = true;

  if (maxColumnNoise < 1.0) {
    const auto maxColumn = (1 - maxColumnNoise) * core.transactionIds.size();
    int columnSum = 0;
    for (auto &&i : core.transactionIds) {
      columnSum += trIncludeItem(dataset.transactions[i], item);
    }
    ok &= columnSum >= maxColumn;
  }

  if (maxRowNoise < 1.0) {
    const auto maxRow = (1 - maxRowNoise) * core.itemIds.size();
    for (auto &&i : core.transactionIds) {
      int rowSum = 0;
      for (auto &&j : core.itemIds) {
        rowSum += trIncludeItem(dataset.transactions[i], j);
      }
      rowSum += trIncludeItem(dataset.transactions[i], item);
      ok &= rowSum >= maxRow;
    }
  }

  return ok;
}

template <typename T>
bool notTooNoisyTransaction(const TransactionList<T> &dataset,
                            const Pattern<T> &core, float maxRowNoise,
                            float maxColumnNoise, size_t transaction) {
  bool ok = true;

  if (maxColumnNoise < 1.0) {
    const auto maxColumn = (1 - maxColumnNoise) * core.transactionIds.size();
    for (auto &&j : core.itemIds) {
      int columnSum = 0;
      for (auto &&i : core.transactionIds) {
        columnSum += trIncludeItem(dataset.transactions[i], j);
      }
      columnSum += trIncludeItem(dataset.transactions[transaction], j);
      ok &= columnSum >= maxColumn;
    }
  }

  if (maxRowNoise < 1.0) {
    const auto maxRow = (1 - maxRowNoise) * core.itemIds.size();
    int rowSum = 0;
    for (auto &&j : core.itemIds) {
      rowSum += trIncludeItem(dataset.transactions[transaction], j);
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

  if (residualDataset.elCount == 0) {
    return {core, extensionList, 0};
  }

  BENCH_START(sortItems);
  const auto sorted = residualDataset.itemsByFreq();
  BENCH_END(sortItems);
  BENCH_START(firstItem);
  auto s1 = sorted[0];
  core.addItem(s1);

#pragma omp parallel
  {
    std::vector<size_t> included;
    included.reserve(residualDataset.size());
#pragma omp for nowait
    for (size_t i = 0; i < residualDataset.size(); i++) {
      if (trIncludeItem(residualDataset.transactions.at(i), s1)) {
        included.push_back(i);
      }
    }

#pragma omp critical
    { core.addTransactions(included); }
  }

  BENCH_END(firstItem);

  auto falseNegatives = residualDataset.elCount - core.getSize();
  float currentCost = costFunction(currentFalsePositives, falseNegatives,
                                   patterns.complexity + core.getComplexity(),
                                   complexityWeight);

  BENCH_START(otherItems);
  for (size_t i = 1; i < sorted.size(); i++) {
    const auto sh = sorted[i];
    Pattern<T> candidate(core.itemIds);
    candidate.addItem(sh);

    std::vector<size_t> copied(core.transactionIds.cbegin(),
                               core.transactionIds.cend());

#pragma omp parallel
    {
      std::vector<size_t> included;
      included.reserve(copied.size());
#pragma omp for nowait
      for (size_t i = 0; i < copied.size(); i++) {
        if (trIncludeItem(residualDataset.transactions.at(copied[i]), sh)) {
          included.push_back(copied[i]);
        }
      }

#pragma omp critical
      { candidate.addTransactions(included); }
    }

    const auto falseNegativesCandidate =
        residualDataset.elCount - candidate.getSize();
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
  BENCH_END(otherItems);

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

  size_t timeA = 0;
  size_t timeB = 0;
  std::chrono::system_clock::time_point start, end;

  bool addedItem = true;
  while (addedItem) {
    start = std::chrono::system_clock::now();
    for (size_t trId = 0; trId < dataset.size(); trId++) {
      if (!currentCore.hasTransaction(trId)) {
        size_t falsePositivesCandidate = falsePositives;
        size_t falseNegativesCandidate = falseNegatives;

        std::vector<T> copied(currentCore.itemIds.cbegin(),
                              currentCore.itemIds.cend());

#pragma omp parallel for reduction(+:falsePositivesCandidate) reduction(-:falseNegativesCandidate)
        for (size_t i = 0; i < copied.size(); i++) {
          const auto item = copied[i];
          bool covered = patterns.covers(trId, item);
          bool on = trIncludeItem(dataset.transactions[trId], item);
          if (!on && !covered) {
            falsePositivesCandidate++;
          } else if (!covered) {
            falseNegativesCandidate--;
          }
        }

        float candidateCost =
            costFunction(falsePositivesCandidate, falseNegativesCandidate,
                         patterns.complexity + currentCore.getComplexity() + 1,
                         complexityWeight);

        if (candidateCost <= currentCost &&
            notTooNoisyTransaction(dataset, currentCore, maxRowNoise,
                                   maxColumnNoise, trId)) {
          currentCore.addTransaction(trId);
          currentCost = candidateCost;
          falsePositives = falsePositivesCandidate;
          falseNegatives = falseNegativesCandidate;
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

      size_t falsePositivesCandidate = falsePositives;
      size_t falseNegativesCandidate = falseNegatives;

      std::vector<size_t> copied(currentCore.transactionIds.cbegin(),
                                 currentCore.transactionIds.cend());

#pragma omp parallel for reduction(+:falsePositivesCandidate) reduction(-:falseNegativesCandidate)
      for (size_t i = 0; i < copied.size(); i++) {
        const auto trId = copied[i];
        const bool covered = patterns.covers(trId, extension);
        const bool on = trIncludeItem(dataset.transactions[trId], extension);
        if (!on && !covered) {
          falsePositivesCandidate++;
        } else if (!covered) {
          falseNegativesCandidate--;
        }
      }

      float candidateCost =
          costFunction(falsePositivesCandidate, falseNegativesCandidate,
                       patterns.complexity + currentCore.getComplexity() + 1,
                       complexityWeight);
      if (candidateCost <= currentCost &&
          notTooNoisyItem(dataset, currentCore, maxRowNoise, maxColumnNoise,
                          extension)) {
        currentCore.addItem(extension);
        currentCost = candidateCost;
        falsePositives = falsePositivesCandidate;
        falseNegatives = falseNegativesCandidate;

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

  size_t findCoreTime = 0;
  size_t extendCoreTime = 0;
  size_t removePatternTime = 0;
  std::chrono::system_clock::time_point start, end;

  for (int i = 0; i < maxK; i++) {
    start = std::chrono::system_clock::now();
    auto [core, extensionList, resultFalseNegatives] =
        findCore(patterns, residualDataset, falsePositives, complexityWeight);
    end = std::chrono::system_clock::now();
    findCoreTime +=
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
            .count();

    falseNegatives = resultFalseNegatives;

    start = std::chrono::system_clock::now();
    auto [extendedCore, resultFalsePositives2, resultFalseNegatives2] =
        extendCore(patterns, dataset, core, extensionList, falseNegatives,
                   falsePositives, maxRowNoise, maxColumnNoise,
                   complexityWeight);
    end = std::chrono::system_clock::now();
    extendCoreTime +=
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
            .count();

    falsePositives = resultFalsePositives2;
    falseNegatives = resultFalseNegatives2;
    float candidateCost = costFunction(falsePositives, falseNegatives,
                                       patterns.complexity, complexityWeight);
    if (prevCost < candidateCost) {
      // J cannot be improved any more
      break;
    }

    patterns.addPattern(extendedCore);

    start = std::chrono::system_clock::now();
    residualDataset.removePattern(extendedCore);
    end = std::chrono::system_clock::now();
    removePatternTime +=
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
            .count();

    prevCost = candidateCost;

    if (residualDataset.elCount == 0) {
      // No more data to explain
      break;
    }
  }

  std::cout << "findCoreTime: " << findCoreTime << std::endl;
  std::cout << "extendCoreTime: " << extendCoreTime << std::endl;
  std::cout << "removePatternTime: " << removePatternTime << std::endl;

  return patterns;
}
