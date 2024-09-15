#include "plane.h"

NType Plane::distance(const Point3D &p) const {
    return {0};
}

RelationType Polygon::relationWithPlane(const Plane &plane) const {
    bool front = false, back = false;
    for (const Point3D &vertex: vertices) {
        NType distance = plane.distance(vertex);
        if (distance > 0) {
            front = true;
        } else if (distance < 0) {
            back = true;
        }
        if (front && back) {
            return SPLIT;
        }
    }
    return front ? IN_FRONT : (back ? BEHIND : COINCIDENT);
}

std::pair<Polygon, Polygon> Polygon::split(const Plane &plane) const {
    std::vector<Point3D> frontVertices, backVertices;
    return std::make_pair(Polygon(frontVertices), Polygon(backVertices));
}

Plane Polygon::getPlane() const {
    return Plane();
}
