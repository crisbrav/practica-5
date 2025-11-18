#ifndef SIMULATION_H
#define SIMULATION_H

#include <vector>
#include <string>
#include "Box.h"
#include "Particle.h"
#include "Obstacle.h"

class Simulation {
public:
    Box box;
    std::vector<Particle> particles;
    std::vector<Obstacle> obstacles;
    double dt;
    double totalTime;
    double obstacleRestitution; // e

    Simulation(double width, double height,
               double dt_, double totalTime_,
               double e_);

    void addParticle(const Particle& p);
    void addObstacle(const Obstacle& o);

    void run(const std::string& outputFile);

private:
    void handleParticleObstacleCollisions(std::ofstream& log, double time);
    void handleParticleParticleCollisions(std::ofstream& log, double time);
    void logState(std::ofstream& log, double time);
};

#endif // SIMULATION_H
