#pragma once

#include "point.h"
#include "rect.h"

class Particle {
    Point2D position;
    Point2D velocity;
public:
    Particle();

    Particle(Point2D position, Point2D velocity);

    void updatePosition(Rect &board);
};
