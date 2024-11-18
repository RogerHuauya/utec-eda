#pragma once

#include <vector>
#include <limits>
#include <algorithm>
#include <numeric>
#include "point.h"
#include "data.h"

class SSNode {
private:
    size_t maxPointsPerNode;
    Point centroid;
    float radius;
    bool isLeaf;
    SSNode *parent;
    std::vector<Data *> _data;

    // For searching
    SSNode *findClosestChild(const Point &target);

    size_t directionOfMaxVariance();

    SSNode *split();

    size_t findSplitIndex(size_t coordinateIndex);

    std::vector<Point> getEntriesCentroids();

    size_t minVarianceSplit(const std::vector<float> &values);

public:
    std::vector<SSNode *> children;

    SSNode(const Point &centroid, float radius = 0.0f, bool isLeaf = true,
           SSNode *parent = nullptr)
            : centroid(centroid), radius(radius), isLeaf(isLeaf),
              parent(parent) {}

    // Checks if a point is inside the bounding sphere
    bool intersectsPoint(const Point &point) const;

    // Getters
    const Point &getCentroid() const { return centroid; }

    float getRadius() const { return radius; }

    const std::vector<SSNode *> &getChildren() const { return children; }

    const std::vector<Data *> &getData() const { return _data; }

    bool getIsLeaf() const { return isLeaf; }

    SSNode *getParent() const { return parent; }

    // Insertion
    SSNode *searchParentLeaf(SSNode *node, const Point &target);

    SSNode *insert(SSNode *node, Data *_data);

    // Search
    SSNode *search(SSNode *node, Data *_data);

// For insertion
    void updateBoundingEnvelope();
};

class SSTree {
private:
    SSNode *root;
    size_t maxPointsPerNode;

public:
    SSTree(size_t maxPointsPerNode)
            : maxPointsPerNode(maxPointsPerNode), root(nullptr) {}

    SSTree() = default;

    void insert(Data *_data);

    SSNode *search(Data *_data);

    SSNode *getRoot() const { return root; }
};

