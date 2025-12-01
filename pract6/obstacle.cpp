#include "obstacle.h"
#include "Particle.h"
#include <algorithm>
#include <cmath>

using namespace std;

Obstacle::Obstacle(const Vec2& c, double h)
    : center(c), halfSize(h)
{
}

bool Obstacle::checkCollision(const Particle& p, Vec2& outNormal) const {
    if (!p.active) return false;

    double minX = center.x - halfSize;
    double maxX = center.x + halfSize;
    double minY = center.y - halfSize;
    double maxY = center.y + halfSize;

    // Punto más cercano del rectángulo al centro de la partícula
    double closestX = max(minX, min(p.position.x, maxX));
    double closestY = max(minY, min(p.position.y, maxY));

    double dx = p.position.x - closestX;
    double dy = p.position.y - closestY;
    double dist2 = dx * dx + dy * dy;

    if (dist2 > p.radius * p.radius) {
        return false;
    }

    // Calculamos normal
    // Caso especial: centro dentro del rectángulo
    if (dist2 == 0.0) {
        // Elegimos normal basándonos en la menor distancia a un lado
        double leftDist   = abs(p.position.x - minX);
        double rightDist  = abs(maxX - p.position.x);
        double bottomDist = abs(p.position.y - minY);
        double topDist    = abs(maxY - p.position.y);

        double minDist = min(min(leftDist, rightDist),
                             min(bottomDist, topDist));

        if (minDist == leftDist)          outNormal = Vec2(-1.0, 0.0);
        else if (minDist == rightDist)    outNormal = Vec2(1.0, 0.0);
        else if (minDist == bottomDist)   outNormal = Vec2(0.0, -1.0);
        else                              outNormal = Vec2(0.0, 1.0);
    } else {
        Vec2 normal(dx, dy);
        outNormal = normal.normalized();
    }

    return true;
}
