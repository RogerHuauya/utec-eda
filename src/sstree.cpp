#include "sstree.h"

/**
 * intersectsPoint
 * Verifies if a point lies inside the bounding sphere of the node.
 * @param point: The point to check.
 * @return bool: Returns true if the point is within the sphere, false otherwise.
 */
bool SSNode::intersectsPoint(const Point &point) const {
  return (centroid - point).norm() <= radius;
}

/**
 * findClosestChild
 * Finds the closest child to a given point.
 * @param target: The target point to find the nearest child.
 * @return SSNode*: Returns a pointer to the closest child.
 */
SSNode *SSNode::findClosestChild(const Point &target) {
  if (children.empty()) {
    return nullptr; // Early exit if there are no children
  }

  SSNode *closestChild = nullptr;
  float minDistance = std::numeric_limits<float>::max();

  for (SSNode *child: children) {
    float dist = (child->centroid - target).norm();
    if (dist < minDistance) {
      minDistance = dist;
      closestChild = child;
    }
  }

  return closestChild;
}

/**
 * updateBoundingEnvelope
 * Updates the centroid and radius of the node based on internal nodes or data points.
 */
void SSNode::updateBoundingEnvelope() {
  if (_data.empty() && children.empty()) return;

  std::vector<Point> centroids = getEntriesCentroids();
  Point mean(Eigen::VectorXf::Zero(
          (Eigen::Index)centroids[0].dim())); // Correctly initialize with a zero vector
  for (const Point &_centroid: centroids) {
    mean += _centroid;
  }
  mean /= (float) centroids.size();

  float maxRadius = 0.0f;
  for (const Point &_centroid: centroids) {
    float distance = (mean - _centroid).norm();
    if (distance > maxRadius) {
      maxRadius = distance;
    }
  }

  centroid = mean;
  radius = maxRadius;
}

/**
 * directionOfMaxVariance
 * Computes and returns the index of the direction with the maximum variance.
 * @return size_t: Index of the direction with the maximum variance.
 */
size_t SSNode::directionOfMaxVariance() {
  std::vector<Point> centroids = getEntriesCentroids();
  size_t maxDirection = 0;
  float maxVariance = 0.0f;

  for (size_t i = 0; i < centroids.size(); ++i) {
    float mean = 0.0f;
    for (const Point &p: centroids) {
      mean += p[i];
    }
    mean /= centroids.size();

    float variance = 0.0f;
    for (const Point &p: centroids) {
      variance += (p[i] - mean) * (p[i] - mean);
    }
    variance /= centroids.size();

    if (variance > maxVariance) {
      maxVariance = variance;
      maxDirection = i;
    }
  }

  return maxDirection;
}

/**
 * split
 * Splits the node and returns the new node created.
 * Similar to R-tree implementation.
 * @return SSNode*: Pointer to the new node created by the split.
 */
SSNode *SSNode::split() {
  size_t splitIndex = findSplitIndex(directionOfMaxVariance());

  SSNode *newNode = new SSNode(centroid, radius, isLeaf, parent);
  newNode->_data = std::vector<Data *>(_data.begin() + splitIndex,
                                       _data.end());
  _data.erase(_data.begin() + splitIndex, _data.end());

  updateBoundingEnvelope();
  newNode->updateBoundingEnvelope();

  return newNode;
}

/**
 * findSplitIndex
 * Finds the split index along a specific coordinate.
 * @param coordinateIndex: The index of the coordinate for the split.
 * @return size_t: The split index.
 */
size_t SSNode::findSplitIndex(size_t coordinateIndex) {
  std::sort(_data.begin(), _data.end(), [&](Data *a, Data *b) {
      return a->getEmbedding()[coordinateIndex] <
             b->getEmbedding()[coordinateIndex];
  });

  return _data.size() / 2;
}

/**
 * getEntriesCentroids
 * Returns the centroids of the entries.
 * These centroids can be points stored in leaves or centroids of child nodes in internal nodes.
 * @return std::vector<Point>: Vector of entry centroids.
 */
std::vector<Point> SSNode::getEntriesCentroids() {
  std::vector<Point> centroids;
  if (isLeaf) {
    for (Data *d: _data) {
      centroids.push_back(d->getEmbedding());
    }
  } else {
    for (SSNode *child: children) {
      centroids.push_back(child->centroid);
    }
  }
  return centroids;
}

