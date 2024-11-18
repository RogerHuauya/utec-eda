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
  if (isLeaf) {
    std::cout << "Error: findClosestChild called on a leaf node." << std::endl;
    exit(0);
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

float
SSNode::computeMeanForDimension(std::vector<Point> &centroids, size_t i) {
  float mean = 0.f;
  for (auto &point: centroids) {
    mean += point[i];
  }
  mean = mean / static_cast<float>(centroids.size());
  return mean;
}

/**
 * updateBoundingEnvelope
 * Updates the centroid and radius of the node based on internal nodes or data points.
 */
void SSNode::updateBoundingEnvelope() {
  std::vector<Point> points = getEntriesCentroids();
  for (size_t i = 0; i < DIM; i++)
    this->centroid[i] = computeMeanForDimension(points, i);

  this->radius = 0.f;
  if (isLeaf) {
    for (auto &point: points) {
      this->radius = std::max(this->radius, (point - centroid).norm());
    }
  } else {
    for (auto &child: children) {
      float dist = (child->centroid - centroid).norm() + child->radius;
      if (dist > radius) {
        radius = dist;
      }
    }
  }
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
std::pair<SSNode *, SSNode *> SSNode::split() {
  size_t splitIndex = findSplitIndex(directionOfMaxVariance());

  SSNode *newNode1;
  SSNode *newNode2;

  if (isLeaf) {
    newNode1 = new SSNode(centroid, radius, true, parent);
    newNode2 = new SSNode(centroid, radius, true, parent);

    newNode1->_data = std::vector<Data *>(_data.begin(),
                                          _data.begin() + splitIndex);
    newNode2->_data = std::vector<Data *>(_data.begin() + splitIndex,
                                          _data.end());
  } else {
    newNode1 = new SSNode(centroid, radius, false, parent);
    newNode2 = new SSNode(centroid, radius, false, parent);

    newNode1->children = std::vector<SSNode *>(children.begin(),
                                               children.begin() + splitIndex);
    newNode2->children = std::vector<SSNode *>(children.begin() + splitIndex,
                                               children.end());

    for (SSNode *child: newNode1->children) {
      child->parent = newNode1;
    }
    for (SSNode *child: newNode2->children) {
      child->parent = newNode2;
    }
  }

  newNode1->updateBoundingEnvelope();
  newNode2->updateBoundingEnvelope();

  return {newNode1, newNode2};
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
      centroids.emplace_back(d->getEmbedding());
    }
  } else {
    for (SSNode *child: children) {
      centroids.emplace_back(child->centroid);
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
std::pair<SSNode *, SSNode *> SSNode::insert(SSNode *node, Data *_data) {
  if (node->isLeaf) {
    if (std::find(node->_data.begin(), node->_data.end(), _data) !=
        node->_data.end()) {
      return {nullptr, nullptr};
    }

    node->_data.push_back(_data);
    node->updateBoundingEnvelope();
    if (node->_data.size() <= maxPointsPerNode) {
      return {nullptr, nullptr};
    }
    return node->split();
  }
  SSNode *closestChild = node->findClosestChild(_data->getEmbedding());
  auto [newRoot1, newRoot2] = insert(closestChild, _data);
  if (newRoot1 == nullptr) {
    node->updateBoundingEnvelope();
    return {nullptr, nullptr};
  }
  std::erase(node->children, closestChild);
  node->children.push_back(newRoot1);
  node->children.push_back(newRoot2);

  node->updateBoundingEnvelope();

  if (node->children.size() <= maxPointsPerNode) {
    return {nullptr, nullptr};
  }

  return node->split();
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
  if (root == nullptr) {
    root = new SSNode(_data->getEmbedding(), 0.0f, true, nullptr);
  }
  auto [newRoot1, newRoot2] = root->insert(root, _data);
  if (newRoot1 != nullptr) {
    root = new SSNode(_data->getEmbedding(), 0.0f, false, nullptr);
    root->children.push_back(newRoot1);
    root->children.push_back(newRoot2);
    root->isLeaf = false;
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