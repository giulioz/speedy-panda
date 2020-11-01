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
    auto [core, extensionList, resultFalseNegatives] =
        findCore(patterns, dataset, 0, 0.5);
    CHECK(resultFalseNegatives == 0);
    CHECK(compareSet(core.itemIds, {0, 1, 2}));
    CHECK(compareSet(core.transactionIds, {0, 1}));
  }
  SUBCASE("Sparse 1") {
    TransactionList dataset;
    dataset.addTransaction({0, 1, 2});
    dataset.addTransaction({0, 1});
    dataset.addTransaction({0, 1});
    PatternList<int> patterns;
    auto [core, extensionList, resultFalseNegatives] =
        findCore(patterns, dataset, 0, 0.5);
    CHECK(resultFalseNegatives == 1);
    CHECK(compareSet(core.itemIds, {0, 1}));
    CHECK(compareSet(core.transactionIds, {0, 1, 2}));
  }
  SUBCASE("Sparse 2") {
    TransactionList dataset;
    dataset.addTransaction({0, 1, 2});
    dataset.addTransaction({0, 1, 2, 3});
    dataset.addTransaction({0});
    PatternList<int> patterns;
    auto [core, extensionList, resultFalseNegatives] =
        findCore(patterns, dataset, 0, 0.5);
    CHECK(resultFalseNegatives == 2);
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
    auto [core, extensionList, resultFalseNegatives] =
        findCore(patterns, dataset, 0, 0.5);
    CHECK(resultFalseNegatives == 6);
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
    auto [core, extensionList, resultFalseNegatives] =
        findCore(patterns, dataset, 0, 0.5);
    CHECK(resultFalseNegatives == 8);
    CHECK(compareSet(core.itemIds, {0, 1}));
    CHECK(compareSet(core.transactionIds, {0, 1, 2, 3, 4, 5}));
    patterns.addPattern(core);
    dataset.removePattern(core);

    auto [core2, extensionList2, resultFalseNegatives2] =
        findCore(patterns, dataset, 0, 0.5);
    CHECK(resultFalseNegatives2 == 2);
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
    auto [core, extensionList, resultFalseNegatives] =
        findCore(patterns, dataset, 0, 0.5);
    auto [extendedCore, resultFalsePositives2, resultFalseNegatives2] =
        extendCore(patterns, dataset, core, extensionList, resultFalseNegatives,
                   0, 1.0, 1.0, 0.5);
    CHECK(resultFalsePositives2 == 0);
    CHECK(resultFalseNegatives2 == 0);
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
    auto [core, extensionList, resultFalseNegatives] =
        findCore(patterns, dataset, 0, 0.5);
    auto [extendedCore, resultFalsePositives2, resultFalseNegatives2] =
        extendCore(patterns, dataset, core, extensionList, resultFalseNegatives,
                   0, 1.0, 1.0, 0.5);
    CHECK(resultFalsePositives2 == 1);
    CHECK(resultFalseNegatives2 == 0);
    CHECK(compareSet(extendedCore.itemIds, {0, 1, 2}));
    CHECK(compareSet(extendedCore.transactionIds, {0, 1, 2, 3, 4}));
  }
  SUBCASE("Sparse 2") {
    TransactionList dataset;
    dataset.addTransaction({0, 1, 2});
    dataset.addTransaction({0, 1, 2, 3});
    dataset.addTransaction({0});
    PatternList<int> patterns;
    auto [core, extensionList, resultFalseNegatives] =
        findCore(patterns, dataset, 0, 0.5);
    auto [extendedCore, resultFalsePositives2, resultFalseNegatives2] =
        extendCore(patterns, dataset, core, extensionList, resultFalseNegatives,
                   0, 1.0, 1.0, 0.5);
    CHECK(resultFalsePositives2 == 0);
    CHECK(resultFalseNegatives2 == 2);
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
    auto [core, extensionList, resultFalseNegatives] =
        findCore(patterns, dataset, 0, 0.5);
    auto [extendedCore, resultFalsePositives2, resultFalseNegatives2] =
        extendCore(patterns, dataset, core, extensionList, resultFalseNegatives,
                   0, 1.0, 1.0, 0.5);
    CHECK(resultFalsePositives2 == 0);
    CHECK(resultFalseNegatives2 == 6);
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
    auto [core, extensionList, resultFalseNegatives] =
        findCore(patterns, dataset, 0, 0.5);
    auto [extendedCore, resultFalsePositives2, resultFalseNegatives2] =
        extendCore(patterns, dataset, core, extensionList, resultFalseNegatives,
                   0, 1.0, 1.0, 0.5);
    CHECK(resultFalsePositives2 == 1);
    CHECK(resultFalseNegatives2 == 0);
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
    auto [core, extensionList, resultFalseNegatives] =
        findCore(patterns, dataset, 0, 0.5);
    auto [extendedCore, resultFalsePositives2, resultFalseNegatives2] =
        extendCore(patterns, dataset, core, extensionList, resultFalseNegatives,
                   0, 1.0, 1.0, 0.5);
    CHECK(resultFalsePositives2 == 1);
    CHECK(resultFalseNegatives2 == 0);
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
    auto [core, extensionList, resultFalseNegatives] =
        findCore(patterns, dataset, 0, 0.5);
    auto [extendedCore, resultFalsePositivesE, resultFalseNegativesE] =
        extendCore(patterns, dataset, core, extensionList, resultFalseNegatives,
                   0, 1.0, 1.0, 0.5);
    CHECK(resultFalsePositivesE == 0);
    CHECK(resultFalseNegativesE == 8);
    CHECK(compareSet(extendedCore.itemIds, {0, 1}));
    CHECK(compareSet(extendedCore.transactionIds, {0, 1, 2, 3, 4, 5}));
    patterns.addPattern(extendedCore);
    dataset.removePattern(extendedCore);

    auto [core2, extensionList2, resultFalseNegatives2] =
        findCore(patterns, dataset, resultFalsePositivesE, 0.5);
    auto [extendedCore2, resultFalsePositivesE2, resultFalseNegativesE2] =
        extendCore(patterns, dataset, core2, extensionList2,
                   resultFalseNegatives2, resultFalsePositivesE, 1.0, 1.0, 0.5);
    CHECK(resultFalsePositivesE2 == 0);
    CHECK(resultFalseNegativesE2 == 2);
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
    auto [core, extensionList, resultFalseNegatives] =
        findCore(patterns, dataset, 0, 0.5);
    auto [extendedCore, resultFalsePositivesE, resultFalseNegativesE] =
        extendCore(patterns, dataset, core, extensionList, resultFalseNegatives,
                   0, 1.0, 1.0, 0.5);
    CHECK(resultFalsePositivesE == 0);
    CHECK(resultFalseNegativesE == 15);
    CHECK(compareSet(extendedCore.itemIds, {0, 1}));
    CHECK(compareSet(extendedCore.transactionIds,
                     {0, 1, 2, 3, 4, 5, 6, 7, 9, 10, 11}));
    patterns.addPattern(extendedCore);
    dataset.removePattern(extendedCore);

    auto [core2, extensionList2, resultFalseNegatives2] =
        findCore(patterns, dataset, resultFalsePositivesE, 0.5);
    auto [extendedCore2, resultFalsePositivesE2, resultFalseNegativesE2] =
        extendCore(patterns, dataset, core2, extensionList2,
                   resultFalseNegatives2, resultFalsePositivesE, 1.0, 1.0, 0.5);
    CHECK(resultFalsePositivesE2 == 1);
    CHECK(resultFalseNegativesE2 == 1);
    CHECK(compareSet(extendedCore2.itemIds, {2, 3, 4}));
    CHECK(compareSet(extendedCore2.transactionIds, {0, 1, 2, 3, 4}));
  }
}