/**
 * minVarianceSplit
 * Finds the optimal split index to minimize the sum of variances of the two partitions.
 * @param values: Vector of values to find the minimal variance split.
 * @return size_t: Index of the minimal variance split.
 */
size_t SSNode::minVarianceSplit(const std::vector<float> &values) {
  size_t minIndex = 0;
  float minVarianceSum = std::numeric_limits<float>::max();

  for (size_t i = 1; i < values.size() - 1; ++i) {
    float leftMean =
            std::accumulate(values.begin(), values.begin() + i, 0.0f) / i;
    float rightMean = std::accumulate(values.begin() + i, values.end(), 0.0f) /
                      (values.size() - i);

    float leftVariance = 0.0f, rightVariance = 0.0f;
    for (size_t j = 0; j < i; ++j) {
      leftVariance += (values[j] - leftMean) * (values[j] - leftMean);
    }
    for (size_t j = i; j < values.size(); ++j) {
      rightVariance += (values[j] - rightMean) * (values[j] - rightMean);
    }

    float varianceSum = leftVariance + rightVariance;
    if (varianceSum < minVarianceSum) {
      minVarianceSum = varianceSum;
      minIndex = i;
    }
  }

  return minIndex;
}

/**
 * searchParentLeaf
 * Finds the appropriate leaf node for inserting a point.
 * @param node: Node to start the search from.
 * @param target: Target point for the search.
 * @return SSNode*: Suitable leaf node for insertion.
 */
SSNode *SSNode::searchParentLeaf(SSNode *node, const Point &target) {
  while (!node->isLeaf) {
    node = node->findClosestChild(target);
  }
  return node;
}

/**
 * insert
 * Inserts data into the node, splitting if necessary.
 * @param node: Node to insert data into.
 * @param _data: Data to be inserted.
 * @return SSNode*: New root node if a split occurred, otherwise nullptr.
 */
SSNode *SSNode::insert(SSNode *node, Data *_data) {
  if (node->isLeaf) {
    node->_data.push_back(_data);
    if (node->_data.size() > maxPointsPerNode) {
      return node->split();
    }
    node->updateBoundingEnvelope();
    return nullptr;
  }

  SSNode *leaf = searchParentLeaf(node, _data->getEmbedding());
  SSNode *newNode = leaf->insert(leaf, _data);

  if (newNode) {
    node->children.push_back(newNode);
    node->updateBoundingEnvelope();
    if (node->children.size() > maxPointsPerNode) {
      return node->split();
    }
  }

  return nullptr;
}

/**
 * search
 * Searches for specific data in the tree.
 * @param node: Node to start the search from.
 * @param _data: Data to search for.
 * @return SSNode*: Node containing the data (or nullptr if not found).
 */
SSNode *SSNode::search(SSNode *node, Data *_data) {
  if (node->isLeaf) {
    for (Data *d: node->_data) {
      if (d == _data) {
        return node;
      }
    }
    return nullptr;
  }

  SSNode *closestChild = node->findClosestChild(_data->getEmbedding());
  return search(closestChild, _data);
}

/**
 * insert
 * Inserts data into the tree.
 * @param _data: Data to be inserted.
 */
void SSTree::insert(Data *_data) {
  if (!root) {
    root = new SSNode(_data->getEmbedding(), 0.0f, true);
  }

  SSNode *newRoot = root->insert(root, _data);
  if (newRoot) {
    // If the root splits, create a new internal root node
    auto *newInternalRoot = new SSNode(root->getCentroid(), root->getRadius(),
                                       false);
    newInternalRoot->children.push_back(root);
    newInternalRoot->children.push_back(newRoot);
    newInternalRoot->updateBoundingEnvelope();
    root = newInternalRoot; // Update the root to the new internal node
  }
}

/**
 * search
 * Searches for specific data in the tree.
 * @param _data: Data to search for.
 * @return SSNode*: Node containing the data (or nullptr if not found).
 */
SSNode *SSTree::search(Data *_data) {
  return root ? root->search(root, _data) : nullptr;
}