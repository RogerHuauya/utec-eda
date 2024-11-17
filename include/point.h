#pragma once

#include <Eigen/Dense>
#include <iostream>
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


class Point3D {
public:
    NType x, y, z;

    Point3D() : x(0), y(0), z(0) {}

    // copy assignment constructor

    Point3D(const Point3D &p) : x(p.x), y(p.y), z(p.z) {}

    Point3D(NType x, NType y, NType z) : x(x), y(y), z(z) {}

    ~Point3D() = default;

    NType getX() const { return x; }

    NType getY() const { return y; }

    NType getZ() const { return z; }

    void setX(NType x) { this->x = x; }

    void setY(NType y) { this->y = y; }

    void setZ(NType z) { this->z = z; }

    NType distance(const Point3D &p) const {
      return sqrt((x - p.x) * (x - p.x) + (y - p.y) * (y - p.y) +
                  (z - p.z) * (z - p.z));
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
      return {x + p.x, y + p.y, z + p.z};
    }

    Point3D operator*(const NType &f) const {
      return {x * f, y * f, z * f};
    }

    friend Point3D operator*(const NType &f, const Point3D &p) {
      return {p.x * f, p.y * f, p.z * f};
    }

    // Print
    friend std::ostream &operator<<(std::ostream &os, const Point3D &p) {
      os << "(" << p.x.getValue() << "," << p.y.getValue() << ","
         << p.z.getValue() << ")";
      return os;
    }
};

constexpr std::size_t DIM = 768;
constexpr float EPSILON = 1e-8f;

class Point {
public:
    // Constructores
    Point() : coordinates_(Eigen::VectorXf::Zero(DIM)) {}

    explicit Point(const Eigen::VectorXf &coordinates);

    // Operadores
    Point operator+(const Point &other) const;

    Point &operator+=(const Point &other);

    Point operator-(const Point &other) const;

    Point &operator-=(const Point &other);

    Point operator*(float scalar) const;

    Point &operator*=(float scalar);

    Point operator/(float scalar) const;

    Point &operator/=(float scalar);

    // Métodos adicionales
    float norm() const { return coordinates_.norm(); }

    float normSquared() const { return coordinates_.squaredNorm(); }

    float distance(const Point &other) const { return (*this - other).norm(); }

    float distanceSquared(const Point &other) const {
      return (*this - other).normSquared();
    }

    // Operadores de acceso
    float operator[](std::size_t index) const { return coordinates_(index); }

    float &operator[](std::size_t index) { return coordinates_(index); }

    // Puntos aleatorios
    static Point random(float min = 0.0f, float max = 1.0f);

    // Print!
    void print() const;

private:
    Eigen::VectorXf coordinates_;
};