#include "Particle.h"

Particle::Particle(int id_, const Vec2& pos, const Vec2& vel,
                   double m, double r)
    : id(id_), position(pos), velocity(vel),
    mass(m), radius(r), active(true)
{
}

void Particle::update(double dt) {
    position += velocity * dt;
}
