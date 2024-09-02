#include "rect.h"
#include <iostream>


Rect::Rect(const Point2D &pmin, const Point2D &pmax) {
    this->pmin = pmin;
    this->pmax = pmax;
}

Point2D Rect::getPmin() const {
    return this->pmin;
}

Point2D Rect::getPmax() const {
    return this->pmax;
}

Point2D Rect::getCenter() const {
    return {(pmin.getX() + pmax.getX()) / 2, (pmin.getY() + pmax.getY()) / 2};
}

bool Rect::contains(const Point2D &p) const {
    return p.getX() >= pmin.getX() && p.getX() <= pmax.getX() &&
           p.getY() >= pmin.getY() && p.getY() <= pmax.getY();
}

bool Rect::intersects(const Rect &rect) const {
    return pmin.getX() <= rect.pmax.getX() &&
           pmax.getX() >= rect.pmin.getX() &&
           pmin.getY() <= rect.pmax.getY() &&
           pmax.getY() >= rect.pmin.getY();
}

bool Rect::isWithin(const Rect &rect) const {
    return pmin.getX() >= rect.pmin.getX() &&
           pmax.getX() <= rect.pmax.getX() &&
           pmin.getY() >= rect.pmin.getY() &&
           pmax.getY() <= rect.pmax.getY();
}

bool Rect::isValid() const {
    return pmin.getX() <= pmax.getX() && pmin.getY() <= pmax.getY();
}

bool Rect::operator==(const Rect &rect) const {
    return pmin == rect.pmin && pmax == rect.pmax;
}

bool Rect::operator!=(const Rect &rect) const {
    return pmin != rect.pmin || pmax != rect.pmax;
}

std::ostream &operator<<(std::ostream &os, const Rect &rect) {
    os << "Rect: pmin(" << rect.pmin.getX() << ", " << rect.pmin.getY()
       << "), pmax(" << rect.pmax.getX() << ", "
       << rect.pmax.getY() << ")";
    return os;
}