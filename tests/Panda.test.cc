#include "../src/panda.h"
#include "common.h"
#include "doctest.h"

TEST_CASE("notTooNoisy") {
  SUBCASE("Dense") {
    TransactionList dataset;
    dataset.addTransaction({0, 1, 2});
    dataset.addTransaction({0, 1, 2});
    Pattern pattern({0, 1, 2}, {0, 1});
    CHECK(notTooNoisy(dataset, pattern, 1.0, 1.0));
    CHECK(notTooNoisy(dataset, pattern, 0.001, 0.001));
  }
  SUBCASE("Sparse 1") {
    TransactionList dataset;
    dataset.addTransaction({0, 1, 2});
    dataset.addTransaction({0, 1, 2});
    dataset.addTransaction({0, 1});
    Pattern pattern({0, 1, 2}, {0, 1, 2});
    CHECK(notTooNoisy(dataset, pattern, 1.0, 1.0));
    CHECK(!notTooNoisy(dataset, pattern, 0.001, 1.0));
  }
  SUBCASE("Sparse 2") {
    TransactionList dataset;
    dataset.addTransaction({0, 1, 2});
    dataset.addTransaction({0, 1});
    dataset.addTransaction({3, 4, 5});
    Pattern pattern({0, 1, 2}, {0, 1, 2});
    CHECK(notTooNoisy(dataset, pattern, 1.0, 1.0));
    CHECK(!notTooNoisy(dataset, pattern, 1.0, 0.001));
  }
}

TEST_CASE("findCore") {
  SUBCASE("Dense") {
    TransactionList dataset;
    dataset.addTransaction({0, 1, 2});
    dataset.addTransaction({0, 1, 2});
    PatternList<int> patterns;
    auto [core, extensionList] = findCore(patterns, dataset, dataset, 0.5);
    CHECK(compareSet(core.itemIds, {0, 1, 2}));
    CHECK(compareSet(core.transactionIds, {0, 1}));
  }
  SUBCASE("Sparse 1") {
    TransactionList dataset;
    dataset.addTransaction({0, 1, 2});
    dataset.addTransaction({0, 1});
    dataset.addTransaction({0, 1});
    PatternList<int> patterns;
    auto [core, extensionList] = findCore(patterns, dataset, dataset, 0.5);
    CHECK(compareSet(core.itemIds, {0, 1}));
    CHECK(compareSet(core.transactionIds, {0, 1, 2}));
  }
  SUBCASE("Sparse 2") {
    TransactionList dataset;
    dataset.addTransaction({0, 1, 2});
    dataset.addTransaction({0, 1, 2, 3});
    dataset.addTransaction({0});
    PatternList<int> patterns;
    auto [core, extensionList] = findCore(patterns, dataset, dataset, 0.5);
    CHECK(compareSet(core.itemIds, {0, 1, 2}));
    CHECK(compareSet(core.transactionIds, {0, 1}));
  }
  SUBCASE("Sparse 3") {
    TransactionList dataset;
    dataset.addTransaction({0, 1, 2});
    dataset.addTransaction({0, 1, 2, 3});
    dataset.addTransaction({0});
    dataset.addTransaction({0, 1});
    dataset.addTransaction({1, 0});
    dataset.addTransaction({1, 2});
    PatternList<int> patterns;
    auto [core, extensionList] = findCore(patterns, dataset, dataset, 0.5);
    CHECK(compareSet(core.itemIds, {0, 1}));
    CHECK(compareSet(core.transactionIds, {0, 1, 3, 4}));
  }
  SUBCASE("Multiple 1") {
    TransactionList dataset;
    dataset.addTransaction({0, 1, 2, 3});
    dataset.addTransaction({0, 1, 2, 3});
    dataset.addTransaction({0, 1, 2, 3});
    dataset.addTransaction({0, 1});
    dataset.addTransaction({0, 1});
    dataset.addTransaction({1, 0});
    dataset.addTransaction({1, 2});
    PatternList<int> patterns;
    auto [core, extensionList] = findCore(patterns, dataset, dataset, 0.5);
    CHECK(compareSet(core.itemIds, {0, 1}));
    CHECK(compareSet(core.transactionIds, {0, 1, 2, 3, 4, 5}));
    patterns.addPattern(core);
    dataset.removePattern(core);

    auto [core2, extensionList2] = findCore(patterns, dataset, dataset, 0.5);
    CHECK(compareSet(core2.itemIds, {2, 3}));
    CHECK(compareSet(core2.transactionIds, {0, 1, 2}));
  }
}

