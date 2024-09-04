#include <queue>
#include <cmath>
#include <iostream>
#include <set>
#include "quadtree.h"

size_t QuadTree::bucketSize = 6;


void
QuadTree::insert(const std::vector<std::shared_ptr<Particle>> &particles) {
    for (const auto &particle: particles) {
        root->insert(particle);
    }
}


std::vector<std::shared_ptr<Particle>>
QuadTree::knn(const Point2D &queryPoint, size_t k) {
    using PQElement = std::pair<NType, std::shared_ptr<Particle>>;
    auto compareParticles = [](const PQElement &a, const PQElement &b) {
        return a.first <
               b.first;
    };
    std::priority_queue<PQElement, std::vector<PQElement>, decltype(compareParticles)> pq(
            compareParticles);

    // Priority queue for best-first search of nodes
    using SearchNode = std::pair<NType, QuadNode *>;
    auto compareNodes = [](const SearchNode &a, const SearchNode &b) {
        return a.first > b.first;  // Min heap, closer nodes should come first
    };
    std::priority_queue<SearchNode, std::vector<SearchNode>, decltype(compareNodes)> searchQueue(
            compareNodes);

    // Start the search with the root node
    searchQueue.emplace(0.0, root.get());
    while (!searchQueue.empty()) {
        // Get the node with the smallest distance
        auto [nodeDist, currentNode] = searchQueue.top();
        searchQueue.pop();

        if (!currentNode) continue;  // Skip null nodes

        if (currentNode->isLeaf()) {
            for (const auto &particle: currentNode->getParticles()) {
                NType dist = queryPoint.distance(particle->getPosition());
                if (pq.size() < k) {
                    pq.emplace(dist, particle);
                } else if (dist < pq.top().first) {
                    pq.pop();
                    pq.emplace(dist, particle);
                }
            }
        } else {
            for (const auto &child: currentNode->getChildren()) {
                if (child) {
                    NType childDist = minDistToRect(queryPoint,
                                                    child->getBoundary());
                    searchQueue.emplace(childDist, child.get());
                }
            }
        }
    }


    std::vector<std::shared_ptr<Particle>> result;
    while (!pq.empty()) {
        result.push_back(pq.top().second);
        pq.pop();
    }

    std::reverse(result.begin(), result.end());

    return result;
}

void QuadTree::updateTree() {
    std::set<std::shared_ptr<Particle>> particlesToRelocate;
    root->updateNode();
}
