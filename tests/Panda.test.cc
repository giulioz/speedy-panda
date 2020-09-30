#include "../src/panda.h"
#include "common.h"
#include "doctest.h"

TEST_CASE("notTooNoisy") {
  
}

TEST_CASE("findCore") {
  SUBCASE("Dense") {
    TransactionList dataset;
    dataset.addTransaction({0, 1, 2});
    dataset.addTransaction({0, 1, 2});
    ResultState state(dataset);
    auto [core, extensionList] = findCore(state);
    CHECK(compareSet(core.itemIds, {0, 1, 2}));
    CHECK(compareSet(core.transactionIds, {0, 1}));
  }
  SUBCASE("Sparse 1") {
    TransactionList dataset;
    dataset.addTransaction({0, 1, 2});
    dataset.addTransaction({0, 1});
    dataset.addTransaction({0, 1});
    ResultState state(dataset);
    auto [core, extensionList] = findCore(state);
    CHECK(compareSet(core.itemIds, {0, 1}));
    CHECK(compareSet(core.transactionIds, {0, 1, 2}));
  }
  SUBCASE("Sparse 2") {
    TransactionList dataset;
    dataset.addTransaction({0, 1, 2});
    dataset.addTransaction({0, 1, 2, 3});
    dataset.addTransaction({0});
    ResultState state(dataset);
    auto [core, extensionList] = findCore(state);
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
    auto [core, extensionList] = findCore(state);
    CHECK(compareSet(core.itemIds, {0, 1}));
    CHECK(compareSet(core.transactionIds, {0, 1, 3, 4}));
  }
}