TEST_CASE("extendCore") {
  SUBCASE("Dense") {
    TransactionList dataset;
    dataset.addTransaction({0, 1, 2});
    dataset.addTransaction({0, 1, 2});
    PatternList<int> patterns;
    auto [core, extensionList] = findCore(patterns, dataset, dataset, 0.5);
    auto extendedCore =
        extendCore(patterns, dataset, core, extensionList, 1.0, 1.0, 0.5);
    CHECK(compareSet(extendedCore.itemIds, {0, 1, 2}));
    CHECK(compareSet(extendedCore.transactionIds, {0, 1}));
  }
  SUBCASE("Sparse 1") {
    TransactionList dataset;
    dataset.addTransaction({0, 1, 2});
    dataset.addTransaction({0, 1, 2});
    dataset.addTransaction({0, 1, 2});
    dataset.addTransaction({0, 1, 2});
    dataset.addTransaction({0, 1});
    PatternList<int> patterns;
    auto [core, extensionList] = findCore(patterns, dataset, dataset, 0.5);
    auto extendedCore =
        extendCore(patterns, dataset, core, extensionList, 1.0, 1.0, 0.5);
    CHECK(compareSet(extendedCore.itemIds, {0, 1, 2}));
    CHECK(compareSet(extendedCore.transactionIds, {0, 1, 2, 3, 4}));
  }
  SUBCASE("Sparse 2") {
    TransactionList dataset;
    dataset.addTransaction({0, 1, 2});
    dataset.addTransaction({0, 1, 2, 3});
    dataset.addTransaction({0});
    PatternList<int> patterns;
    auto [core, extensionList] = findCore(patterns, dataset, dataset, 0.5);
    auto extendedCore =
        extendCore(patterns, dataset, core, extensionList, 1.0, 1.0, 0.5);
    CHECK(compareSet(extendedCore.itemIds, {0, 1, 2}));
    CHECK(compareSet(extendedCore.transactionIds, {0, 1}));
  }
  SUBCASE("Sparse 3") {
    TransactionList dataset;
    dataset.addTransaction({0, 1, 2});
    dataset.addTransaction({0, 1, 2, 3});
    dataset.addTransaction({0});
    dataset.addTransaction({0, 1});
    dataset.addTransaction({1, 0});
    dataset.addTransaction({1, 2});
    PatternList<int> patterns;
    auto [core, extensionList] = findCore(patterns, dataset, dataset, 0.5);
    auto extendedCore =
        extendCore(patterns, dataset, core, extensionList, 1.0, 1.0, 0.5);
    CHECK(compareSet(extendedCore.itemIds, {0, 1}));
    CHECK(compareSet(extendedCore.transactionIds, {0, 1, 3, 4}));
  }
  SUBCASE("Sparse 4") {
    TransactionList dataset;
    dataset.addTransaction({0, 1, 2, 3});
    dataset.addTransaction({0, 1, 2, 3});
    dataset.addTransaction({0, 1, 2, 3});
    dataset.addTransaction({0, 1, 2, 3});
    dataset.addTransaction({0, 1, 2, 3});
    dataset.addTransaction({0, 1, 2, 3});
    dataset.addTransaction({0, 1, 2, 3});
    dataset.addTransaction({0, 1, 2, 3});
    dataset.addTransaction({0, 2, 3});
    dataset.addTransaction({0, 1, 2, 3});
    dataset.addTransaction({0, 1, 2, 3});
    dataset.addTransaction({0, 1, 2, 3});
    PatternList<int> patterns;
    auto [core, extensionList] = findCore(patterns, dataset, dataset, 0.5);
    auto extendedCore =
        extendCore(patterns, dataset, core, extensionList, 1.0, 1.0, 0.5);
    CHECK(compareSet(extendedCore.itemIds, {0, 1, 2, 3}));
    CHECK(compareSet(extendedCore.transactionIds,
                     {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11}));
  }
  SUBCASE("Sparse 5") {
    TransactionList dataset;
    dataset.addTransaction({0, 1, 2});
    dataset.addTransaction({0, 1, 2});
    dataset.addTransaction({0, 1, 2});
    dataset.addTransaction({0, 1, 2});
    dataset.addTransaction({0, 1, 2});
    dataset.addTransaction({0, 1, 2});
    dataset.addTransaction({0, 1, 2});
    dataset.addTransaction({0, 1, 2});
    dataset.addTransaction({0, 1});
    dataset.addTransaction({0, 1, 2});
    dataset.addTransaction({0, 1, 2});
    dataset.addTransaction({0, 1, 2});
    PatternList<int> patterns;
    auto [core, extensionList] = findCore(patterns, dataset, dataset, 0.5);
    auto extendedCore =
        extendCore(patterns, dataset, core, extensionList, 1.0, 1.0, 0.5);
    CHECK(compareSet(extendedCore.itemIds, {0, 1, 2}));
    CHECK(compareSet(extendedCore.transactionIds,
                     {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11}));
  }
  SUBCASE("Multiple 1") {
    TransactionList dataset;
    dataset.addTransaction({0, 1, 2, 3});
    dataset.addTransaction({0, 1, 2, 3});
    dataset.addTransaction({0, 1, 2, 3});
    dataset.addTransaction({0, 1});
    dataset.addTransaction({0, 1});
    dataset.addTransaction({1, 0});
    dataset.addTransaction({1, 2});
    PatternList<int> patterns;
    auto [core, extensionList] = findCore(patterns, dataset, dataset, 0.5);
    auto extendedCore =
        extendCore(patterns, dataset, core, extensionList, 1.0, 1.0, 0.5);
    CHECK(compareSet(extendedCore.itemIds, {0, 1}));
    CHECK(compareSet(extendedCore.transactionIds, {0, 1, 2, 3, 4, 5}));
    patterns.addPattern(extendedCore);
    dataset.removePattern(extendedCore);

    auto [core2, extensionList2] = findCore(patterns, dataset, dataset, 0.5);
    auto extendedCore2 =
        extendCore(patterns, dataset, core2, extensionList2, 1.0, 1.0, 0.5);
    CHECK(compareSet(extendedCore2.itemIds, {2, 3}));
    CHECK(compareSet(extendedCore2.transactionIds, {0, 1, 2}));
  }
  SUBCASE("Multiple 2") {
    TransactionList dataset;
    dataset.addTransaction({0, 1, 2, 3, 4});
    dataset.addTransaction({0, 1, 2, 3, 4});
    dataset.addTransaction({0, 1, 2, 3, 4});
    dataset.addTransaction({0, 1, 2, 3, 4});
    dataset.addTransaction({0, 1, 2, 4});
    dataset.addTransaction({0, 1});
    dataset.addTransaction({0, 1});
    dataset.addTransaction({0, 1});
    dataset.addTransaction({0});
    dataset.addTransaction({0, 1});
    dataset.addTransaction({0, 1});
    dataset.addTransaction({0, 1});
    PatternList<int> patterns;
    auto [core, extensionList] = findCore(patterns, dataset, dataset, 0.5);
    auto extendedCore =
        extendCore(patterns, dataset, core, extensionList, 1.0, 1.0, 0.5);
    CHECK(compareSet(extendedCore.itemIds, {0, 1}));
    CHECK(compareSet(extendedCore.transactionIds,
                     {0, 1, 2, 3, 4, 5, 6, 7, 9, 10, 11}));
    patterns.addPattern(extendedCore);
    dataset.removePattern(extendedCore);

    auto [core2, extensionList2] = findCore(patterns, dataset, dataset, 0.5);
    auto extendedCore2 =
        extendCore(patterns, dataset, core2, extensionList2, 1.0, 1.0, 0.5);
    CHECK(compareSet(extendedCore2.itemIds, {2, 3, 4}));
    CHECK(compareSet(extendedCore2.transactionIds, {0, 1, 2, 3, 4}));
  }
  SUBCASE("Multiple 3") {
    TransactionList dataset;
    dataset.addTransaction({0, 1, 2});
    dataset.addTransaction({0, 1, 2});
    dataset.addTransaction({0, 1, 2, 3, 4, 5});
    dataset.addTransaction({0, 1, 2, 3, 4, 5});
    dataset.addTransaction({1, 2, 3, 4, 5});
    dataset.addTransaction({1, 2, 3, 4, 5});
    dataset.addTransaction({1, 2, 3, 4, 5, 6, 7, 8});
    dataset.addTransaction({4, 5, 6, 7, 8});
    PatternList<int> patterns;
    auto [core, extensionList] = findCore(patterns, dataset, dataset, 0.5);
    auto extendedCore =
        extendCore(patterns, dataset, core, extensionList, 1.0, 1.0, 0.5);
    CHECK(compareSet(extendedCore.itemIds, {1, 2, 3, 4, 5}));
    CHECK(compareSet(extendedCore.transactionIds, {2, 3, 4, 5, 6}));
    patterns.addPattern(extendedCore);
    dataset.removePattern(extendedCore);

    auto [core2, extensionList2] = findCore(patterns, dataset, dataset, 0.5);
    auto extendedCore2 =
        extendCore(patterns, dataset, core2, extensionList2, 1.0, 1.0, 0.5);
    CHECK(compareSet(extendedCore2.itemIds, {0, 1, 2}));
    CHECK(compareSet(extendedCore2.transactionIds, {0, 1, 2, 3}));
    patterns.addPattern(extendedCore2);
    dataset.removePattern(extendedCore2);

    auto [core3, extensionList3] = findCore(patterns, dataset, dataset, 0.5);
    auto extendedCore3 =
        extendCore(patterns, dataset, core3, extensionList3, 1.0, 1.0, 0.5);
    CHECK(compareSet(extendedCore3.itemIds, {4, 5, 6, 7, 8}));
    CHECK(compareSet(extendedCore3.transactionIds, {6, 7}));
    patterns.addPattern(extendedCore3);
    dataset.removePattern(extendedCore3);
  }
}
