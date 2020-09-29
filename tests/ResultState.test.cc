#include "../src/PatternList.h"
#include "../src/ResultState.h"
#include "../src/TransactionList.h"
#include "common.h"
#include "doctest.h"

TEST_CASE("Noise Calculation") {
  SUBCASE("Overlapping False Positives") {
    TransactionList dataset;
    dataset.addTransaction({0, 1});
    dataset.addTransaction({0, 2, 3});
    dataset.addTransaction({1, 2, 3});
    ResultState state(dataset);
    CHECK(state.currentNoise() == 8);
    CHECK(state.tryAddPattern(Pattern({0, 1}, {0, 1}), 0.5) ==
          doctest::Approx(8).epsilon(0.01));
    state.addPattern(Pattern({0, 1}, {0, 1}));
    CHECK(state.currentNoise() == 6);
    CHECK(state.tryAddPattern(Pattern({1, 2, 3}, {1, 2}), 0.5) ==
          doctest::Approx(9.0 / 2 + 2).epsilon(0.01));
    state.addPattern(Pattern({1, 2, 3}, {1, 2}));
    CHECK(state.currentNoise() == 2);
  }
}

// TEST_CASE("Noise Calculation") {
//   TransactionList dataset;
//   PatternList patterns;
//   CHECK(dataset.calcNoise(patterns).first == 0);

//   dataset.addTransaction({0, 1, 2});
//   CHECK(dataset.calcNoise(patterns).first == 3);
//   dataset.addTransaction({0, 1, 5});
//   CHECK(dataset.calcNoise(patterns).first == 6);

//   Pattern p1;
//   p1.itemIds = {0, 1, 2};
//   p1.transactionIds = {0, 1};
//   patterns.addPattern(p1);
//   CHECK(dataset.calcNoise(patterns).first == 2);

//   dataset.addTransaction({0, 1, 2});
//   CHECK(dataset.calcNoise(patterns).first == 5);

//   dataset.addTransaction({7, 8, 9});
//   dataset.addTransaction({7, 8, 9});
//   dataset.addTransaction({5, 10, 11});
//   dataset.addTransaction({8, 9, 7});
//   dataset.addTransaction({7, 8, 9, 10});
//   CHECK(dataset.calcNoise(patterns).first == 21);

//   Pattern p2;
//   p2.itemIds = {9, 8, 7};
//   p2.transactionIds = {3, 4, 6, 7};
//   patterns.addPattern(p2);
//   CHECK(dataset.calcNoise(patterns).first == 9);

//   SUBCASE("Overlapping patterns") {
//     dataset.addTransaction({3, 4, 5});
//     dataset.addTransaction({3, 4, 5});
//     dataset.addTransaction({3, 4, 5, 6, 7, 8});
//     dataset.addTransaction({3, 4, 5, 6, 7, 8, 9});
//     CHECK(dataset.calcNoise(patterns).first == 28);

//     Pattern p3;
//     p3.itemIds = {3, 4, 5};
//     p3.transactionIds = {8, 9, 10, 11};
//     patterns.addPattern(p3);
//     CHECK(dataset.calcNoise(patterns).first == 16);

//     Pattern p4;
//     p4.itemIds = {6, 7, 8};
//     p4.transactionIds = {10, 11};
//     patterns.addPattern(p4);
//     CHECK(dataset.calcNoise(patterns).first == 10);
//   }
// }
