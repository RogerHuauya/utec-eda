#include "rect.h"

Rect::Rect() {
    pmin = Point2D(0, 0);
    pmax = Point2D(0, 0);
}

Rect::Rect(Point2D pmin, Point2D pmax) {
    this->pmin = pmin;
    this->pmax = pmax;
}

Rect::Rect(float xmin, float ymin, float xmax, float ymax) {
    pmin = Point2D(xmin, ymin);
    pmax = Point2D(xmax, ymax);
}

void Rect::setPmin(Point2D pmin) {
    this->pmin = pmin;
}

void Rect::setPmax(Point2D pmax) {
    this->pmax = pmax;
}

Point2D Rect::getPmin() const {
    return this->pmin;
}

Point2D Rect::getPmax() const {
    return this->pmax;
}

