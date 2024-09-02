#pragma once

#include "point.h"

class Rect {
    private:
        Point2D pmin;
        Point2D pmax;
    public:
        Rect();
        Rect(Point2D pmin, Point2D pmax);
        Rect(float xmin, float ymin, float xmax, float ymax);
        void setPmin(Point2D pmin);
        void setPmax(Point2D pmax);
        Point2D getPmin() const;
        Point2D getPmax() const;
};