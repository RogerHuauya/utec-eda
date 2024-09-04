#pragma once

#include "point.h"
#include "rect.h"
#include "datatype.h"

class Particle {
    Point2D position;
    Point2D velocity;
    static const NType timeStep;
public:
    Particle();

    Particle(const Point2D &position, const Point2D &velocity);

    Point2D getPosition() const;

    Point2D getVelocity() const;

    void updatePosition(const Rect &boundary);

    friend std::ostream &
    operator<<(std::ostream &os, const Particle &particle);

};
