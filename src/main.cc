#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <queue>
#include <set>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "args.hxx"
#include "panda.h"

// Read transactions from a file given path, line by line and space separated
TransactionList<int> readIntTransactions(const std::string &path) {
  TransactionList<int> result;

  std::ifstream infile(path);
  std::string line;
  while (std::getline(infile, line)) {
    std::stringstream ss(line);
    int val;
    std::vector<int> items;
    // std::set<int> items;

    while (ss >> val) {
      items.push_back(val);
      // items.insert(val);
      if (ss.peek() == ' ') ss.ignore();
    }

    result.addTransaction(items);
  }

  return result;
}

int main(int argc, char *argv[]) {
  int kPatterns = 8;
  float complexityWeight = 0.8;
  float maxRowNoise = 1.0;
  float maxColumnNoise = 1.0;

  args::ArgumentParser parser(
      "Finds approximate patterns in datasets with noise.");
  args::HelpFlag help(parser, "help", "Display this help menu", {'h', "help"});
  args::ValueFlag<int> kPatternsParam(parser, "8", "Max number of patterns",
                                      {'k'});
  args::ValueFlag<float> complexityWeightParam(
      parser, "0.8", "Pattern complexity weight", {'w'});
  args::ValueFlag<float> maxRowNoiseParam(parser, "1.0", "Row tollerance ratio",
                                          {'y'});
  args::ValueFlag<float> maxColumnNoiseParam(parser, "1.0",
                                             "Column tollerance ratio", {'y'});
  args::Positional<std::string> filename(parser, "filename",
                                         "The dataset to process");
  try {
    parser.ParseCLI(argc, argv);
  } catch (const args::Help &) {
    std::cout << parser;
    return 0;
  } catch (const args::ParseError &e) {
    std::cerr << e.what() << std::endl;
    std::cerr << parser;
    return 1;
  } catch (args::ValidationError e) {
    std::cerr << e.what() << std::endl;
    std::cerr << parser;
    return 1;
  }

  if (kPatternsParam) kPatterns = args::get(kPatternsParam);
  if (complexityWeightParam)
    complexityWeight = args::get(complexityWeightParam);
  if (maxRowNoiseParam) maxRowNoise = args::get(maxRowNoiseParam);
  if (maxColumnNoiseParam) maxColumnNoise = args::get(maxColumnNoiseParam);

  if (!filename) {
    std::cout << parser;
    return 1;
  }

  auto dataset = readIntTransactions(args::get(filename));

  auto patterns =
      panda(kPatterns, dataset, maxRowNoise, maxColumnNoise, complexityWeight);

  std::cout << "Patterns:" << std::endl;
  for (auto &&p : patterns.patterns) {
    std::vector<int> items(p.itemIds.begin(), p.itemIds.end());
    std::sort(items.begin(), items.end());
    for (auto &&i : items) {
      std::cout << i << " ";
    }
    // std::cout << "(";
    // for (auto &&i : p.transactionIds) {
    //   std::cout << i << " ";
    // }
    // std::cout << ")";
    std::cout << " (" << p.transactionIds.size() << ")";
    std::cout << std::endl;
  }

  // std::cout << "Total cost: " << state.currentCost(complexityWeight)
  //           << std::endl;
  // std::cout << "Total noise: " << state.currentNoise() << std::endl;
  // std::cout << "Total false positives: " << state.currentFalsePositives
  //           << std::endl;
  // std::cout << "Total false negatives: " << state.residualDataset.elCount
  //           << std::endl;
  // std::cout << "Total complexity: " << state.patterns.complexity <<
  // std::endl;

  return 0;
}
