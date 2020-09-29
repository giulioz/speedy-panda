#include "../src/panda.h"
#include "common.h"
#include "doctest.h"

TEST_CASE("findCore") {
  SUBCASE("Simple") {
    TransactionList dataset;
    dataset.addTransaction({0, 1, 2});
    dataset.addTransaction({0, 1, 2});
    ResultState state(dataset);
    auto [core, extensionList] = findCore(state);
    CHECK(compareSet(core.itemIds, {0, 1, 2}));
    CHECK(compareSet(core.transactionIds, {0, 1}));
  }
}
