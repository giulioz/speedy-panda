#include "../src/costutils.h"
#include "doctest.h"

TEST_CASE("calcPatternsComplexity") {
  PatternList patterns;
  CHECK(calcPatternsComplexity(patterns) == 0);

  Pattern p1;
  patterns.push_back(p1);
  CHECK(calcPatternsComplexity(patterns) == 0);

  Pattern p2;
  p2.itemIds.insert(5);
  p2.itemIds.insert(5);
  p2.itemIds.insert(3);
  p2.transactionIds.insert(5);
  p2.transactionIds.insert(5);
  p2.transactionIds.insert(3);
  p2.transactionIds.insert(8);
  patterns.push_back(p2);
  CHECK(calcPatternsComplexity(patterns) == 5);

  Pattern p3;
  p3.itemIds.insert(1);
  p3.itemIds.insert(2);
  p3.transactionIds.insert(3);
  patterns.push_back(p3);
  CHECK(calcPatternsComplexity(patterns) == 8);

  Pattern p4;
  p4.itemIds.insert(1);
  p4.itemIds.insert(2);
  p4.itemIds.insert(5);
  p4.transactionIds.insert(3);
  p4.transactionIds.insert(8);
  patterns.push_back(p4);
  CHECK(calcPatternsComplexity(patterns) == 13);
}

TEST_CASE("calcNoise") {
  TransactionList dataset;
  PatternList patterns;
  CHECK(calcNoise(patterns, dataset) == 0);

  dataset.push_back({0, 1, 2});
  CHECK(calcNoise(patterns, dataset) == 3);
  dataset.push_back({0, 1, 5});
  CHECK(calcNoise(patterns, dataset) == 6);

  Pattern p1;
  p1.itemIds = {0, 1, 2};
  p1.transactionIds = {0, 1};
  patterns.push_back(p1);
  CHECK(calcNoise(patterns, dataset) == 2);

  dataset.push_back({0, 1, 2});
  CHECK(calcNoise(patterns, dataset) == 5);

  dataset.push_back({7, 8, 9});
  dataset.push_back({7, 8, 9});
  dataset.push_back({5, 10, 11});
  dataset.push_back({8, 9, 7});
  dataset.push_back({7, 8, 9, 10});
  CHECK(calcNoise(patterns, dataset) == 21);

  Pattern p2;
  p2.itemIds = {9, 8, 7};
  p2.transactionIds = {3, 4, 6, 7};
  patterns.push_back(p2);
  CHECK(calcNoise(patterns, dataset) == 9);

  SUBCASE("Overlapping patterns") {
    dataset.push_back({3, 4, 5});
    dataset.push_back({3, 4, 5});
    dataset.push_back({3, 4, 5, 6, 7, 8});
    dataset.push_back({3, 4, 5, 6, 7, 8, 9});
    CHECK(calcNoise(patterns, dataset) == 28);

    Pattern p3;
    p3.itemIds = {3, 4, 5};
    p3.transactionIds = {8, 9, 10, 11};
    patterns.push_back(p3);
    CHECK(calcNoise(patterns, dataset) == 16);

    Pattern p4;
    p4.itemIds = {6, 7, 8};
    p4.transactionIds = {10, 11};
    patterns.push_back(p4);
    CHECK(calcNoise(patterns, dataset) == 10);
  }
}
