#include "../src/PatternList.h"
#include "doctest.h"

TEST_CASE("Complexity Calculation") {
  PatternList patterns;
  CHECK(patterns.complexity == 0);

  Pattern p1;
  patterns.addPattern(p1);
  CHECK(patterns.complexity == 0);

  Pattern p2;
  p2.addItem(5);
  p2.addItem(5);
  p2.addItem(3);
  p2.addTransaction(5);
  p2.addTransaction(5);
  p2.addTransaction(3);
  p2.addTransaction(8);
  patterns.addPattern(p2);
  CHECK(patterns.complexity == 5);

  Pattern p3;
  p3.addItem(1);
  p3.addItem(2);
  p3.addTransaction(3);
  patterns.addPattern(p3);
  CHECK(patterns.complexity == 8);

  Pattern p4;
  p4.addItem(1);
  p4.addItem(2);
  p4.addItem(5);
  p4.addTransaction(3);
  p4.addTransaction(8);
  patterns.addPattern(p4);
  CHECK(patterns.complexity == 13);
}
