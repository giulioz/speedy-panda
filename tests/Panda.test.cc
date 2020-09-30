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
    ResultState state(dataset);
    auto [core, extensionList] = findCore(state, 0.5);
    CHECK(compareSet(core.itemIds, {0, 1, 2}));
    CHECK(compareSet(core.transactionIds, {0, 1}));
  }
  SUBCASE("Sparse 1") {
    TransactionList dataset;
    dataset.addTransaction({0, 1, 2});
    dataset.addTransaction({0, 1});
    dataset.addTransaction({0, 1});
    ResultState state(dataset);
    auto [core, extensionList] = findCore(state, 0.5);
    CHECK(compareSet(core.itemIds, {0, 1}));
    CHECK(compareSet(core.transactionIds, {0, 1, 2}));
  }
  SUBCASE("Sparse 2") {
    TransactionList dataset;
    dataset.addTransaction({0, 1, 2});
    dataset.addTransaction({0, 1, 2, 3});
    dataset.addTransaction({0});
    ResultState state(dataset);
    auto [core, extensionList] = findCore(state, 0.5);
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
    ResultState state(dataset);
    auto [core, extensionList] = findCore(state, 0.5);
    CHECK(compareSet(core.itemIds, {0, 1}));
    CHECK(compareSet(core.transactionIds, {0, 1, 3, 4}));
  }
}

TEST_CASE("extendCore") {
  SUBCASE("Dense") {
    TransactionList dataset;
    dataset.addTransaction({0, 1, 2});
    dataset.addTransaction({0, 1, 2});
    ResultState state(dataset);
    auto [core, extensionList] = findCore(state, 0.5);
    core = extendCore(state, core, extensionList, 1.0, 1.0, 0.5);
    CHECK(compareSet(core.itemIds, {0, 1, 2}));
    CHECK(compareSet(core.transactionIds, {0, 1}));
  }
  SUBCASE("Sparse 1") {
    TransactionList dataset;
    dataset.addTransaction({0, 1, 2});
    dataset.addTransaction({0, 1, 2});
    dataset.addTransaction({0, 1, 2});
    dataset.addTransaction({0, 1, 2});
    dataset.addTransaction({0, 1});
    ResultState state(dataset);
    auto [core, extensionList] = findCore(state, 0.5);
    core = extendCore(state, core, extensionList, 1.0, 1.0, 0.5);
    CHECK(compareSet(core.itemIds, {0, 1, 2}));
    CHECK(compareSet(core.transactionIds, {0, 1, 2, 3, 4}));
  }
  SUBCASE("Sparse 2") {
    TransactionList dataset;
    dataset.addTransaction({0, 1, 2});
    dataset.addTransaction({0, 1, 2, 3});
    dataset.addTransaction({0});
    ResultState state(dataset);
    auto [core, extensionList] = findCore(state, 0.5);
    core = extendCore(state, core, extensionList, 1.0, 1.0, 0.5);
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
    ResultState state(dataset);
    auto [core, extensionList] = findCore(state, 0.5);
    core = extendCore(state, core, extensionList, 1.0, 1.0, 0.5);
    CHECK(compareSet(core.itemIds, {0, 1}));
    CHECK(compareSet(core.transactionIds, {0, 1, 3, 4}));
  }
}
