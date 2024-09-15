#include "bsptree.h"

void BSPNode::insert(const Polygon &polygon) {
    return;
}


void BSPTree::insert(const Polygon &polygon) {
    if (!root) {
        root = new BSPNode(polygon.getPlane());
        root->insert(polygon);
    } else {
        root->insert(polygon);
    }
}