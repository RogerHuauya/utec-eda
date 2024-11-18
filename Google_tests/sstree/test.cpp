#include <gtest/gtest.h>
#include <vector>
#include <unordered_set>
#include <random>
#include "point.h"
#include "data.h"
#include "sstree.h"

constexpr size_t NUM_POINTS = 100;
constexpr size_t MAX_POINTS_PER_NODE = 20;

/*
 * Helper Functions
 */

// Generates random data points for testing
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

// Collects data from the tree using DFS
void collectDataDFS(SSNode *node, std::unordered_set<Data *> &treeData) {
  if (node->getIsLeaf()) {
    for (const auto &d: node->getData()) {
      treeData.insert(d);
    }
  } else {
    for (const auto &child: node->getChildren()) {
      collectDataDFS(child, treeData);
    }
  }
}

// Helper function to check if all leaves are at the same level
bool leavesAtSameLevelDFS(SSNode *node, int level, int &leafLevel) {
  if (node->getIsLeaf()) {
    if (leafLevel == -1) leafLevel = level;
    return leafLevel == level;
  }
  for (const auto &child: node->getChildren()) {
    if (!leavesAtSameLevelDFS(child, level + 1, leafLevel)) return false;
  }
  return true;
}

// Helper function to check if no node exceeds the maximum number of children
bool noNodeExceedsMaxChildrenDFS(SSNode *node, size_t maxPointsPerNode) {
  if (node->getChildren().size() > maxPointsPerNode) return false;
  for (const auto &child: node->getChildren()) {
    if (!noNodeExceedsMaxChildrenDFS(child, maxPointsPerNode)) return false;
  }
  return true;
}

// Helper function to check if all points are inside the bounding sphere of their respective nodes
bool sphereCoversAllPointsDFS(SSNode *node) {
  if (!node->getIsLeaf()) return true;
  const Point &centroid = node->getCentroid();
  float radius = node->getRadius();
  for (const auto &data: node->getData()) {
    if (Point::distance(centroid, data->getEmbedding()) > radius) return false;
  }
  return true;
}

bool dfsSphereCoversAllPoints(SSNode *node) {
  if (node->getIsLeaf()) {
    return sphereCoversAllPointsDFS(node);
  } else {
    for (const auto &child: node->getChildren()) {
      if (!dfsSphereCoversAllPoints(child)) return false;
    }
  }
  return true;
}

// Runs a DFS to check if all points are covered by their node's bounding sphere
bool sphereCoversAllPoints(SSNode *root) {
  return dfsSphereCoversAllPoints(root);
}

// Helper function to check if all children are inside the bounding sphere of their parent node
bool sphereCoversAllChildrenSpheresDFS(SSNode *node) {
  if (node->getIsLeaf()) return true;
  const Point &centroid = node->getCentroid();
  float radius = node->getRadius();
  for (const auto &child: node->getChildren()) {
    const Point &childCentroid = child->getCentroid();
    float childRadius = child->getRadius();
    if (Point::distance(centroid, childCentroid) + childRadius > radius)
      return false;
  }
  return true;
}

bool dfsSphereCoversAllChildrenSpheres(SSNode *node) {
  if (!sphereCoversAllChildrenSpheresDFS(node)) return false;
  for (const auto &child: node->getChildren()) {
    if (!dfsSphereCoversAllChildrenSpheres(child)) return false;
  }
  return true;
}


// Runs a DFS to check if all children spheres are covered by their parent node's sphere
bool sphereCoversAllChildrenSpheres(SSNode *root) {
  return dfsSphereCoversAllChildrenSpheres(root);
}


/*
 * Google Test Fixture
 */
class SSTreeTest : public ::testing::Test {
protected:
    SSTree tree{MAX_POINTS_PER_NODE};
    std::vector<Data *> data;

    void SetUp() override {
      data = generateRandomData(NUM_POINTS);
      for (const auto &d: data) {
        tree.insert(d);
      }
    }

    void TearDown() override {
      // Clean up allocated memory
      for (auto &d: data) {
        delete d;
      }
    }
};

/*
 * Test Cases Using the Fixture
 */

// Test 1: Check if all data is present in the tree
TEST_F(SSTreeTest, AllDataPresent) {
  std::unordered_set<Data *> dataSet(data.begin(), data.end());
  std::unordered_set<Data *> treeData;
  collectDataDFS(tree.getRoot(), treeData);

  for (const auto &d: dataSet) {
    EXPECT_TRUE(treeData.find(d) != treeData.end());
  }

  for (const auto &d: treeData) {
    EXPECT_TRUE(dataSet.find(d) != dataSet.end());
  }
}

// Test 2: Check if all leaves are at the same level
TEST_F(SSTreeTest, LeavesAtSameLevel) {
  int leafLevel = -1;
  EXPECT_TRUE(leavesAtSameLevelDFS(tree.getRoot(), 0, leafLevel));
}

// Test 3: Check if no node exceeds the maximum number of children
TEST_F(SSTreeTest, NoNodeExceedsMaxChildren) {
  EXPECT_TRUE(
          noNodeExceedsMaxChildrenDFS(tree.getRoot(), MAX_POINTS_PER_NODE));
}

// Test 4: Check if all points are inside the bounding sphere of their respective nodes
TEST_F(SSTreeTest, SphereCoversAllPoints) {
  EXPECT_TRUE(sphereCoversAllPoints(tree.getRoot()));
}

// Test 5: Check if all children are inside the bounding sphere of their parent node
TEST_F(SSTreeTest, SphereCoversAllChildrenSpheres) {
  EXPECT_TRUE(sphereCoversAllChildrenSpheres(tree.getRoot()));
}

/*
 * Main Function for Google Test
 */
int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}