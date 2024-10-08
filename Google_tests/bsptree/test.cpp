#include <gtest/gtest.h>
#include <vector>
#include <random>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <unordered_set>
#include "datatype.h"
#include "line.h"
#include "plane.h"
#include "bsptree.h"

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<float> dis(0.0f, 1.0f);

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
        int numVertices = 3;

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

bool verifySubtreePolygons(BSPNode *node, const Plane &parentPlane,
                           bool shouldBeInFront,
                           std::unordered_set<const Polygon *> &verifiedPolygons) {
    if (!node) {
        return true;
    }

    for (const Polygon &polygon: node->getPolygons()) {
        if (verifiedPolygons.find(&polygon) != verifiedPolygons.end()) {
            continue;
        }

        RelationType relation = polygon.relationWithPlane(parentPlane);

        if (relation == RelationType::SPLIT) {
            std::cerr
                    << "Error: A polygon in the subtree is still marked as SPLIT."
                    << std::endl;
            return false;
        }

        if (shouldBeInFront) {
            if (relation != RelationType::IN_FRONT &&
                relation != RelationType::COINCIDENT) {
                std::cerr
                        << "Error: A polygon in the front subtree is not in front of the partition plane."
                        << std::endl;
                return false;
            }
        } else {
            if (relation != RelationType::BEHIND &&
                relation != RelationType::COINCIDENT) {
                std::cerr
                        << "Error: A polygon in the back subtree is not behind the partition plane."
                        << std::endl;
                return false;
            }
        }

        verifiedPolygons.insert(&polygon);
    }

    if (!verifySubtreePolygons(node->getFront(), parentPlane, shouldBeInFront,
                               verifiedPolygons)) {
        return false;
    }
    if (!verifySubtreePolygons(node->getBack(), parentPlane, shouldBeInFront,
                               verifiedPolygons)) {
        return false;
    }

    return true;
}

bool verifyBSPNode(BSPNode *node,
                   std::unordered_set<const Polygon *> &verifiedPolygons) {
    if (!node) {
        return true;
    }

    if (!node->getPolygons().empty()) {
        return true;
    }

    const Plane &partition = node->getPartition();

    for (const Polygon &polygon: node->getPolygons()) {
        RelationType relation = polygon.relationWithPlane(partition);
        if (relation != RelationType::COINCIDENT) {
            std::cerr
                    << "Error: A polygon stored in the node is not coplanar with its partition plane."
                    << std::endl;
            return false;
        }
        verifiedPolygons.insert(&polygon);
    }

    if (node->getFront()) {
        if (!verifySubtreePolygons(node->getFront(), partition, true,
                                   verifiedPolygons)) {
            return false;
        }
        if (!verifyBSPNode(node->getFront(), verifiedPolygons)) {
            return false;
        }
    }

    if (node->getBack()) {
        if (!verifySubtreePolygons(node->getBack(), partition, false,
                                   verifiedPolygons)) {
            return false;
        }
        if (!verifyBSPNode(node->getBack(), verifiedPolygons)) {
            return false;
        }
    }

    return true;
}

bool arePlanesEqual(const Plane &plane1, const Plane &plane2) {
    Vector3D normal1 = plane1.getNormal().unit();
    Vector3D normal2 = plane2.getNormal().unit();

    Vector3D crossProduct = normal1.crossProduct(normal2);
    if (crossProduct.mag() > 0) {
        return false;
    }

    Point3D pointInPlane1 = plane1.getPoint();
    Point3D pointInPlane2 = plane2.getPoint();
    Vector3D pointDifference = pointInPlane1 - pointInPlane2;
    NType dotProduct = normal1.dotProduct(pointDifference);
    if (abs(dotProduct) == 0) {
        return true;
    }

    return false;
}

bool
verifyUniquePartitions(BSPNode *node, std::vector<Plane> &usedPartitions) {
    if (!node) return true;

    if (!node->getPolygons().empty()) {
        return true;
    }

    Plane currentPartition = node->getPartition();

    for (const Plane &existingPartition: usedPartitions) {
        if (arePlanesEqual(currentPartition, existingPartition)) {
            std::cerr
                    << "Error: The partition plane has already been used in another node."
                    << std::endl;
            return false;
        }
    }

    usedPartitions.push_back(currentPartition);

    return verifyUniquePartitions(node->getFront(), usedPartitions) &&
           verifyUniquePartitions(node->getBack(), usedPartitions);
}

// Google Test Fixture
class BSPTreeTest : public ::testing::Test {
protected:
    BSPTree bspTree;
    std::unordered_set<const Polygon *> verifiedPolygons;
    std::vector<Plane> usedPartitions;

    virtual void SetUp() {
        int n_polygons = 200;
        int p_min = 0, p_max = 500;
        std::vector<Polygon> randomPolygons = generateRandomPolygons(
                n_polygons, p_min, p_max, p_min, p_max, p_min, p_max);
        for (const auto &polygon: randomPolygons) {
            bspTree.insert(polygon);
        }
    }
};

TEST_F(BSPTreeTest, TestBSPTreeValidity) {
    bool isValid = verifyBSPNode(bspTree.getRoot(), verifiedPolygons);
    ASSERT_TRUE(
            isValid) << "Error: Some polygons are not correctly located in the BSP-Tree.";
}

TEST_F(BSPTreeTest, TestUniquePartitions) {
    bool uniquePartitions = verifyUniquePartitions(bspTree.getRoot(),
                                                   usedPartitions);
    ASSERT_TRUE(uniquePartitions)
                                << "Error: There are repeated partition planes in the BSP-Tree.";
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}