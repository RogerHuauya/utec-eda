#include <gtest/gtest.h>
#include <vector>
#include <random>
#include <cmath>
#include <algorithm>
#include <iostream>
#include "datatype.h"
#include "line.h"
#include "plane.h"
#include "bsptree.h"


std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<float> dis(0.0f, 1.0f);

// Helper functions for generating random polygons
NType randomInRange(float min, float max) {
    return min + (max - min) * dis(gen);
}

Vector3D randomUnitVector() {
    NType theta = randomInRange(0, 2 * M_PI);
    NType phi = acos(randomInRange(-1.0f, 1.0f));
    NType x = sin(phi) * cos(theta);
    NType y = sin(phi) * sin(theta);
    NType z = cos(phi);
    return Vector3D(x, y, z);
}

Point3D randomPointInBox(float x_min, float x_max, float y_min, float y_max,
                         float z_min, float z_max) {
    NType x = randomInRange(x_min, x_max);
    NType y = randomInRange(y_min, y_max);
    NType z = randomInRange(z_min, z_max);
    return Point3D(x, y, z);
}

std::pair<Vector3D, Vector3D> generateOrthogonalVectors(const Vector3D &v) {
    Vector3D a = (abs(v.getX()) > abs(v.getZ())) ? Vector3D(v.getY(),
                                                            -v.getX(), 0)
                                                 : Vector3D(0, -v.getZ(),
                                                            v.getY());
    Vector3D b = v.crossProduct(a);
    a.normalize();
    b.normalize();
    return {a, b};
}

std::vector<Polygon>
generateRandomPolygons(int n, float x_min, float x_max, float y_min,
                       float y_max, float z_min, float z_max) {
    std::vector<Polygon> polygons;
    std::uniform_int_distribution<int> vertexCountDist(3, 6);
    std::uniform_real_distribution<float> angleDist(0, 2 * M_PI);
    std::uniform_real_distribution<float> radiusDist(0.5, 1.5);

    for (int i = 0; i < n; ++i) {
        Point3D P = randomPointInBox(x_min, x_max, y_min, y_max, z_min, z_max);
        Vector3D v = randomUnitVector();
        v.normalize();

        auto [u, w] = generateOrthogonalVectors(v);
        int numVertices = vertexCountDist(gen);

        std::vector<Point3D> vertices;
        NType angleIncrement = 2 * M_PI / numVertices;
        for (int j = 0; j < numVertices; ++j) {
            NType angle =
                    j * angleIncrement + angleDist(gen) * (angleIncrement / 4);
            NType radius = radiusDist(gen);

            NType scale_u = radius * cos(angle);
            NType scale_w = radius * sin(angle);

            Point3D vertex = P + u * scale_u + w * scale_w;
            vertices.push_back(vertex);
        }

        if (vertices.size() >= 3) {
            Polygon polygon(vertices);
            polygons.push_back(polygon);
        } else {
            --i;
        }
    }
    return polygons;
}

