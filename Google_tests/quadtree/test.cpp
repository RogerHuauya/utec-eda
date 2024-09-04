#include <gtest/gtest.h>
#include <iostream>
#include <set>
#include <random>
#include <vector>
#include "quadtree.h"

void printHello() {
    std::cout << "Hello, World!\n";
}

TEST(HelloWorldTest, PrintHello) {
    testing::internal::CaptureStdout();
    printHello();
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_EQ(output, "Hello, World!\n");
}


std::vector<std::shared_ptr<Particle>>
generateRandomParticles(int n, const Rect &boundary,
                        NType maxVelocityMagnitude) {
    std::vector<std::shared_ptr<Particle>> particles;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> posDistX(
            boundary.getPmin().getX().getValue(),
            boundary.getPmax().getX().getValue());
    std::uniform_real_distribution<float> posDistY(
            boundary.getPmin().getY().getValue(),
            boundary.getPmax().getY().getValue());
    std::uniform_real_distribution<float> velDist(
            -maxVelocityMagnitude.getValue(), maxVelocityMagnitude.getValue());

    for (int i = 0; i < n; ++i) {
        NType x = NType(posDistX(gen));
        NType y = NType(posDistY(gen));
        Point2D position(x, y);

        NType vx = NType(velDist(gen));
        NType vy = NType(velDist(gen));
        Point2D velocity(vx, vy);

        auto particle = std::make_shared<Particle>(position, velocity);
        particles.push_back(particle);
    }

    return particles;
}

void traverseTree(QuadNode *node,
                  std::set<std::shared_ptr<Particle>> &foundParticles) {
    if (node->isLeaf()) {
        for (const auto &particle: node->getParticles()) {
            foundParticles.insert(particle);
        }
    } else {
        for (const auto &child: node->getChildren()) {
            if (child) {
                traverseTree(child.get(), foundParticles);
            }
        }
    }
}

bool verifyAllDataIndexed(QuadNode *rootNode,
                          const std::set<std::shared_ptr<Particle>> &insertedParticles) {
    std::set<std::shared_ptr<Particle>> foundParticles;
    traverseTree(rootNode, foundParticles);
    return foundParticles == insertedParticles;
}

bool traverseAndCheckInternalNodes(QuadNode *node) {
    if (!node->isLeaf()) {
        for (const auto &child: node->getChildren()) {
            if (child && node->isLeaf()) {
                return false;
            }
        }
        for (const auto &child: node->getChildren()) {
            if (child) {
                if (!traverseAndCheckInternalNodes(child.get())) {
                    return false;
                }
            }
        }
    }
    return true;
}

bool verifyInternalNodesNotLeaf(QuadNode *rootNode) {
    return traverseAndCheckInternalNodes(rootNode);
}

bool traverseAndCheckLeafNodes(QuadNode *node) {
    if (node->isLeaf()) {
        for (const auto &child: node->getChildren()) {
            if (child) {
                return false;
            }
        }
    } else {
        for (const auto &child: node->getChildren()) {
            if (child) {
                if (!traverseAndCheckLeafNodes(child.get())) {
                    return false;
                }
            }
        }
    }
    return true;
}

bool verifyLeafNodesHaveNoChildren(QuadNode *rootNode) {
    return traverseAndCheckLeafNodes(rootNode);
}

bool traverseAndCheckBucketSize(QuadNode *node, size_t bucketSize) {
    if (node->isLeaf()) {
        if (node->getParticles().size() > bucketSize) {
            return false;
        }
    } else {
        for (const auto &child: node->getChildren()) {
            if (child) {
                if (!traverseAndCheckBucketSize(child.get(), bucketSize)) {
                    return false;
                }
            }
        }
    }
    return true;
}

bool verifyLeafNodesBucketSize(QuadNode *rootNode, size_t bucketSize) {
    return traverseAndCheckBucketSize(rootNode, bucketSize);
}

bool traverseAndCheckBoundaries(QuadNode *node) {
    if (!node->isLeaf()) {
        for (const auto &child: node->getChildren()) {
            if (child && !child->getBoundary().isWithin(node->getBoundary())) {
                return false;
            }
        }
        for (const auto &child: node->getChildren()) {
            if (child) {
                if (!traverseAndCheckBoundaries(child.get())) {
                    return false;
                }
            }
        }
    }
    return true;
}

bool verifyChildBoundariesWithinParent(QuadNode *rootNode) {
    return traverseAndCheckBoundaries(rootNode);
}

