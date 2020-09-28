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
