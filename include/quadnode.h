#pragma once

#include "particle.h"

#include <vector>
#include <iostream>
#include <set>
#include <random>
#include <vector>

class QuadNode {
private:
    std::vector<std::shared_ptr<Particle>> particles; // Bucket size constraint
    std::array<std::unique_ptr<QuadNode>, 4> children; // NW, NE, SW, SE
    Rect boundary;
    QuadNode *parent;
    bool _isLeaf;

    void addToBucket(const std::shared_ptr<Particle> &particle);

    bool propagate(const std::shared_ptr<Particle> &particle);

    void subdivide();

    void relocateParticle(
            const std::shared_ptr<Particle> &particle); // cuando se mueve la particula

    void removeEmptyNode(QuadNode *emptyChild);

public:
    QuadNode(NType xmin, NType ymin, NType xmax, NType ymax,
             QuadNode *parent = nullptr)
            : boundary(Point2D(xmin, ymin), Point2D(xmax, ymax)),
              parent(parent), _isLeaf(true) {}

    explicit QuadNode(const Rect &boundary, QuadNode *parent = nullptr)
            : boundary(boundary), parent(parent), _isLeaf(true) {}

    bool insert(const std::shared_ptr<Particle> &particle);
    
    void updateNode();

    // Getters
    const std::vector<std::shared_ptr<Particle>> &
    getParticles() const { return particles; }

    const std::unique_ptr<QuadNode> &
    getChild(size_t index) const { return children[index]; }

    const std::array<std::unique_ptr<QuadNode>, 4> &
    getChildren() const { return children; }

    const Rect &getBoundary() const { return boundary; }

    const QuadNode *getParent() const { return parent; }

    // Setters
    void setParent(QuadNode *_parent) { this->parent = _parent; }

    bool isLeaf() const { return _isLeaf; }
};
