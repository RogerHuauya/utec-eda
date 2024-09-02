#pragma once

#include "point.h"

class Rect {
private:
    Point2D pmin, pmax;
public:
    Rect() = default;

    Rect(const Point2D &pmin, const Point2D &pmax);

    ~Rect() = default;

    Point2D getPmin() const;

    Point2D getPmax() const;

    Point2D getCenter() const;

    bool contains(const Point2D &p) const;

    bool intersects(const Rect &rect) const;

    bool isWithin(const Rect &rect) const;

    bool isValid() const;

    bool operator==(const Rect &rect) const;

    bool operator!=(const Rect &rect) const;

    friend std::ostream &operator<<(std::ostream &os, const Rect &rect);

};