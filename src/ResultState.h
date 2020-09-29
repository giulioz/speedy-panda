#pragma once

#include <algorithm>
#include <initializer_list>
#include <list>
#include <map>
#include <vector>

#include "PatternList.h"
#include "TransactionList.h"

template <typename T = int>
struct ResultState {
  TransactionList<T> dataset;
  PatternList<T> patterns;
  TransactionList<T> residualDataset;

  size_t currentFalsePositives = 0;

  ResultState(const TransactionList<T> &dataset,
              const PatternList<T> &patterns = PatternList<T>())
      : dataset(dataset),
        patterns(patterns),
        residualDataset(dataset),
        currentFalsePositives(0) {}

  // Copy constructor
  ResultState(const ResultState<T> &other)
      : dataset(other.dataset),
        patterns(other.patterns),
        residualDataset(other.residualDataset),
        currentFalsePositives(other.currentFalsePositives) {}

  void sortResidualDataset() { residualDataset.sortByFreq(); }

  inline size_t currentNoise() const {
    return residualDataset.elCount + currentFalsePositives;
  }

  inline float currentCost(const float complexityWeight = 0.5) const {
    return complexityWeight * patterns.complexity + currentNoise();
  }

  void addPattern(const Pattern<T> &pattern) {
    patterns.addPattern(pattern);

    currentFalsePositives += dataset.calcPatternFalsePositives(pattern);
    residualDataset.removePattern(pattern);
  }

  float tryAddPattern(const Pattern<T> &pattern,
                      const float complexityWeight = 0.5) const {
    ResultState<T> state2(*this);
    state2.addPattern(pattern);
    return state2.currentCost(complexityWeight);
  }
};
