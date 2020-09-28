#pragma once

#include <algorithm>
#include <map>
#include <vector>

#include "PatternList.h"

template <typename T = int>
struct Transaction {
  std::vector<T> items;
  size_t trId;

  template <typename C>
  Transaction(const C &elements, size_t trId = 0)
      : items(elements.begin(), elements.end()), trId(trId) {}

  Transaction(size_t trId = 0) : trId(trId) {}

  // Copy constructor
  Transaction(const Transaction<T> &other)
      : items(other.items), trId(other.trId) {}

  bool includes(const T &val) const {
    return std::find(items.cbegin(), items.cend(), val) != items.cend();
  }
};

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
    addTransaction(Transaction(elements));
  }

  // Also sets trId
  void addTransaction(const Transaction<T> &transaction) {
    transactions.push_back(transaction);
    transactions[transactions.size() - 1].trId = transactions.size() - 1;
    elCount += transaction.items.size();
  }

  inline size_t size() const { return transactions.size(); }

  // Removes a pattern footprint from transactions (for residual dataset)
  void removePattern(const Pattern<T> &pattern) {
    for (size_t i = 0; i < transactions.size(); i++) {
      // if is in pattern transactions
      if (pattern.transactionIds.count(transactions[i].trId) > 0) {
        std::vector<T> resultRow;

        for (const auto &trItem : transactions[i].items) {
          // if is in pattern items
          if (pattern.itemIds.count(trItem) == 0) {
            resultRow.push_back(trItem);
          }
        }

        const int diff = transactions[i].items.size() - resultRow.size();
        transactions[i].items = resultRow;
        elCount -= diff;
      }
    }
  }

  // Build a frequency map for every item
  std::map<T, size_t> getItemsFreq() const {
    std::map<int, size_t> itemsFreqMap;
    for (size_t trId = 0; trId < transactions.size(); trId++) {
      for (auto &&i : transactions[trId].items) {
        if (itemsFreqMap.find(i) == itemsFreqMap.end()) {
          itemsFreqMap[i] = 1;
        } else {
          itemsFreqMap[i]++;
        }
      }
    }

    return itemsFreqMap;
  }

  // By frequency sorting strategy for greedy method
  void sortByFreq() {
    const auto itemsFreqMap = getItemsFreq();

    // Sort every transaction items
    for (size_t i = 0; i < transactions.size(); i++) {
      std::vector<T> &items = transactions[i].items;
      std::stable_sort(items.begin(), items.end(),
                [&itemsFreqMap](const T &a, const T &b) {
                  return itemsFreqMap.at(a) > itemsFreqMap.at(b);
                });
    }

    // Sort transactions
    std::stable_sort(
        transactions.begin(), transactions.end(),
        [&itemsFreqMap](const Transaction<T> &a, const Transaction<T> &b) {
          size_t aFreq = 0;
          for (auto &&i : a.items) {
            aFreq += itemsFreqMap.at(i);
          }
          size_t bFreq = 0;
          for (auto &&i : b.items) {
            bFreq += itemsFreqMap.at(i);
          }
          return aFreq > bFreq;
        });
  }
};
