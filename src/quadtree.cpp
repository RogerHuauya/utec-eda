#include "quadtree.h"

size_t QuadTree::bucketSize = 6;

void
QuadTree::insert(const std::vector<std::shared_ptr<Particle>> &particles) {
    for (const auto &particle: particles) {
        root->insert(particle);
    }
}

std::vector<Point2D> QuadTree::knn(const Point2D &queryPoint, size_t k) {
    return root->knn(queryPoint, k);
}

void QuadTree::updateTree() {
    root->updateNode();
}
