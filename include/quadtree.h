#pragma once

#include <vector>
#include <memory>
#include <array>

#include "quadnode.h"

class QuadTree {
private:
    std::unique_ptr<QuadNode> root;

public:
    static size_t bucketSize;

    // Constructors
    QuadTree() = default;

    QuadTree(NType xmin, NType ymin, NType xmax, NType ymax,
             size_t _bucketSize)
            : root(std::make_unique<QuadNode>(
            Rect(Point2D(xmin, ymin), Point2D(xmax, ymax)))) {
        QuadTree::bucketSize = _bucketSize;
    }

    QuadTree(const Rect &boundary, size_t _bucketSize)
            : root(std::make_unique<QuadNode>(boundary)) {
        QuadTree::bucketSize = _bucketSize;
    }

    QuadTree(NType xmin, NType ymin, NType xmax, NType ymax)
            : root(std::make_unique<QuadNode>(
            Rect(Point2D(xmin, ymin), Point2D(xmax, ymax)))) {}

    QuadTree(const Rect &boundary)
            : root(std::make_unique<QuadNode>(boundary)) {}

    void insert(const std::vector<std::shared_ptr<Particle>> &particles);

    std::vector<Point2D> knn(const Point2D &queryPoint, size_t k);

    const std::unique_ptr<QuadNode> &getRoot() const { return root; }

    void updateTree();
};