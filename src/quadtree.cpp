#include <queue>
#include <cmath>
#include <iostream>
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
    // Priority queue for best-first search: stores nodes or particles with distances
    using PQElement = std::pair<NType, std::shared_ptr<Particle>>;
    auto compareParticles = [](const PQElement &a, const PQElement &b) {
        return a.first <
               b.first;  // Max heap, we want to keep the farthest in the top
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

        // If it's a leaf node, iterate over the particles
        if (currentNode->isLeaf()) {
            for (const auto &particle: currentNode->getParticles()) {
                NType dist = queryPoint.distance(particle->getPosition());
                if (pq.size() < k) {
                    // Add to queue if we haven't found k neighbors yet
                    pq.emplace(dist, particle);
                } else if (dist < pq.top().first) {
                    // If current particle is closer than the farthest one in the queue, replace it
                    pq.pop();
                    pq.emplace(dist, particle);
                }
            }
        } else {
            // Otherwise, propagate the search to child nodes
            for (const auto &child: currentNode->getChildren()) {
                if (child) {
                    NType childDist = minDistToRect(queryPoint,
                                                    child->getBoundary());
                    searchQueue.emplace(childDist, child.get());
                }
            }
        }
    }


    // Collect the k nearest particles from the priority queue
    std::vector<std::shared_ptr<Particle>> result;
    while (!pq.empty()) {
        result.push_back(pq.top().second);
        pq.pop();
    }

    // Reverse the result to get them in increasing order of distance
    std::reverse(result.begin(), result.end());

    return result;
}

void QuadTree::updateTree() {
    root->updateNode();
}
