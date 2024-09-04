#pragma once

#include "datatype.h"

class Rect;

class Point2D {
public:
    Point2D();

    Point2D(NType x, NType y);

    void setX(NType x);

    void setY(NType y);

    NType getX() const;

    NType getY() const;

    NType distance(const Point2D &p) const;

    bool operator==(const Point2D &p) const {
        return x == p.x && y == p.y;
    }

    bool operator!=(const Point2D &p) const {
        return x != p.x || y != p.y;
    }

    Point2D operator+(const Point2D &p) const {
        return {x + p.x, y + p.y};
    }

    Point2D operator-(const Point2D &p) const {
        return {x - p.x, y - p.y};
    }

    Point2D operator*(const NType &f) const {
        return {x * f, y * f};
    }

    friend Point2D operator*(const NType &f, const Point2D &p) {
        return {p.x * f, p.y * f};
    }

    friend std::ostream &operator<<(std::ostream &os, const Point2D &p);

private:
    NType x;
    NType y;
};

NType distance(const Point2D &a, const Point2D &b);

NType minDistToRect(const Point2D &p, const Rect &rect);