#pragma once

#include <initializer_list>
#include <set>
#include <vector>

template <typename T = int>
struct Pattern {
  std::set<T> itemIds;
  std::set<size_t> transactionIds;

  // Empty constructor
  Pattern() {}

  Pattern(const std::initializer_list<T> &itemIds,
          const std::initializer_list<size_t> &transactionIds)
      : itemIds(itemIds.begin(), itemIds.end()),
        transactionIds(transactionIds.begin(), transactionIds.end()) {}

  // Copy constructor
  Pattern(const Pattern<T> &other)
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
