#pragma once

#include <algorithm>
#include <iostream>
#include <map>
#include <queue>
#include <set>
#include <utility>
#include <vector>

#include "PatternList.h"
#include "ResultState.h"
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
  return complexityWeight * complexity + (falsePositives + falseNegatives);
}

/*
 * Core algorithm functions
 */

template <typename T>
std::pair<Pattern<T>, std::queue<T>> findCore(ResultState<T> &state,
                                              float complexityWeight) {
  std::queue<T> extensionList;
  Pattern<T> core;

  if (state.residualDataset.size() == 0) {
    return std::make_pair(core, extensionList);
  }

  size_t falseNegatives = state.residualDataset.elCount;

  const auto sorted = state.residualDataset.itemsByFreq();
  auto s1 = sorted[0];
  core.addItem(s1);

  for (size_t i = 0; i < state.residualDataset.size(); i++) {
    if (trIncludeItem(state.residualDataset.transactions[i], s1)) {
      core.addTransaction(i);
      falseNegatives--;
    }
  }

  float currentCost = costFunction(
      state.currentFalsePositives, falseNegatives,
      state.patterns.complexity + core.getComplexity(), complexityWeight);

  for (size_t i = 1; i < sorted.size(); i++) {
    const auto sh = sorted[i];
    size_t falseNegativesCandidate = falseNegatives;

    Pattern<T> candidate(core.itemIds);
    candidate.addItem(sh);
    for (auto &&trId : core.transactionIds) {
      if (trIncludeItem(state.residualDataset.transactions[trId], sh)) {
        candidate.addTransaction(trId);
        falseNegativesCandidate--;
      } else {
        falseNegativesCandidate += candidate.itemIds.size();
      }
    }

    float candidateCost =
        costFunction(state.currentFalsePositives, falseNegativesCandidate,
                     state.patterns.complexity + candidate.getComplexity(),
                     complexityWeight);
    if (candidateCost <= currentCost) {
      core = std::move(candidate);
      currentCost = candidateCost;
      falseNegatives = falseNegativesCandidate;
    } else {
      extensionList.push(sh);
    }
  }

  return std::make_pair(core, extensionList);
}

template <typename T>
Pattern<T> extendCore(ResultState<T> &state, const Pattern<T> &core,
                      std::queue<T> &extensionList, float maxRowNoise,
                      float maxColumnNoise, float complexityWeight) {
  Pattern<T> currentCore = core;

  size_t falsePositives = state.currentFalsePositives;
  size_t falseNegatives = state.residualDataset.elCount;

  float currentCost =
      costFunction(falsePositives, falseNegatives,
                   state.patterns.complexity + currentCore.getComplexity(),
                   complexityWeight);

  bool addedItem = true;
  while (addedItem) {
    const auto uncoveredTransactions =
        currentCore.transactionsUncovered(state.dataset.size());

    for (auto &&trId : uncoveredTransactions) {
      size_t falsePositivesCandidate = falsePositives;
      size_t falseNegativesCandidate = falseNegatives;

      size_t foundItems = 0;
      for (auto &&item : state.dataset.transactions[trId]) {
        if (currentCore.hasItem(item)) {
          foundItems++;
          falseNegatives--;
        }
      }
      falsePositivesCandidate += currentCore.itemIds.size() - foundItems;

      float candidateCost = costFunction(
          falsePositivesCandidate, falseNegativesCandidate,
          state.patterns.complexity + currentCore.getComplexity() + 1,
          complexityWeight);

      if (candidateCost <= currentCost) {
        // std::cout << "ADDED TRANSACTION" << std::endl;
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
        if (trIncludeItem(state.residualDataset.transactions[trId],
                          extension)) {
          falseNegativesCandidate--;
        } else {
          falsePositivesCandidate++;
        }
      }

      float candidateCost = costFunction(
          falsePositivesCandidate, falseNegativesCandidate,
          state.patterns.complexity + currentCore.getComplexity() + 1,
          complexityWeight);
      if (candidateCost <= currentCost) {
        // std::cout << "ADDED ITEM" << std::endl;
        currentCore.addItem(extension);
        currentCost = candidateCost;
        falsePositives = falsePositivesCandidate;
        falseNegatives = falseNegativesCandidate;

        addedItem = true;
        break;
      }
    }
  }

  return currentCore;
}

template <typename T>
ResultState<T> panda(int maxK, const TransactionList<T> &dataset,
                     float maxRowNoise, float maxColumnNoise,
                     float complexityWeight) {
  ResultState<T> state(dataset, PatternList<T>(maxK));

  for (int i = 0; i < maxK; i++) {
    auto [core, extensionList] = findCore(state, complexityWeight);
    core = extendCore(state, core, extensionList, maxRowNoise, maxColumnNoise,
                      complexityWeight);

    if (state.currentCost(complexityWeight) <
        state.tryAddPattern(core, complexityWeight)) {
      // J cannot be improved any more
      break;
    }

    state.addPattern(core);

    if (state.residualDataset.elCount == 0) {
      // No more data to explain
      break;
    }
  }

  return state;
}
