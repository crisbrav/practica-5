#ifndef GAMESIMULATION_H
#define GAMESIMULATION_H

#include <vector>
#include "Box.h"
#include "Particle.h"
#include "obstacle.h"

class GameSimulation {
public:
    Box box;
    std::vector<Particle> particles;
    std::vector<Obstacle> obstacles;

    double dt;
    double obstacleRestitution;

    int playerIndex;      // índice de la partícula controlada por el jugador
    bool gameOver;

    GameSimulation(double width, double height,
                   double dt_ = 0.016,
                   double restitution_ = 0.6);

    void step(); // avanza la simulación un paso de dt

    // acceso a partículas/obstáculos
    const std::vector<Particle>& getParticles() const;
    const std::vector<Obstacle>& getObstacles() const;

    Particle* getPlayer();
    const Particle* getPlayer() const;

    bool isGameOver() const;

private:
    void handleWallCollisions();
    void handleParticleObstacleCollisions();
    void handleParticleParticleCollisions();
};

#endif // GAMESIMULATION_H
