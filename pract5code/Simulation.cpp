#include "Simulation.h"
#include <fstream>
#include <iostream>
#include <cmath>

Simulation::Simulation(double width, double height,
                       double dt_, double totalTime_,
                       double e_)
    : box(width, height),
    dt(dt_),
    totalTime(totalTime_),
    obstacleRestitution(e_)
{
}

void Simulation::addParticle(const Particle& p) {
    particles.push_back(p);
}

void Simulation::addObstacle(const Obstacle& o) {
    obstacles.push_back(o);
}

void Simulation::run(const std::string& outputFile) {
    std::ofstream log(outputFile);
    if (!log) {
        std::cerr << "No se pudo abrir el archivo de salida\n";
        return;
    }

    log << "# numParticles dt totalTime\n";
    log << particles.size() << " " << dt << " " << totalTime << "\n\n";

    double time = 0.0;
    int steps = static_cast<int>(totalTime / dt);

    for (int step = 0; step <= steps; ++step) {
        time = step * dt;

        // 1. Actualizar posiciones
        for (auto& p : particles) {
            if (!p.active) continue;
            p.update(dt);
        }

        // 2. Colisiones con paredes
        for (auto& p : particles) {
            box.handleWallCollision(p, log, time);
        }

        // 3. Colisiones partícula-obstáculo
        handleParticleObstacleCollisions(log, time);

        // 4. Colisiones partícula-partícula (inelásticas, fusión)
        handleParticleParticleCollisions(log, time);

        // 5. Registrar estado
        logState(log, time);
    }

    log.close();
    std::cout << "Simulación terminada. Resultados en " << outputFile << "\n";
}

void Simulation::handleParticleObstacleCollisions(std::ofstream& log, double time) {
    for (std::size_t i = 0; i < particles.size(); ++i) {
        Particle& p = particles[i];
        if (!p.active) continue;

        for (std::size_t j = 0; j < obstacles.size(); ++j) {
            Vec2 normal;
            if (obstacles[j].checkCollision(p, normal)) {
                // Descomponer velocidad en componentes normal y tangencial
                Vec2 v = p.velocity;
                double v_n_scalar = v.dot(normal);
                Vec2 v_n = normal * v_n_scalar;
                Vec2 v_t = v - v_n;

                // Aplicar coeficiente de restitución a la componente normal
                Vec2 v_n_prime = normal * (-obstacleRestitution * v_n_scalar);
                p.velocity = v_n_prime + v_t;

                log << "COLLISION_PO " << time << " " << p.id
                    << " OBSTACLE " << j << "\n";
            }
        }
    }
}

void Simulation::handleParticleParticleCollisions(std::ofstream& log, double time) {
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
                // Colisión completamente inelástica: se fusionan
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

                log << "COLLISION_PP " << time << " "
                    << a.id << " " << b.id
                    << " MERGE_INTO " << a.id << "\n";
            }
        }
    }
}

void Simulation::logState(std::ofstream& log, double time) {
    for (const auto& p : particles) {
        log << "STATE " << time << " "
            << p.id << " "
            << p.position.x << " "
            << p.position.y << " "
            << p.velocity.x << " "
            << p.velocity.y << " "
            << p.mass << " "
            << p.radius << " "
            << (p.active ? 1 : 0) << "\n";
    }
    log << "\n";
}