bool traverseAndCheckNoIntersections(QuadNode *node) {
    if (!node->isLeaf()) {
        for (int i = 0; i < 4; ++i) {
            for (int j = i + 1; j < 4; ++j) {
                if (node->getChild(i) && node->getChild(j)) {
                    if (node->getChild(i)->getBoundary().intersects(
                            node->getChild(j)->getBoundary())) {
                        return false;
                    }
                }
            }
        }
        for (const auto &child: node->getChildren()) {
            if (child) {
                if (!traverseAndCheckNoIntersections(child.get())) {
                    return false;
                }
            }
        }
    }
    return true;
}

bool verifyNoIntersectingChildBoundaries(QuadNode *rootNode) {
    return traverseAndCheckNoIntersections(rootNode);
}

bool traverseAndCheckParticlesInCorrectLeaf(QuadNode *node) {
    if (node->isLeaf()) {
        for (const auto &particle: node->getParticles()) {
            if (!node->getBoundary().contains(particle->getPosition())) {
                return false;
            }
        }
    } else {
        for (const auto &child: node->getChildren()) {
            if (child) {
                if (!traverseAndCheckParticlesInCorrectLeaf(child.get())) {
                    return false;
                }
            }
        }
    }
    return true;
}

bool verifyParticlesInCorrectLeaf(QuadNode *rootNode) {
    return traverseAndCheckParticlesInCorrectLeaf(rootNode);
}


bool verifyKNN(QuadTree& tree, const std::vector<std::shared_ptr<Particle>>& particles, const Rect& boundary) {
    // Generar un punto de consulta aleatorio dentro del boundary
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> posDistX(boundary.getPmin().getX().getValue(), boundary.getPmax().getX().getValue());
    std::uniform_real_distribution<float> posDistY(boundary.getPmin().getY().getValue(), boundary.getPmax().getY().getValue());

    NType queryX = NType(posDistX(gen));
    NType queryY = NType(posDistY(gen));
    Point2D queryPoint(queryX, queryY);

    // Elegir un k aleatorio
    size_t k = std::uniform_int_distribution<size_t>(1, 10)(gen);

    // Obtener k-NN usando QuadTree
    std::vector<std::shared_ptr<Particle>> knnTree = tree.knn(queryPoint, k);

    // Obtener k-NN usando fuerza bruta
    std::vector<std::shared_ptr<Particle>> knnBruteForce = particles;
    std::sort(knnBruteForce.begin(), knnBruteForce.end(), [&queryPoint](const std::shared_ptr<Particle>& a, const std::shared_ptr<Particle>& b) {
        return queryPoint.distance(a->getPosition()) < queryPoint.distance(b->getPosition());
    });
    knnBruteForce.resize(k); // Seleccionar los primeros k vecinos más cercanos

    // Verificar si ambos resultados son equivalentes y en el mismo orden
    if (knnTree.size() != knnBruteForce.size()) {
        return false;
    }
    for (size_t i = 0; i < k; ++i) {
        if (knnTree[i] != knnBruteForce[i]) {
            return false;
        }
    }

    return true;
}


class QuadTreeTest : public ::testing::Test {
protected:
    Rect boundary;
    QuadTree tree;
    std::vector<std::shared_ptr<Particle>> particles;

    void SetUp() override {
        boundary = Rect(Point2D(0, 0), Point2D(100, 100));
        tree = QuadTree(boundary);
        int numParticles = 200000;
        NType maxVelocity = 5.0;
        particles = generateRandomParticles(numParticles, boundary,
                                            maxVelocity);
        tree.insert(particles);
    }
};

TEST_F(QuadTreeTest, AllDataIndexed) {
    EXPECT_TRUE(verifyAllDataIndexed(tree.getRoot().get(),
                                     {particles.begin(), particles.end()}));
}

TEST_F(QuadTreeTest, InternalNodesNotLeaf) {
    EXPECT_TRUE(verifyInternalNodesNotLeaf(tree.getRoot().get()));
}

TEST_F(QuadTreeTest, LeafNodesHaveNoChildren) {
    EXPECT_TRUE(verifyLeafNodesHaveNoChildren(tree.getRoot().get()));
}

TEST_F(QuadTreeTest, LeafNodesBucketSize) {
    EXPECT_TRUE(verifyLeafNodesBucketSize(tree.getRoot().get(),
                                          QuadTree::bucketSize));
}

TEST_F(QuadTreeTest, ChildBoundariesWithinParent) {
    EXPECT_TRUE(verifyChildBoundariesWithinParent(tree.getRoot().get()));
}

TEST_F(QuadTreeTest, NoIntersectingChildBoundaries) {
    EXPECT_TRUE(verifyNoIntersectingChildBoundaries(tree.getRoot().get()));
}

TEST_F(QuadTreeTest, ParticlesInCorrectLeaf) {
    EXPECT_TRUE(verifyParticlesInCorrectLeaf(tree.getRoot().get()));
}

TEST_F(QuadTreeTest, KnnSearch) {
    EXPECT_TRUE(verifyKNN(tree, particles, boundary));
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}