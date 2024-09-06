#include <gtest/gtest.h>
#include <cassert>
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
std::uniform_real_distribution<> dis(-1.0, 1.0);

// Helper functions for generating random polygons
NType randomInRange(NType min, NType max) {
    return min + (max - min) * dis(gen);
}

Vector3D randomUnitVector() {
    NType x = randomInRange(-1.0, 1.0);
    NType y = randomInRange(-1.0, 1.0);
    NType z = randomInRange(-1.0, 1.0);
    Vector3D v(x, y, z);
    v.normalize();
    return v;
}

Point3D randomPointInBox(NType x_min, NType x_max, NType y_min, NType y_max, NType z_min, NType z_max) {
    NType x = randomInRange(x_min, x_max);
    NType y = randomInRange(y_min, y_max);
    NType z = randomInRange(z_min, z_max);
    return Point3D(x, y, z);
}

std::pair<Vector3D, Vector3D> generateOrthogonalVectors(const Vector3D& v) {
    Vector3D a = (abs(v.getX()) > abs(v.getZ())) ? Vector3D(v.getY(), -v.getX(), 0) : Vector3D(0, -v.getZ(), v.getY());
    Vector3D b = v.crossProduct(a);
    a.normalize();
    b.normalize();
    return {a, b};
}

std::vector<Polygon> generateRandomPolygons(int n, NType x_min, NType x_max, NType y_min, NType y_max, NType z_min, NType z_max) {
    std::vector<Polygon> polygons;

    for (int i = 0; i < n; ++i) {
        Point3D P = randomPointInBox(x_min, x_max, y_min, y_max, z_min, z_max);
        Vector3D v = randomUnitVector();
        auto [u, w] = generateOrthogonalVectors(v);

        Plane plane(P, v);
        std::vector<Point3D> vertices;
        vertices.push_back(P);

        int numExtraPoints = 2 + (rand() % 3);
        for (int j = 0; j < numExtraPoints; ++j) {
            NType scale_u = randomInRange(-2.0, 2.0);
            NType scale_w = randomInRange(-2.0, 2.0);
            Point3D extraPoint = P + u * scale_u + w * scale_w;
            vertices.push_back(extraPoint);
        }

        Polygon polygon(vertices);
        polygons.push_back(polygon);
    }
    return polygons;
}

// Helper function to verify polygons in the BSP-Tree
bool verifyPolygonsInNode(BSPNode* node, const std::vector<Polygon>& allPolygons) {
    if (!node) return true;

    for (const Polygon& polygon : node->polygons) {
        RelationType relation = polygon.relationWithPlane(node->partition);

        if (relation == SPLIT) {
            std::pair<Polygon, Polygon> splitPolys = polygon.split(node->partition);
            bool frontContains = node->front && std::find(node->front->polygons.begin(), node->front->polygons.end(), splitPolys.first) != node->front->polygons.end();
            bool backContains  = node->back && std::find(node->back->polygons.begin(), node->back->polygons.end(), splitPolys.second) != node->back->polygons.end();

            if (!frontContains || !backContains) {
                std::cerr << "Error: Polygon that should be split is not found in corresponding subtrees." << std::endl;
                return false;
            }
        } else if ((relation == IN_FRONT && node->front && std::find(node->front->polygons.begin(), node->front->polygons.end(), polygon) == node->front->polygons.end()) ||
                   (relation == BEHIND && node->back && std::find(node->back->polygons.begin(), node->back->polygons.end(), polygon) == node->back->polygons.end())) {
            std::cerr << "Error: Polygon is on the wrong side of the partition plane or missing." << std::endl;
            return false;
        }
    }
    return verifyPolygonsInNode(node->front, allPolygons) && verifyPolygonsInNode(node->back, allPolygons);
}

class BSPTreeTest : public ::testing::Test {
protected:
    BSPTree bspTree;
    std::vector<Polygon> randomPolygons;

    void SetUp() override {
        int n_polygons = 200;
        int p_min = 0, p_max = 500;
        randomPolygons = generateRandomPolygons(n_polygons, p_min, p_max, p_min, p_max, p_min, p_max);
        for (const auto& polygon : randomPolygons) {
            bspTree.insert(polygon);
        }
    }
};

TEST_F(BSPTreeTest, PolygonsCorrectlyPlaced) {
    EXPECT_TRUE(verifyPolygonsInNode(bspTree.getRoot(), randomPolygons));
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
