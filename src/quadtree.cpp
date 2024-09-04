#include <queue>
#include <cmath>

#include "quadtree.h"

size_t QuadTree::bucketSize = 6;

NType squaredDistance(const Point2D &a, const Point2D &b) {
    NType dx = a.getX() - b.getX();
    NType dy = a.getY() - b.getY();
    return dx * dx + dy * dy;
}

// Helper function to calculate the minimum distance from a point to a rectangle
NType minDistToRect(const Point2D &p, const Rect &rect) {
    NType dx = std::max(rect.getPmin().getX() - p.getX(), NType(0));
    dx = std::max(dx, p.getX() - rect.getPmax().getX());

    NType dy = std::max(rect.getPmin().getY() - p.getY(), NType(0));
    dy = std::max(dy, p.getY() - rect.getPmax().getY());

    return dx * dx + dy * dy;
}

void QuadTree::insert(const std::vector<std::shared_ptr<Particle>> &particles) {
    for (const auto &particle : particles) {
        root->insert(particle);
    }
}

std::vector<std::shared_ptr<Particle>> QuadTree::knn(const Point2D &queryPoint, size_t k) {
    /** This function returns the k-nearest neighbors to the query point.
     * The implementation uses the technique best first search by
     * using a priority queue to store the nodes to be visited.
     */
    struct QueueEntry {
        QuadNode *node;
        NType distance;

        bool operator<(const QueueEntry &other) const {
            return distance > other.distance;  // Min-heap based on distance
        }
    };

    std::priority_queue<QueueEntry> queue;
    std::priority_queue<NType> bestDistances; // Max-heap to keep track of the k-nearest distances
    std::vector<std::shared_ptr<Particle>> nearestNeighbors;

    if (!root) {
        return nearestNeighbors;  // Return an empty vector if the root is null
    }

    queue.push({root.get(), minDistToRect(queryPoint, root->getBoundary())});

    while (!queue.empty()) {
        auto current = queue.top();
        queue.pop();

        if (current.node->isLeaf()) {
            // For leaf nodes, check all particles
            for (const auto &particle : current.node->getParticles()) {
                NType dist = squaredDistance(queryPoint, particle->getPosition());
                if (bestDistances.size() < k || dist < bestDistances.top()) {
                    if (bestDistances.size() == k) {
                        bestDistances.pop();
                    }
                    bestDistances.push(dist);
                    nearestNeighbors.push_back(particle);
                }
            }
        } else {
            // For non-leaf nodes, add all children to the queue
            for (const auto &child : current.node->getChildren()) {
                if (child) {
                    NType dist = minDistToRect(queryPoint, child->getBoundary());
                    if (bestDistances.size() < k || dist < bestDistances.top()) {
                        queue.push({child.get(), dist});
                    }
                }
            }
        }
    }

    // Trim the neighbors list to size k (in case there are more than k)
    if (nearestNeighbors.size() > k) {
        std::nth_element(nearestNeighbors.begin(), nearestNeighbors.begin() + k, nearestNeighbors.end(),
                         [&](const std::shared_ptr<Particle> &a, const std::shared_ptr<Particle> &b) {
                             return squaredDistance(queryPoint, a->getPosition()) < squaredDistance(queryPoint, b->getPosition());
                         });
        nearestNeighbors.resize(k);
    }

    return nearestNeighbors;
}

void QuadTree::updateTree() {
    root->updateNode();
}
