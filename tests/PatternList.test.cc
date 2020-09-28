#include "../src/PatternList.h"
#include "doctest.h"

TEST_CASE("Complexity Calculation") {
  PatternList patterns;
  CHECK(patterns.complexity == 0);

  Pattern p1;
  patterns.addPattern(p1);
  CHECK(patterns.complexity == 0);

  Pattern p2;
  p2.itemIds.insert(5);
  p2.itemIds.insert(5);
  p2.itemIds.insert(3);
  p2.transactionIds.insert(5);
  p2.transactionIds.insert(5);
  p2.transactionIds.insert(3);
  p2.transactionIds.insert(8);
  patterns.addPattern(p2);
  CHECK(patterns.complexity == 5);

  Pattern p3;
  p3.itemIds.insert(1);
  p3.itemIds.insert(2);
  p3.transactionIds.insert(3);
  patterns.addPattern(p3);
  CHECK(patterns.complexity == 8);

  Pattern p4;
  p4.itemIds.insert(1);
  p4.itemIds.insert(2);
  p4.itemIds.insert(5);
  p4.transactionIds.insert(3);
  p4.transactionIds.insert(8);
  patterns.addPattern(p4);
  CHECK(patterns.complexity == 13);
}
