#pragma once
#include "particle.h"
#include <vector>


class QuadNode {
    std::vector<Particle> particles;
    QuadNode *children[4];
    Rect limits;
    QuadNode *parent = nullptr;
    bool is_leaf = false;

public:
    QuadNode();
    void insert(Particle& p);
    void updateNode();
};
