#include <iostream>
#include <cmath>
#include "point.h"
#include "rect.h"

Point2D::Point2D() {
    x = 0;
    y = 0;
}

Point2D::Point2D(NType x, NType y) {
    this->x = x;
    this->y = y;
}

void Point2D::setX(NType x) {
    this->x = x;
}

void Point2D::setY(NType y) {
    this->y = y;
}

NType Point2D::getX() const {
    return x;
}

NType Point2D::getY() const {
    return y;
}

NType Point2D::distance(const Point2D &p) const {
    return sqrt(pow(x - p.x, 2) + pow(y - p.y, 2));
}

std::ostream &operator<<(std::ostream &os, const Point2D &p) {
    os << "(" << p.x << ", " << p.y << ")";
    return os;
}

NType distance(const Point2D &a, const Point2D &b) {
    NType dx = a.getX() - b.getX();
    NType dy = a.getY() - b.getY();
    return sqrt(dx * dx + dy * dy);
}

NType minDistToRect(const Point2D &p, const Rect &rect) {
    NType dx = std::max(rect.getPmin().getX() - p.getX(), NType(0));
    dx = std::max(dx, p.getX() - rect.getPmax().getX());

    NType dy = std::max(rect.getPmin().getY() - p.getY(), NType(0));
    dy = std::max(dy, p.getY() - rect.getPmax().getY());

    return sqrt(dx * dx + dy * dy);
}

