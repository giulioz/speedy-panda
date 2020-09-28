#pragma once

#include <set>
#include <vector>

template <typename T = int>
struct Pattern {
  std::set<T> itemIds;
  std::set<size_t> transactionIds;

  int getComplexity() const { return transactionIds.size() + itemIds.size(); }
};

template <typename T = int>
struct PatternList {
  std::vector<Pattern<T>> patterns;
  size_t complexity = 0;

  void addPattern(const Pattern<T> &pattern) {
    patterns.push_back(pattern);
    complexity += pattern.getComplexity();
  }
};
