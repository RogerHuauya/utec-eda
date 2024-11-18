#include <iostream>
#include <cmath>
#include <random>

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


// Constructor
Point::Point(const Eigen::VectorXf &coordinates) : coordinates_(coordinates) {
  if (coordinates.size() != DIM) {
    std::cout << "Dimensionalidad incorrecta :c" << std::endl;
    std::cout << coordinates.size() << std::endl;
    throw std::invalid_argument("Dimensionalidad incorrecta :c");
  }
}

// Operadores
Point Point::operator+(const Point &other) const {
  return Point(coordinates_ + other.coordinates_);
}

Point &Point::operator+=(const Point &other) {
  coordinates_ += other.coordinates_;
  return *this;
}

Point Point::operator-(const Point &other) const {
  return Point(coordinates_ - other.coordinates_);
}

Point &Point::operator-=(const Point &other) {
  coordinates_ -= other.coordinates_;
  return *this;
}

Point Point::operator*(float scalar) const {
  return Point(coordinates_ * scalar);
}

Point &Point::operator*=(float scalar) {
  coordinates_ *= scalar;
  return *this;
}

Point Point::operator/(float scalar) const {
  if (std::abs(scalar) < EPSILON) {
    throw std::invalid_argument("División por cero (o casi cero).");
  }
  return Point(coordinates_ / scalar);
}

Point &Point::operator/=(float scalar) {
  if (std::abs(scalar) < EPSILON) {
    throw std::invalid_argument("División por cero (o casi cero).");
  }
  coordinates_ /= scalar;
  return *this;
}

// Punto aleatorio
Point Point::random(float min, float max) {
  static std::random_device rd;
  static std::mt19937 gen(rd());
  std::uniform_real_distribution<float> dis(min, max);

  Eigen::VectorXf coordinates(DIM);
  for (std::size_t i = 0; i < DIM; ++i) {
    coordinates[i] = dis(gen);
  }

  return Point(coordinates);
}

// Imprimir el punto
void Point::print() const {
  std::cout << "Point(";
  for (std::size_t i = 0; i < DIM; ++i) {
    std::cout << coordinates_[i];
    if (i < DIM - 1) std::cout << ", ";
  }
  std::cout << ")" << std::endl;
}
