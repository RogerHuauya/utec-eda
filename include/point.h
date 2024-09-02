#pragma once

class Point2D{
    public:
        Point2D();
        Point2D(float x, float y);
        void setX(float x);
        void setY(float y);
        float getX();
        float getY();
        void print();

        Point2D operator+(const Point2D& p){
            return Point2D(x + p.x, y + p.y);
        }
        Point2D operator-(const Point2D& p){
            return Point2D(x - p.x, y - p.y);
        }
        bool operator==(const Point2D& p){
            return x == p.x && y == p.y;
        }
        Point2D operator*(const float& f){
            return Point2D(x*f, y*f);
        }

        Point2D operator*(const int& f){
            return Point2D(x*f, y*f);
        }

        friend Point2D operator*(const float& f, const Point2D& p){
            return Point2D(p.x*f, p.y*f);
        }

        friend Point2D operator*(const int& f, const Point2D& p){
            return Point2D(p.x*f, p.y*f);
        }

    private:
        float x;
        float y;
};