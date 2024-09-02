#include "particle.h"

Particle::Particle(){
    position = Point2D(0,0);
    velocity = Point2D(0,0);
}

Particle::Particle(Point2D position, Point2D velocity){
    this->position = position;
    this->velocity = velocity;
}

void Particle::updatePosition(Rect& board){
    position.setX(position.getX() + velocity.getX());
    position.setY(position.getY() + velocity.getY());
    if(position.getX() < board.getPmin().getX() || position.getX() > board.getPmax().getX()){
        velocity.setX(-velocity.getX());
    }
    if(position.getY() < board.getPmin().getY() || position.getY() > board.getPmax().getY()){
        velocity.setY(-velocity.getY());
    }
}