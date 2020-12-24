#include <initializer_list>
#include <iostream>

#include "../src/PatternList.h"
#include "../src/TransactionList.h"
#include "common.h"
#include "doctest.h"

TEST_CASE("ElCount and Construction") {
  TransactionList dataset;
  CHECK(dataset.elCount == 0);

  dataset.addTransaction({0, 1, 2});
  CHECK(dataset.elCount == 3);

  dataset.addTransaction({3, 4});
  CHECK(dataset.elCount == 5);

  CHECK(trIncludeItem(dataset.transactions[1], 3));
  CHECK(!trIncludeItem(dataset.transactions[1], 5));
}

TEST_CASE("Remove pattern") {
  TransactionList dataset;
  dataset.addTransaction({0, 1, 2});  // 0
  dataset.addTransaction({0, 1, 2});  // 1
  dataset.addTransaction({0, 1, 2});  // 2
  dataset.addTransaction({3, 4, 5});  // 3
  CHECK(dataset.size() == 4);
  CHECK(dataset.elCount == 12);
  CHECK(dataset.transactions[0].size() == 3);
  CHECK(dataset.transactions[1].size() == 3);
  CHECK(dataset.transactions[2].size() == 3);
  CHECK(dataset.transactions[3].size() == 3);

  Pattern p1;
  p1.itemIds = {0, 1, 2};
  p1.transactionIds = {0, 1, 2};
  dataset.removePattern(p1);
  CHECK(dataset.size() == 4);
  CHECK(dataset.elCount == 3);
  dataset.removePattern(p1);
  CHECK(dataset.size() == 4);
  CHECK(dataset.elCount == 3);
  CHECK(dataset.transactions[0].size() == 0);
  CHECK(dataset.transactions[1].size() == 0);
  CHECK(dataset.transactions[2].size() == 0);
  CHECK(dataset.transactions[3].size() == 3);

  dataset.addTransaction({3, 4, 5});     // 4
  dataset.addTransaction({3, 4, 5, 6});  // 5
  Pattern p2;
  p2.itemIds = {3, 4, 5, 6};
  p2.transactionIds = {3, 4, 5};
  dataset.removePattern(p2);
  CHECK(dataset.size() == 6);
  CHECK(dataset.elCount == 0);
  CHECK(dataset.transactions[0].size() == 0);
  CHECK(dataset.transactions[1].size() == 0);
  CHECK(dataset.transactions[2].size() == 0);
  CHECK(dataset.transactions[3].size() == 0);
  CHECK(dataset.transactions[4].size() == 0);
  CHECK(dataset.transactions[5].size() == 0);
  CHECK(!trIncludeItem(dataset.transactions[5], 6));

  SUBCASE("Overlapping patterns") {
    dataset.addTransaction({7, 8, 9, 10, 11});          // 6
    dataset.addTransaction({7, 8, 9, 10, 11});          // 7
    dataset.addTransaction({7, 8, 9, 10, 11, 12, 13});  // 8
    dataset.addTransaction({7, 8, 9, 10, 11, 12, 13});  // 9
    dataset.addTransaction({7, 8, 9, 10, 11, 12, 13});  // 10
    Pattern p3;
    p3.itemIds = {7, 8, 9, 10, 11};
    p3.transactionIds = {6, 7, 8, 9, 10};
    dataset.removePattern(p3);
    CHECK(dataset.size() == 11);
    CHECK(dataset.elCount == 6);
    CHECK(dataset.transactions[6].size() == 0);
    CHECK(dataset.transactions[7].size() == 0);
    CHECK(dataset.transactions[8].size() == 2);
    CHECK(dataset.transactions[9].size() == 2);
    CHECK(dataset.transactions[10].size() == 2);
    CHECK(compareArr(dataset.transactions[8], {12, 13}));
    CHECK(compareArr(dataset.transactions[9], {12, 13}));
    CHECK(compareArr(dataset.transactions[10], {12, 13}));
    Pattern p4;
    p4.itemIds = {11, 12, 13};
    p4.transactionIds = {8, 9, 10};
    dataset.removePattern(p4);
    CHECK(dataset.elCount == 0);
    CHECK(dataset.transactions[8].size() == 0);
    CHECK(dataset.transactions[9].size() == 0);
    CHECK(dataset.transactions[10].size() == 0);
  }
}

TEST_CASE("Sorting") {
  TransactionList dataset;
  dataset.addTransaction({2, 0});
  dataset.addTransaction({0, 1, 2});
  dataset.addTransaction({3});
  dataset.addTransaction({1, 0, 2});
  dataset.addTransaction({1, 0, 2});
  dataset.addTransaction({3});
  dataset.addTransaction({1, 0, 2});
  dataset.addTransaction({2, 7});
  auto sorted = dataset.itemsByFreq();
  CHECK(compareArr(sorted, {2, 0, 1, 3, 7}));
}
