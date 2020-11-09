#pragma once

template <typename A, typename T>
bool compareArr(const A &a, const std::initializer_list<T> &b) {
  auto aIter = a.begin();
  auto bIter = b.begin();
  while (aIter != a.end() && bIter != b.end()) {
    if (*aIter != *bIter) return false;
    aIter++;
    bIter++;
    if (aIter == a.end() && bIter != b.end()) return false;
    if (aIter != a.end() && bIter == b.end()) return false;
  }
  return true;
}

template <typename A, typename T>
bool compareArrUnordered(const A &a, const std::initializer_list<T> &b) {
  if (a.size() != b.size()) return false;

  for (auto &&i : b) {
    if (std::find(a.cbegin(), a.cend(), i) == a.cend()) {
      return false;
    }
  }

  return true;
}

template <typename A, typename T>
bool compareSet(const A &a, const std::initializer_list<T> &b) {
  for (auto &&i : b) {
    if (a.count(i) == 0) return false;
  }
  return a.size() == b.size();
}
