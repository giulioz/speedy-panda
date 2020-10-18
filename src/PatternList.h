#pragma once

#include <initializer_list>
#include <unordered_set>
#include <vector>

template <typename T = int>
struct Pattern {
  std::unordered_set<T> itemIds;
  std::unordered_set<size_t> transactionIds;

  // Empty constructor
  Pattern() {}

  Pattern(const std::initializer_list<T> &itemIds,
          const std::initializer_list<size_t> &transactionIds)
      : itemIds(itemIds.begin(), itemIds.end()),
        transactionIds(transactionIds.begin(), transactionIds.end()) {}

  // Copy constructor
  Pattern(const Pattern<T> &other)
      : itemIds(other.itemIds), transactionIds(other.transactionIds) {}

  inline size_t getComplexity() const {
    return itemIds.size() + transactionIds.size();
  }

  inline void addItem(const T item) { itemIds.insert(item); }
  inline void addTransaction(const size_t transaction) {
    transactionIds.insert(transaction);
  }

  inline void removeItem(const T item) { itemIds.erase(item); }
  inline void removeTransaction(const size_t transaction) {
    transactionIds.erase(transaction);
  }

  inline bool hasItem(const T item) const { return itemIds.count(item) > 0; }
  inline bool hasTransaction(const size_t transaction) const {
    return transactionIds.count(transaction) > 0;
  }
};

template <typename T = int>
struct PatternList {
  std::vector<Pattern<T>> patterns;
  size_t complexity = 0;

  // Empty constructor
  PatternList() {}

  // Reserve size
  PatternList(size_t size) { patterns.reserve(size); }

  // Copy constructor
  PatternList(const PatternList &other)
      : patterns(other.patterns), complexity(other.complexity) {}

  void addPattern(const Pattern<T> &pattern) {
    patterns.push_back(pattern);
    complexity += pattern.getComplexity();
  }
};
