#ifndef PARTICLE_H
#define PARTICLE_H

#include "Vec2.h"

class Particle {
public:
    int id;
    Vec2 position;
    Vec2 velocity;
    double mass;
    double radius;
    bool active;

    Particle(int id_, const Vec2& pos, const Vec2& vel,
             double m, double r);

    void update(double dt);
};

#endif // PARTICLE_H
