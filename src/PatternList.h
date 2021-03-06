#pragma once

#include <algorithm>
#include <initializer_list>
#include <unordered_set>
#include <vector>

template <typename T = int>
struct Pattern {
  std::vector<T> itemIds;
  // std::vector<size_t> transactionIds;
  std::unordered_set<size_t> transactionIds;

  // Empty constructor
  Pattern() {}

  Pattern(const std::vector<T> &itemIds)
      : itemIds(itemIds.begin(), itemIds.end()) {}

  Pattern(const std::vector<T> &itemIds,
          const std::unordered_set<size_t> &transactionIds)
      : itemIds(itemIds.begin(), itemIds.end()),
        transactionIds(transactionIds.begin(), transactionIds.end()) {}

  Pattern(const std::initializer_list<T> &itemIds,
          const std::initializer_list<size_t> &transactionIds)
      : itemIds(itemIds.begin(), itemIds.end()),
        transactionIds(transactionIds.begin(), transactionIds.end()) {}

  // Copy constructor
  Pattern(const Pattern<T> &other)
      : itemIds(other.itemIds), transactionIds(other.transactionIds) {}

  // Move constructor
  Pattern(const Pattern<T> &&other)
      : itemIds(std::move(other.itemIds)),
        transactionIds(std::move(other.transactionIds)) {}

  // Copy operator
  Pattern &operator=(const Pattern<T> &other) {
    itemIds = other.itemIds;
    transactionIds = other.transactionIds;
    return *this;
  }

  // Move operator
  Pattern &operator=(const Pattern<T> &&other) {
    itemIds = std::move(other.itemIds);
    transactionIds = std::move(other.transactionIds);
    return *this;
  }

  inline size_t getComplexity() const {
    return itemIds.size() + transactionIds.size();
  }

  inline size_t getSize() const {
    return itemIds.size() * transactionIds.size();
  }

  inline void addItem(const T item) {
    // itemIds.insert(item);
    itemIds.push_back(item);
  }
  inline void addTransaction(const size_t transaction) {
    transactionIds.insert(transaction);
    // transactionIds.push_back(transaction);
  }

  template <typename E>
  void addTransactions(const E &transactions) {
    transactionIds.insert(transactions.cbegin(), transactions.cend());
    // transactionIds.insert(transactionIds.end(), transactions.cbegin(),
    //                       transactions.cend());
  }

  inline bool hasItem(const T item) const {
    // return itemIds.count(item) > 0;
    return std::find(itemIds.cbegin(), itemIds.cend(), item) != itemIds.cend();
  }
  inline bool hasTransaction(const size_t transaction) const {
    return transactionIds.count(transaction) > 0;
    // return std::find(transactionIds.cbegin(), transactionIds.cend(),
    //                  transaction) != transactionIds.cend();
  }

  inline bool covers(const size_t trId, const T &item) const {
    return hasTransaction(trId) && hasItem(item);
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

  bool covers(const size_t trId, const T &item) const {
    for (auto &&pat : patterns) {
      if (pat.covers(trId, item)) {
        return true;
      }
    }
    return false;
  }
};
