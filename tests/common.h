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
