#include <gtest/gtest.h>
#include <unordered_set>
#include <vector>
#include "sstree.h"
#include "point.h"
#include "data.h"

std::vector<Data *> generateRandomData(size_t numPoints) {
  std::vector<Data *> data;
  for (size_t i = 0; i < numPoints; ++i) {
    Point embedding = Point::random();
    std::string imagePath = "eda_" + std::to_string(i) + ".jpg";
    Data *dataPoint = new Data(embedding, imagePath);
    data.push_back(dataPoint);
  }
  return data;
}

// General template for testing tree properties
template<typename Tree, typename Node, typename Predicate>
bool checkTreeProperty(const Tree &tree, Predicate predicate) {
  std::unordered_set<Node *> visited;
  return predicate(tree.getRoot(), visited);
}

// Specific predicates for your tests
bool
allDataPresentPredicate(SSNode *node, std::unordered_set<Data *> &dataSet) {
  if (node->getIsLeaf()) {
    for (const auto &data: node->getData()) {
      dataSet.erase(data);
    }
  } else {
    for (const auto &child: node->getChildren()) {
      allDataPresentPredicate(child, dataSet);
    }
  }
  return dataSet.empty();
}

bool leavesAtSameLevelPredicate(SSNode *node, int level, int &leafLevel) {
  if (node->getIsLeaf()) {
    if (leafLevel == -1) leafLevel = level;
    return leafLevel == level;
  }
  for (const auto &child: node->getChildren()) {
    if (!leavesAtSameLevelPredicate(child, level + 1, leafLevel)) return false;
  }
  return true;
}

bool noNodeExceedsMaxChildrenPredicate(SSNode *node, size_t maxPointsPerNode) {
  if (node->getChildren().size() > maxPointsPerNode) return false;
  for (const auto &child: node->getChildren()) {
    if (!noNodeExceedsMaxChildrenPredicate(child, maxPointsPerNode))
      return false;
  }
  return true;
}

// Test Fixture
class SSTreeTest : public ::testing::Test {
protected:
    SSTree tree;
    std::vector<Data *> data;

    virtual void SetUp() {
      constexpr size_t NUM_POINTS = 10000;
      constexpr size_t MAX_POINTS_PER_NODE = 20;
      data = generateRandomData(NUM_POINTS);
      tree = SSTree(MAX_POINTS_PER_NODE);

      for (const auto &d: data) {
        tree.insert(d);
      }
    }

    virtual void TearDown() {
      for (auto &d: data) {
        delete d;
      }
    }
};

// Google Test Cases
TEST_F(SSTreeTest, AllDataPresent) {
  std::unordered_set<Data *> dataSet(data.begin(), data.end());
  bool result = allDataPresentPredicate(tree.getRoot(), dataSet);
  ASSERT_TRUE(result) << "Error: Not all data is present in the tree.";
}

TEST_F(SSTreeTest, LeavesAtSameLevel) {
  int leafLevel = -1;
  bool result = leavesAtSameLevelPredicate(tree.getRoot(), 0, leafLevel);
  ASSERT_TRUE(result) << "Error: Leaves are not at the same level.";
}

TEST_F(SSTreeTest, NoNodeExceedsMaxChildren) {
  constexpr size_t MAX_POINTS_PER_NODE = 20;
  bool result = noNodeExceedsMaxChildrenPredicate(tree.getRoot(),
                                                  MAX_POINTS_PER_NODE);
  ASSERT_TRUE(
          result) << "Error: Some nodes exceed the maximum number of children.";
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}