// Helper function to verify polygons in the BSP-Tree
bool
verifyPolygonsInNode(BSPNode *node, const std::vector<Polygon> &allPolygons) {
    if (!node) return true;

    for (const Polygon &polygon: node->getPolygons()) {
        RelationType relation = polygon.relationWithPlane(
                node->getPartition());
        if (relation != COINCIDENT) {
            std::cerr << "Error: Polygon not coplanar to partition plane."
                      << std::endl;
            return false;
        }
    }

    for (const Polygon &polygon: allPolygons) {
        RelationType relation = polygon.relationWithPlane(
                node->getPartition());
        if (relation == SPLIT) {
            std::pair<Polygon, Polygon> splitPolys = polygon.split(
                    node->getPartition());
            bool frontContains = node->getFront() && std::find(
                    node->getFront()->getPolygons().begin(),
                    node->getFront()->getPolygons().end(), splitPolys.first) !=
                                                     node->getFront()->getPolygons().end();
            bool backContains = node->getBack() && std::find(
                    node->getBack()->getPolygons().begin(),
                    node->getBack()->getPolygons().end(), splitPolys.second) !=
                                                   node->getBack()->getPolygons().end();

            if (!frontContains || !backContains) {
                std::cerr
                        << "Error: Split polygon not found in corresponding subtrees."
                        << std::endl;
                return false;
            }
        } else if (relation == IN_FRONT && node->getFront() &&
                   std::find(node->getFront()->getPolygons().begin(),
                             node->getFront()->getPolygons().end(), polygon) ==
                   node->getFront()->getPolygons().end()) {
            std::cerr
                    << "Error: Polygon is on the wrong side of the partition plane."
                    << std::endl;
            return false;
        } else if (relation == BEHIND && node->getBack() &&
                   std::find(node->getBack()->getPolygons().begin(),
                             node->getBack()->getPolygons().end(), polygon) ==
                   node->getBack()->getPolygons().end()) {
            std::cerr
                    << "Error: Polygon is on the wrong side of the partition plane."
                    << std::endl;
            return false;
        }
    }
    return verifyPolygonsInNode(node->getFront(), allPolygons) &&
           verifyPolygonsInNode(node->getBack(), allPolygons);
}

// Verify that partition planes are unique
bool arePlanesEqual(const Plane &plane1, const Plane &plane2) {
    Vector3D normal1 = plane1.getNormal();
    Vector3D normal2 = plane2.getNormal();

    Vector3D crossProduct = normal1.crossProduct(normal2);
    if (crossProduct.mag() != NType(0)) {
        return false;
    }

    Point3D pointInPlane1 = plane1.getPoint();
    Point3D pointInPlane2 = plane2.getPoint();
    Vector3D pointDifference = pointInPlane1 - pointInPlane2;

    NType dotProduct = normal2.dotProduct(pointDifference);
    return (abs(dotProduct) == NType(0));
}

bool
verifyUniquePartitions(BSPNode *node, std::vector<Plane> &usedPartitions) {
    if (!node) return true;

    Plane currentPartition = node->getPartition();
    for (const Plane &existingPartition: usedPartitions) {
        if (arePlanesEqual(currentPartition, existingPartition)) {
            std::cerr << "Error: Duplicate partition planes." << std::endl;
            return false;
        }
    }

    usedPartitions.push_back(currentPartition);
    return verifyUniquePartitions(node->getFront(), usedPartitions) &&
           verifyUniquePartitions(node->getBack(), usedPartitions);
}

// Verify non-empty nodes
bool verifyNonEmptyNodes(BSPNode *node) {
    if (!node) return true;

    if (node->getPolygons().empty()) {
        std::cerr << "Error: Node has no polygons." << std::endl;
        return false;
    }
    return verifyNonEmptyNodes(node->getFront()) &&
           verifyNonEmptyNodes(node->getBack());
}

// Test fixture
class BSPTreeTest : public ::testing::Test {
protected:
    BSPTree bspTree;
    std::vector<Polygon> randomPolygons;

    void SetUp() override {
        int n_polygons = 200;
        float p_min = 0, p_max = 500;
        randomPolygons = generateRandomPolygons(n_polygons, p_min, p_max,
                                                p_min, p_max, p_min, p_max);
        for (const auto &polygon: randomPolygons) {
            bspTree.insert(polygon);
        }
    }
};

// Google Test: Verify polygons are correctly placed in BSP-Tree
TEST_F(BSPTreeTest, PolygonsCorrectlyPlaced) {
    EXPECT_TRUE(verifyPolygonsInNode(bspTree.getRoot(), randomPolygons));
}

// Google Test: Verify partitions are unique
TEST_F(BSPTreeTest, UniquePartitions) {
    std::vector<Plane> usedPartitions;
    EXPECT_TRUE(verifyUniquePartitions(bspTree.getRoot(), usedPartitions));
}

// Google Test: Verify nodes are non-empty
TEST_F(BSPTreeTest, NonEmptyNodes) {
    EXPECT_TRUE(verifyNonEmptyNodes(bspTree.getRoot()));
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}