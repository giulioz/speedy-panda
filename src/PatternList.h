#pragma once

#include <set>
#include <vector>

template <typename T = int>
struct Pattern {
  std::set<T> itemIds;
  std::set<size_t> transactionIds;

  // Empty constructor
  Pattern() {}

  // Copy constructor
  Pattern(const Pattern &other)
      : itemIds(other.itemIds), transactionIds(other.transactionIds) {}

  size_t getComplexity() const {
    return itemIds.size() + transactionIds.size();
  }
};

template <typename T = int>
struct PatternList {
  std::vector<Pattern<T>> patterns;
  size_t complexity = 0;

  // Empty constructor
  PatternList() {}

  // Copy constructor
  PatternList(const PatternList &other)
      : patterns(other.patterns), complexity(other.complexity) {}

  void addPattern(const Pattern<T> &pattern) {
    patterns.push_back(pattern);
    complexity += pattern.getComplexity();
  }
};
