#pragma once

#include "datatype.h"
#include <iostream>

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


class Point3D {
    NType x, y, z;

public:
    Point3D() : x(0), y(0), z(0) {}
    Point3D(NType x, NType y, NType z) : x(x), y(y), z(z) {}

    ~Point3D() = default;

    NType getX() const { return x; }
    NType getY() const { return y; }
    NType getZ() const { return z; }

    void setX(NType x) { this->x = x; }
    void setY(NType y) { this->y = y; }
    void setZ(NType z) { this->z = z; }

    NType distance(const Point3D &p) const {
        return sqrt((x - p.x) * (x - p.x) + (y - p.y) * (y - p.y) + (z - p.z) * (z - p.z));
    }

    // Operators
    bool operator==(const Point3D &p) const {
        return x == p.x && y == p.y && z == p.z;
    }
    bool operator!=(const Point3D &p) const {
        return !(*this == p);
    }
    Point3D operator-(const Point3D &p) const {
        return Point3D(x - p.x, y - p.y, z - p.z);
    }
    Point3D operator+(const Point3D &p) const {
        return Point3D(x + p.x, y + p.y, z + p.z);
    }

    // Print
    friend std::ostream &operator<<(std::ostream &os, const Point3D &p) {
        os << "(" << p.x.getValue() << "," << p.y.getValue() << "," << p.z.getValue() << ")";
        return os;
    }
};