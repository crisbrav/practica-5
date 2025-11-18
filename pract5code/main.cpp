#include <iostream>
#include "Simulation.h"
#include "Particle.h"
#include "Vec2.h"
#include "Obstacle.h"

int main() {
    // Parámetros de la simulación
    double width = 200.0;
    double height = 100.0;
    double dt = 0.01;
    double totalTime = 5.0;
    double e_obstaculo = 0.6;    // coeficiente de restitución inelástico

    Simulation sim(width, height, dt, totalTime, e_obstaculo);

    // Partículas iniciales
    // id, position(x,y), velocity(vx,vy), mass, radius
    sim.addParticle(Particle(0, Vec2(20.0, 20.0), Vec2(10.0, 5.0), 1.0, 2.0));
    sim.addParticle(Particle(1, Vec2(50.0, 40.0), Vec2(-8.0, -3.0), 1.5, 2.5));
    sim.addParticle(Particle(2, Vec2(120.0, 60.0), Vec2(-5.0, 2.0), 1.2, 2.0));
    sim.addParticle(Particle(3, Vec2(80.0, 30.0), Vec2(3.0, -6.0), 0.8, 1.8));

    // Obstáculos cuadrados
    sim.addObstacle(Obstacle(Vec2(40.0, 50.0), 5.0));
    sim.addObstacle(Obstacle(Vec2(80.0, 70.0), 5.0));
    sim.addObstacle(Obstacle(Vec2(120.0, 30.0), 5.0));
    sim.addObstacle(Obstacle(Vec2(160.0, 50.0), 5.0));

    sim.run("simulacion.txt");

    return 0;
}
