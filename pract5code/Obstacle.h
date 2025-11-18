#ifndef OBSTACLE_H
#define OBSTACLE_H

#include "Vec2.h"

class Particle;

class Obstacle {
public:
    Vec2 center;
    double halfSize; // lado = 2 * halfSize

    Obstacle(const Vec2& c, double h);

    // Detecta colisión círculo-rectángulo
    // Devuelve true si hay colisión y escribe la normal de la superficie.
    bool checkCollision(const Particle& p, Vec2& outNormal) const;
};

#endif
