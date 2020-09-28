#pragma once

#include <map>
#include <vector>
#include <algorithm>

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

  std::map<T, size_t> getItemsFreq() const {
    std::map<int, size_t> itemsFreqMap;
    for (size_t trId = 0; trId < size(); trId++) {
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
};
