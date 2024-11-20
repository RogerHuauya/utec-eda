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
    SSNode *parent;
    std::vector<Data *> _data;

    // For searching
    SSNode *findClosestChild(const Point &target);

    size_t directionOfMaxVariance();

    std::pair<SSNode *, SSNode *> split();

    size_t findSplitIndex(size_t coordinateIndex);

    std::vector<Point> getEntriesCentroids();

    size_t minVarianceSplit(const std::vector<float> &values);

public:
    bool isLeaf;

    std::vector<SSNode *> children;

    SSNode(const Point &centroid, float radius = 0.0f, bool isLeaf = true,
           SSNode *parent = nullptr, size_t maxPointsPerNode = 20)
            : centroid(centroid), radius(radius), isLeaf(isLeaf),
              parent(parent), maxPointsPerNode(maxPointsPerNode) {}

    // Checks if a point is inside the bounding sphere
    bool intersectsPoint(const Point &point) const;

    float computeMeanForDimension(std::vector<Point> &centroids, size_t i);

    // Getters
    const Point &getCentroid() const { return centroid; }

    float getRadius() const { return radius; }

    const std::vector<SSNode *> &getChildren() const { return children; }

    const std::vector<Data *> &getData() const { return _data; }

    bool getIsLeaf() const { return isLeaf; }

    SSNode *getParent() const { return parent; }

    // Insertion
    SSNode *searchParentLeaf(SSNode *node, const Point &target);

    std::pair<SSNode *, SSNode *> insert(SSNode *node, Data *_data);

    // Search
    SSNode *search(SSNode *node, Data *_data);

    void knn(SSNode *&node, Point &query, size_t k,
             std::priority_queue<std::pair<float, Data *>> &max_heap);

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

    std::vector<Data *> knn(Point &query, size_t k);
};

