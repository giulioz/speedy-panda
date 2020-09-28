#include "../src/PatternList.h"
#include "../src/TransactionList.h"
#include "doctest.h"

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

TEST_CASE("Noise Calculation") {
  // TransactionList dataset;
  // PatternList patterns;
  // CHECK(calcNoise(patterns, dataset) == 0);

  // dataset.addTransaction({0, 1, 2});
  // CHECK(calcNoise(patterns, dataset) == 3);
  // dataset.addTransaction({0, 1, 5});
  // CHECK(calcNoise(patterns, dataset) == 6);

  // Pattern p1;
  // p1.itemIds = {0, 1, 2};
  // p1.transactionIds = {0, 1};
  // patterns.addPattern(p1);
  // CHECK(calcNoise(patterns, dataset) == 2);

  // dataset.addTransaction({0, 1, 2});
  // CHECK(calcNoise(patterns, dataset) == 5);

  // dataset.addTransaction({7, 8, 9});
  // dataset.addTransaction({7, 8, 9});
  // dataset.addTransaction({5, 10, 11});
  // dataset.addTransaction({8, 9, 7});
  // dataset.addTransaction({7, 8, 9, 10});
  // CHECK(calcNoise(patterns, dataset) == 21);

  // Pattern p2;
  // p2.itemIds = {9, 8, 7};
  // p2.transactionIds = {3, 4, 6, 7};
  // patterns.addPattern(p2);
  // CHECK(calcNoise(patterns, dataset) == 9);

  // SUBCASE("Overlapping patterns") {
  //   dataset.addTransaction({3, 4, 5});
  //   dataset.addTransaction({3, 4, 5});
  //   dataset.addTransaction({3, 4, 5, 6, 7, 8});
  //   dataset.addTransaction({3, 4, 5, 6, 7, 8, 9});
  //   CHECK(calcNoise(patterns, dataset) == 28);

  //   Pattern p3;
  //   p3.itemIds = {3, 4, 5};
  //   p3.transactionIds = {8, 9, 10, 11};
  //   patterns.addPattern(p3);
  //   CHECK(calcNoise(patterns, dataset) == 16);

  //   Pattern p4;
  //   p4.itemIds = {6, 7, 8};
  //   p4.transactionIds = {10, 11};
  //   patterns.addPattern(p4);
  //   CHECK(calcNoise(patterns, dataset) == 10);
  // }
}
