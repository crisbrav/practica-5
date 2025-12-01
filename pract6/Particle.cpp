#include "Particle.h"

Particle::Particle()
    : id(-1),
    position(0.0, 0.0),
    velocity(0.0, 0.0),
    mass(1.0),
    radius(1.0),
    active(false)
{
}

Particle::Particle(int id_, const Vec2& pos, const Vec2& vel,
                   double m, double r)
    : id(id_), position(pos), velocity(vel),
    mass(m), radius(r), active(true)
{
}

void Particle::update(double dt) {
    position += velocity * dt;
}
