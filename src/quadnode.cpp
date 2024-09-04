#include "quadnode.h"
#include "quadtree.h"

bool QuadNode::insert(const std::shared_ptr<Particle> &particle) {
    /**
     * Check if the particle is within the rect of this node.
     * If the particle is outside the rect, return false.
     */
    if (!boundary.contains(particle->getPosition())) {
        return false;
    }

    /**
     * If there is space in the current node's bucket, add the particle.
     * If the bucket is not full, simply add the particle and return true.
     */
    if (particles.size() < QuadTree::bucketSize) {
        addToBucket(particle);
        return true;
    }

    /**
     * If the node is a leaf and there's no space, subdivide the node.
     * Subdivision will create four children nodes.
     */
    if (_isLeaf) {
        subdivide();
    }

    /**
     * Propagate the particle to the appropriate child node.
     * The particle is attempted to be inserted into one of the children.
     */
    return propagate(particle);
}

void QuadNode::updateNode() {
    /**
     * This function would typically be used to update the state of the node,
     * such as recalculating bounds or updating particle positions.
     * The implementation is dependent on specific needs.
     */
}

void QuadNode::addToBucket(const std::shared_ptr<Particle> &particle) {
    /**
     * This function adds a particle to the current node's particle list.
     * It is assumed to be called after the particle has been propagated
     * or if the particle needs to remain in this node.
     */
    particles.push_back(particle);
}

bool QuadNode::propagate(const std::shared_ptr<Particle> &particle) {
    /**
     * This function attempts to insert the particle into one of the children nodes.
     * It is called if the current node is not a leaf.
     * If the particle is successfully inserted into a child node, return true.
     */
    for (auto &child: children) {
        if (child->insert(particle)) {
            return true;
        }
    }
    return false;
}

void QuadNode::subdivide() {
    /**
     * This function subdivides the current node into 4 child nodes
     * and reassigns the particles to the appropriate child node.
     * The current node is marked as non-leaf, and new boundaries
     * for the children are calculated based on the midpoint of the current node's rect.
     */

    _isLeaf = false;  // Mark the current node as non-leaf

    // Calculate midpoints
    auto xmin = boundary.getPmin().getX();
    auto ymin = boundary.getPmin().getY();
    auto xmax = boundary.getPmax().getX();
    auto ymax = boundary.getPmax().getY();
    auto xmid = (xmin + xmax) / 2;
    auto ymid = (ymin + ymax) / 2;

    /**
     * Create the rect regions for the four children.
     * Each child node is initialized with a new rect.
     */
    children[0] = std::make_unique<QuadNode>(
            Rect(Point2D(xmin, ymin), Point2D(xmid, ymid)), this);
    children[1] = std::make_unique<QuadNode>(
            Rect(Point2D(xmid, ymin), Point2D(xmax, ymid)), this);
    children[2] = std::make_unique<QuadNode>(
            Rect(Point2D(xmin, ymid), Point2D(xmid, ymax)), this);
    children[3] = std::make_unique<QuadNode>(
            Rect(Point2D(xmid, ymid), Point2D(xmax, ymax)), this);

    /**
     * Reassign particles to the appropriate child node.
     * After reassigning, clear the particle list in the current node.
     */
    for (const auto &particle: particles) {
        propagate(particle);
    }

    // Clear particles from the current node after reassignment
    particles.clear();
}

void QuadNode::relocateParticle(const std::shared_ptr<Particle> &particle) {
    /**
     * If a particle has moved, this function should find its new location
     * and move it to the correct node.
     * It first removes the particle from the current node,
     * and then attempts to insert it into the appropriate node.
     */

    // Remove the particle from the current node
    auto it = std::remove(particles.begin(), particles.end(), particle);
    if (it != particles.end()) {
        particles.erase(it, particles.end());

        /**
         * Insert the particle into the correct node.
         * If the insertion fails, handle it accordingly,
         * potentially by moving it to the root or discarding it.
         */
        if (!insert(particle)) {
            // If the insertion fails, handle it (e.g., out of bounds)
        }
    }
}

void QuadNode::removeEmptyNode(QuadNode *emptyChild) {
    /**
     * This function removes a child node if it becomes empty
     * after particles have moved away.
     * It searches for the empty child, resets the corresponding pointer,
     * and optionally checks if all children are empty to collapse the node.
     */
    for (auto &child: children) {
        if (child.get() == emptyChild) {
            child.reset();  // Set the child pointer to nullptr
            break;
        }
    }

    /**
     * Optional: Check if all children are empty and collapse the node.
     * If all children are empty, revert the current node back to a leaf node.
     * Instead of using fill, manually reset each child to nullptr.
     */
    bool allEmpty = std::all_of(children.begin(), children.end(),
                                [](const auto &child) { return !child; });
    if (allEmpty) {
        _isLeaf = true;  // Revert back to a leaf node
        for (auto &child: children) {
            child.reset();  // Clear each child
        }
    }
}