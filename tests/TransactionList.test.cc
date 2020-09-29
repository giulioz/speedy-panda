#include <initializer_list>
#include <iostream>

#include "../src/PatternList.h"
#include "../src/TransactionList.h"
#include "doctest.h"
#include "common.h"

TEST_CASE("ElCount and Construction") {
  TransactionList dataset;
  CHECK(dataset.elCount == 0);

  dataset.addTransaction({0, 1, 2});
  CHECK(dataset.elCount == 3);
  CHECK(dataset.transactions[0].trId == 0);

  dataset.addTransaction({3, 4});
  CHECK(dataset.elCount == 5);
  CHECK(dataset.transactions[1].trId == 1);

  CHECK(dataset.transactions[1].includes(3));
  CHECK(!dataset.transactions[1].includes(5));
}

TEST_CASE("Remove pattern") {
  TransactionList dataset;
  dataset.addTransaction({0, 1, 2});  // 0
  dataset.addTransaction({0, 1, 2});  // 1
  dataset.addTransaction({0, 1, 2});  // 2
  dataset.addTransaction({3, 4, 5});  // 3
  CHECK(dataset.size() == 4);
  CHECK(dataset.elCount == 12);
  CHECK(dataset.transactions[0].items.size() == 3);
  CHECK(dataset.transactions[1].items.size() == 3);
  CHECK(dataset.transactions[2].items.size() == 3);
  CHECK(dataset.transactions[3].items.size() == 3);

  Pattern p1;
  p1.itemIds = {0, 1, 2};
  p1.transactionIds = {0, 1, 2};
  CHECK(dataset.calcPatternFalsePositives(p1) == 0);
  dataset.removePattern(p1);
  CHECK(dataset.size() == 4);
  CHECK(dataset.elCount == 3);
  CHECK(dataset.calcPatternFalsePositives(p1) == 9);
  dataset.removePattern(p1);
  CHECK(dataset.size() == 4);
  CHECK(dataset.elCount == 3);
  CHECK(dataset.transactions[0].items.size() == 0);
  CHECK(dataset.transactions[1].items.size() == 0);
  CHECK(dataset.transactions[2].items.size() == 0);
  CHECK(dataset.transactions[3].items.size() == 3);

  dataset.addTransaction({3, 4, 5});     // 4
  dataset.addTransaction({3, 4, 5, 6});  // 5
  Pattern p2;
  p2.itemIds = {3, 4, 5, 6};
  p2.transactionIds = {3, 4, 5};
  CHECK(dataset.calcPatternFalsePositives(p2) == 2);
  dataset.removePattern(p2);
  CHECK(dataset.size() == 6);
  CHECK(dataset.elCount == 0);
  CHECK(dataset.transactions[0].items.size() == 0);
  CHECK(dataset.transactions[1].items.size() == 0);
  CHECK(dataset.transactions[2].items.size() == 0);
  CHECK(dataset.transactions[3].items.size() == 0);
  CHECK(dataset.transactions[4].items.size() == 0);
  CHECK(dataset.transactions[5].items.size() == 0);
  CHECK(!dataset.transactions[5].includes(6));

  SUBCASE("Overlapping patterns") {
    dataset.addTransaction({7, 8, 9, 10, 11});          // 6
    dataset.addTransaction({7, 8, 9, 10, 11});          // 7
    dataset.addTransaction({7, 8, 9, 10, 11, 12, 13});  // 8
    dataset.addTransaction({7, 8, 9, 10, 11, 12, 13});  // 9
    dataset.addTransaction({7, 8, 9, 10, 11, 12, 13});  // 10
    Pattern p3;
    p3.itemIds = {7, 8, 9, 10, 11};
    p3.transactionIds = {6, 7, 8, 9, 10};
    CHECK(dataset.calcPatternFalsePositives(p3) == 0);
    dataset.removePattern(p3);
    CHECK(dataset.size() == 11);
    CHECK(dataset.elCount == 6);
    CHECK(dataset.transactions[6].items.size() == 0);
    CHECK(dataset.transactions[7].items.size() == 0);
    CHECK(dataset.transactions[8].items.size() == 2);
    CHECK(dataset.transactions[9].items.size() == 2);
    CHECK(dataset.transactions[10].items.size() == 2);
    CHECK(compareArr(dataset.transactions[8].items, {12, 13}));
    CHECK(compareArr(dataset.transactions[9].items, {12, 13}));
    CHECK(compareArr(dataset.transactions[10].items, {12, 13}));
    Pattern p4;
    p4.itemIds = {11, 12, 13};
    p4.transactionIds = {8, 9, 10};
    CHECK(dataset.calcPatternFalsePositives(p4) == 3);
    dataset.removePattern(p4);
    CHECK(dataset.elCount == 0);
    CHECK(dataset.transactions[8].items.size() == 0);
    CHECK(dataset.transactions[9].items.size() == 0);
    CHECK(dataset.transactions[10].items.size() == 0);
  }
}

TEST_CASE("Sorting") {
  TransactionList dataset;
  dataset.addTransaction({2, 1, 0});
  dataset.addTransaction({0, 1, 2});
  dataset.addTransaction({3, 5, 8});
  dataset.addTransaction({1, 0, 2});
  dataset.addTransaction({1, 0, 2});
  dataset.addTransaction({3, 5, 8});
  dataset.addTransaction({1, 0, 2});
  dataset.addTransaction({2, 9, 7});
  dataset.sortByFreq();
  CHECK(dataset.transactions[0].trId == 0);
  CHECK(compareArr(dataset.transactions[0].items, {2, 1, 0}));
  CHECK(dataset.transactions[1].trId == 1);
  CHECK(compareArr(dataset.transactions[1].items, {2, 0, 1}));
  CHECK(dataset.transactions[2].trId == 3);
  CHECK(compareArr(dataset.transactions[2].items, {2, 1, 0}));
  CHECK(dataset.transactions[3].trId == 4);
  CHECK(compareArr(dataset.transactions[3].items, {2, 1, 0}));
  CHECK(dataset.transactions[4].trId == 6);
  CHECK(compareArr(dataset.transactions[4].items, {2, 1, 0}));
  CHECK(dataset.transactions[5].trId == 7);
  CHECK(compareArr(dataset.transactions[5].items, {2, 9, 7}));
  CHECK(dataset.transactions[6].trId == 2);
  CHECK(compareArr(dataset.transactions[6].items, {3, 5, 8}));
  CHECK(dataset.transactions[7].trId == 5);
  CHECK(compareArr(dataset.transactions[7].items, {3, 5, 8}));
}
