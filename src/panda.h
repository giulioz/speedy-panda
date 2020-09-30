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
  // return true;
}

/*
 * Core algorithm functions
 */

template <typename T>
std::pair<Pattern<T>, std::queue<T>> findCore(ResultState<T> &state) {
  std::queue<T> extensionList;
  state.sortResidualDataset();
  Pattern<T> core;

  const auto firstRow = state.residualDataset.transactions[0].items;
  auto firstRowIter = firstRow.cbegin();
  auto firstRowEnd = firstRow.cend();
  auto s1 = *firstRowIter;

  core.addItem(s1);
  for (size_t i = 0; i < state.residualDataset.size(); i++) {
    if (state.residualDataset.transactions[i].includes(s1)) {
      core.addTransaction(state.residualDataset.transactions[i].trId);
    }
  }
  firstRowIter++;

  float currentCost = state.tryAddPattern(core);

  while (firstRowIter != firstRowEnd) {
    Pattern<T> candidate = core;
    auto sh = *firstRowIter;
    candidate.addItem(sh);
    for (size_t i = 0; i < state.residualDataset.size(); i++) {
      if (!state.residualDataset.transactions[i].includes(sh)) {
        candidate.removeTransaction(state.residualDataset.transactions[i].trId);
      }
    }

    float candidateCost = state.tryAddPattern(candidate);
    if (candidateCost <= currentCost) {
      core = candidate;
      currentCost = candidateCost;
    } else {
      extensionList.push(sh);
    }

    firstRowIter++;
  }

  return std::make_pair(core, extensionList);
}

template <typename T>
Pattern<T> extendCore(ResultState<T> &state, Pattern<T> &core,
                      std::queue<T> &extensionList, float maxRowNoise,
                      float maxColumnNoise) {
  bool addedItem = true;

  while (addedItem) {
    float currentCost = state.tryAddPattern(core);

    for (size_t trId = 0; trId < state.dataset.size(); trId++) {
      if (core.hasTransaction(trId) == 0) {
        Pattern<T> candidate = core;
        candidate.addTransaction(trId);

        if (notTooNoisy(state.dataset, candidate, maxRowNoise,
                        maxColumnNoise)) {
          float candidateCost = state.tryAddPattern(candidate);
          if (candidateCost <= currentCost) {
            core = candidate;
            currentCost = candidateCost;
          }
        }
      }
    }

    addedItem = false;
    while (!extensionList.empty()) {
      auto extension = extensionList.front();
      extensionList.pop();
      Pattern<T> candidate = core;
      candidate.addItem(extension);

      if (notTooNoisy(state.dataset, candidate, maxRowNoise, maxColumnNoise)) {
        float candidateCost = state.tryAddPattern(candidate);
        if (candidateCost <= currentCost) {
          core = candidate;
          currentCost = candidateCost;
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
  ResultState<T> state(dataset, PatternList<T>(maxK));

  for (int i = 0; i < maxK; i++) {
    auto [core, extensionList] = findCore(state);
    core = extendCore(state, core, extensionList, maxRowNoise, maxColumnNoise);

    if (state.currentCost() < state.tryAddPattern(core)) {
      // J cannot be improved any more
      break;
    }

    state.addPattern(core);

    if (state.residualDataset.elCount == 0) {
      // No more data to explain
      break;
    }
  }

  return state.patterns;
}
