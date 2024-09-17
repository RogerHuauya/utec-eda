#include "plane.h"

NType Plane::distance(const Point3D &p) const {
    Point3D diff = p - _p;
    return _n.dotProduct(diff);
}


Point3D Plane::intersect(const Line &l) const {
    NType t = (_n.dotProduct(_p) - _n.dotProduct(l.getPoint())) /
              _n.dotProduct(l.getUnit());
    return l.getPoint() + t * Point3D(l.getUnit());
}

bool Plane::contains(const Point3D &p) const {
    if (_n.dotProduct(p - _p) == 0) {
        return true;
    }
    return false;
}

RelationType Plane::relationWithPlane(const Polygon &polygon) const {
    bool front = false, back = false;
    for (const Point3D &vertex: polygon.getVertices()) {
        NType distance = this->distance(vertex);
        if (distance > 0) {
            front = true;
        } else if (distance < 0) {
            back = true;
        }
    }
    if (front && back) {
        return SPLIT;
    } else if (front) {
        return IN_FRONT;
    } else if (back) {
        return BEHIND;
    } else {
        return COINCIDENT;
    }
}

bool Plane::contains(const Line &l) const {
    return contains(l.getPoint()) && _n.dotProduct(l.getUnit()) == 0;
}

NType Plane::signedDistanceToPoint(const Point3D &point) const {
    Vector3D vectorToPoint = point - _p;

    return vectorToPoint.dotProduct(_n);
}


bool Plane::operator==(const Plane &other) const {
    return ((_n == other._n && contains(other._p) ||
             (_n == -other._n && contains(other._p))));
}

bool Plane::operator!=(const Plane &other) const {
    return !(*this == other);
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
    }
    if (front && back) {
        return SPLIT;
    } else if (front) {
        return IN_FRONT;
    } else if (back) {
        return BEHIND;
    } else {
        return COINCIDENT;
    }
}

std::pair<Polygon, Polygon> Polygon::split(const Plane &plane) const {
    std::vector<Point3D> frontVertices, backVertices;
    std::vector<Point3D> intersections;

    for (size_t i = 0; i < vertices.size(); i++) {
        const Point3D &current = vertices[i];
        const Point3D &next = vertices[(i + 1) % vertices.size()];

        NType currentDist = plane.signedDistanceToPoint(current);
        NType nextDist = plane.signedDistanceToPoint(next);

        if (currentDist >= 0) frontVertices.push_back(current);
        if (currentDist <= 0) backVertices.push_back(current);

        // Check if the edge crosses the plane
        if ((currentDist > 0 && nextDist < 0) ||
            (currentDist < 0 && nextDist > 0)) {
            NType t = currentDist / (currentDist - nextDist);
            Point3D intersection = current + (next - current) * t;
            intersections.push_back(intersection);
            frontVertices.push_back(intersection);
            backVertices.push_back(intersection);
        }
    }

    // Add intersection points to ensure proper polygon formation
    for (const auto &intersection: intersections) {
        if (std::find(frontVertices.begin(), frontVertices.end(),
                      intersection) == frontVertices.end()) {
            frontVertices.push_back(intersection);
        }
        if (std::find(backVertices.begin(), backVertices.end(),
                      intersection) == backVertices.end()) {
            backVertices.push_back(intersection);
        }
    }

    // Handle cases where the plane doesn't actually split the polygon
    if (frontVertices.size() < 3) return {Polygon(), *this};
    if (backVertices.size() < 3) return {*this, Polygon()};

    return {Polygon(frontVertices), Polygon(backVertices)};
}


Point3D Polygon::getCentroid() const {
    Point3D centroid;
    for (const Point3D &vertex: vertices) {
        centroid = centroid + vertex;
    }
    return (1.0 / vertices.size()) * centroid;
}

Vector3D Polygon::getNormal() const {
    Vector3D normal;
    for (size_t i = 0; i < vertices.size(); i++) {
        normal = normal + Vector3D(vertices[i]).crossProduct(
                vertices[(i + 1) % vertices.size()]);
    }
    return normal.unit();
}


Plane Polygon::getPlane() const {
    return {getNormal(), this->getCentroid()};
}



