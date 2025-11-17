#include "Box.h"
#include "Particle.h"

using namespace std;

Box::Box(double w, double h)
    : width(w), height(h)
{
}

bool Box::handleWallCollision(Particle& p, std::ofstream& log, double time) {
    if (!p.active) return false;
    bool collided = false;

    // Paredes izquierda y derecha (x=0 y x=width)
    if (p.position.x - p.radius < 0.0) {
        p.position.x = p.radius; // Corrige penetración
        p.velocity.x = -p.velocity.x;
        log << "COLLISION " << time << " " << p.id << " MURO_IZQ\n";
        collided = true;
    } else if (p.position.x + p.radius > width) {
        p.position.x = width - p.radius;
        p.velocity.x = -p.velocity.x;
        log << "COLLISION " << time << " " << p.id << " MURO_DER\n";
        collided = true;
    }

    // Paredes inferior y superior (y=0 y y=height)
    if (p.position.y - p.radius < 0.0) {
        p.position.y = p.radius;
        p.velocity.y = -p.velocity.y;
        log << "COLLISION " << time << " " << p.id << " MURO_ABAJ\n";
        collided = true;
    } else if (p.position.y + p.radius > height) {
        p.position.y = height - p.radius;
        p.velocity.y = -p.velocity.y;
        log << "COLLISION " << time << " " << p.id << " MURO_ARR\n";
        collided = true;
    }

    return collided;
}
