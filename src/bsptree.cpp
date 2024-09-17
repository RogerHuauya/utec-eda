#include "bsptree.h"
#include <algorithm>

void BSPNode::insert(const Polygon &polygon) {
    RelationType relation = partition.relationWithPlane(polygon);

    switch (relation) {
        case COINCIDENT:
            polygons.push_back(polygon);
            break;

        case SPLIT: {
            std::pair<Polygon, Polygon> splitPolys = polygon.split(partition);
            if (front) {
                front->insert(splitPolys.first);
            } else {
                front = new BSPNode(partition);
                front->insert(splitPolys.first);
            }
            if (back) {
                back->insert(splitPolys.second);
            } else {
                back = new BSPNode(partition);
                back->insert(splitPolys.second);
            }
            break;
        }

        case IN_FRONT:
            if (front) {
                front->insert(polygon);
            } else {
                front = new BSPNode(partition);
            }
            break;

        case BEHIND:
            if (back) {
                back->insert(polygon);
            } else {
                back = new BSPNode(partition);
            }
            break;
    }
}


void BSPTree::insert(const Polygon &polygon) {
    if (!root) {
        root = new BSPNode(polygon.getPlane());
        root->insert(polygon);
    } else {
        root->insert(polygon);
    }
}