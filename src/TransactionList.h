#pragma once

#include <algorithm>
#include <initializer_list>
#include <list>
#include <unordered_map>
#include <vector>

#include "PatternList.h"

template <typename TK, typename TV>
std::vector<TK> extractKeys(const std::unordered_map<TK, TV> &input_map) {
  std::vector<TK> retval;
  for (auto const &element : input_map) {
    retval.push_back(element.first);
  }
  return retval;
}

template <typename T = int>
using Transaction = std::vector<T>;

template <typename T = int>
inline bool trIncludeItem(const Transaction<T> &items, const T &val) {
  // if (items.empty()) {
  //   return false;
  // }

  // Supposing an ordered vector
  return std::binary_search(items.cbegin(), items.cend(), val);

  // When using vector
  // return std::find(items.cbegin(), items.cend(), val) != items.cend();

  // When using set
  // return items.count(val) != 0;
}

template <typename T = int>
struct TransactionList {
  std::vector<Transaction<T>> transactions;
  size_t elCount = 0;

  // Empty constructor
  TransactionList() {}

  // Copy constructor
  TransactionList(const TransactionList<T> &other)
      : transactions(other.transactions), elCount(other.elCount) {}

  void addTransaction(const std::initializer_list<T> &elements) {
    addTransaction(Transaction<T>(elements));
  }

  template <typename C>
  void addTransaction(const C &elements) {
    addTransaction(Transaction<T>(elements.begin(), elements.end()));
  }

  void addTransaction(Transaction<T> transaction) {
    std::stable_sort(transaction.begin(), transaction.end());
    transactions.push_back(transaction);
    elCount += transaction.size();
  }

  void addTransactionSorted(const Transaction<T> &transaction) {
    transactions.push_back(transaction);
    elCount += transaction.size();
  }

  inline size_t size() const { return transactions.size(); }

  // Removes a pattern footprint from transactions (for residual dataset)
  void removePattern(const Pattern<T> &pattern) {
    for (auto &&trId : pattern.transactionIds) {
      std::list<T> resultRow;

      for (const auto &trItem : transactions[trId]) {
        // if is not in pattern items we keep it
        if (pattern.hasItem(trItem) == 0) {
          resultRow.push_back(trItem);
        }
      }

      const int diff = transactions[trId].size() - resultRow.size();
      transactions[trId] = Transaction<T>(resultRow.begin(), resultRow.end());
      elCount -= diff;
    }
  }

  // Returns the number of elements outside the given pattern
  size_t tryRemovePattern(const Pattern<T> &pattern) const {
    size_t count = elCount;

    for (auto &&trId : pattern.transactionIds) {
      size_t found = 0;

      for (const auto &trItem : transactions[trId]) {
        // if is not in pattern items we keep it
        if (pattern.hasItem(trItem) == 0) {
          found++;
        }
      }

      const int diff = transactions[trId].size() - found;
      count -= diff;
    }

    return count;
  }

  // Calculate the number of false positives given a pattern in the dataset
  size_t calcPatternFalsePositives(const Pattern<T> &pattern) const {
    size_t falsePositives = 0;

    for (auto &&trId : pattern.transactionIds) {
      for (auto &&itemId : pattern.itemIds) {
        if (!trIncludeItem(transactions[trId], itemId)) {
          falsePositives++;
        }
      }
    }

    return falsePositives;
  }

  // Build a frequency map for every item
  std::unordered_map<T, size_t> getItemsFreq() const {
    std::unordered_map<T, size_t> itemsFreqMap;
    for (size_t trId = 0; trId < transactions.size(); trId++) {
      for (auto &&i : transactions[trId]) {
        if (itemsFreqMap.count(i) == 0) {
          itemsFreqMap[i] = 1;
        } else {
          itemsFreqMap[i]++;
        }
      }
    }

    return itemsFreqMap;
  }

  // By frequency sorting strategy for greedy method
  std::vector<T> itemsByFreq() const {
    const auto itemsFreqMap = getItemsFreq();
    auto keys = extractKeys(itemsFreqMap);

    std::stable_sort(keys.begin(), keys.end(),
                     [&itemsFreqMap](const T &a, const T &b) {
                       return itemsFreqMap.at(a) > itemsFreqMap.at(b);
                     });

    return keys;
  }
};
