#include "point.h"

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
