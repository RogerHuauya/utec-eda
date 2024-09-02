#pragma once
#include "quadnode.h"
#include <vector>

class QuadTree {
    QuadNode *root;

public:
    static size_t bucket_size;

    void insert(std::vector<Particle> &particles);

    void insert(Particle &particle);

    void updateTree();
    std::vector<Particle> knn(Point2D query, size_t k);
};
