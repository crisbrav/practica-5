#include "GameSimulation.h"
#include <cmath>

GameSimulation::GameSimulation(double width, double height,
                               double dt_, double restitution_)
    : box(width, height),
    dt(dt_),
    obstacleRestitution(restitution_),
    playerIndex(0),
    gameOver(false)
{
}

void GameSimulation::step() {
    if (gameOver) return;

    // 1. Actualizar posiciones
    for (auto& p : particles) {
        if (!p.active) continue;
        p.update(dt);
    }

    // 2. Colisiones con paredes
    handleWallCollisions();

    // 3. Colisiones partícula-obstáculo
    handleParticleObstacleCollisions();

    // 4. Colisiones partícula-partícula
    handleParticleParticleCollisions();
}

const std::vector<Particle>& GameSimulation::getParticles() const {
    return particles;
}

const std::vector<Obstacle>& GameSimulation::getObstacles() const {
    return obstacles;
}

Particle* GameSimulation::getPlayer() {
    if (playerIndex < 0 || playerIndex >= (int)particles.size()) return nullptr;
    return &particles[playerIndex];
}

const Particle* GameSimulation::getPlayer() const {
    if (playerIndex < 0 || playerIndex >= (int)particles.size()) return nullptr;
    return &particles[playerIndex];
}

bool GameSimulation::isGameOver() const {
    return gameOver;
}

void GameSimulation::handleWallCollisions() {
    for (auto& p : particles) {
        std::ofstream dummy; // el método de Box requiere un ofstream, pero no lo usamos
        box.handleWallCollision(p, dummy, 0.0);
    }
}

void GameSimulation::handleParticleObstacleCollisions() {
    for (std::size_t i = 0; i < particles.size(); ++i) {
        Particle& p = particles[i];
        if (!p.active) continue;

        for (std::size_t j = 0; j < obstacles.size(); ++j) {
            Vec2 normal;
            if (obstacles[j].checkCollision(p, normal)) {
                // Si el que colisiona es el jugador, game over
                if ((int)i == playerIndex) {
                    gameOver = true;
                    return;
                }

                // respuesta inelástica como en la simulación
                Vec2 v = p.velocity;
                double v_n_scalar = v.dot(normal);
                Vec2 v_n = normal * v_n_scalar;
                Vec2 v_t = v - v_n;
                Vec2 v_n_prime = normal * (-obstacleRestitution * v_n_scalar);
                p.velocity = v_n_prime + v_t;
            }
        }
    }
}

void GameSimulation::handleParticleParticleCollisions() {
    std::size_t n = particles.size();

    for (std::size_t i = 0; i < n; ++i) {
        Particle& a = particles[i];
        if (!a.active) continue;

        for (std::size_t j = i + 1; j < n; ++j) {
            Particle& b = particles[j];
            if (!b.active) continue;

            Vec2 diff = a.position - b.position;
            double dist = diff.length();
            double minDist = a.radius + b.radius;

            if (dist <= minDist) {
                // Si el jugador colisiona con otra partícula, game over
                if ((int)i == playerIndex || (int)j == playerIndex) {
                    gameOver = true;
                    return;
                }

                // Para las demás, podemos aplicar fusión como antes
                double M = a.mass + b.mass;
                Vec2 newVel = (a.velocity * a.mass + b.velocity * b.mass) * (1.0 / M);
                Vec2 newPos = (a.position * a.mass + b.position * b.mass) * (1.0 / M);
                double newRadius = std::sqrt(a.radius * a.radius +
                                             b.radius * b.radius);

                a.mass = M;
                a.velocity = newVel;
                a.position = newPos;
                a.radius = newRadius;

                b.active = false;
            }
        }
    }
}
