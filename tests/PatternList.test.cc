#include <iostream>

#include "../src/PatternList.h"
#include "common.h"
#include "doctest.h"

TEST_CASE("Uncovered") {
  Pattern p;
  auto uncovered = p.transactionsUncovered(10);
  CHECK(compareArr(uncovered, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}));

  p.addTransaction(6);
  p.addTransaction(8);
  uncovered = p.transactionsUncovered(10);
  CHECK(compareArr(uncovered, {0, 1, 2, 3, 4, 5, 7, 9}));

  p.addTransaction(0);
  uncovered = p.transactionsUncovered(10);
  CHECK(compareArr(uncovered, {1, 2, 3, 4, 5, 7, 9}));
}

TEST_CASE("COVERS") {
  PatternList patterns;
  CHECK(!patterns.covers(0, 0));

  Pattern p1;
  patterns.addPattern(p1);
  CHECK(!patterns.covers(0, 0));

  Pattern p2;
  p2.addItem(5);
  p2.addItem(5);
  p2.addItem(3);
  p2.addTransaction(5);
  p2.addTransaction(5);
  p2.addTransaction(3);
  p2.addTransaction(8);
  patterns.addPattern(p2);
  CHECK(patterns.covers(5, 3));
  CHECK(!patterns.covers(5, 2));
  CHECK(!patterns.covers(6, 5));
  CHECK(!patterns.covers(6, 8));

  Pattern p3;
  p3.addItem(1);
  p3.addItem(2);
  p3.addTransaction(3);
  patterns.addPattern(p3);
  CHECK(patterns.covers(5, 3));
  CHECK(patterns.covers(3, 2));
}

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
