#ifndef GAMESIMULATION_H
#define GAMESIMULATION_H

#include <vector>
#include "Vec2.h"
#include "Particle.h"

enum class PlayerSide {
    Left,
    Right
};

// Obstáculo rectangular (infraestructura o rival)
struct RectBlock {
    double x;      // esquina inferior izquierda
    double y;
    double width;
    double height;

    double resistance;      // "vida" del bloque
    bool destroyed;
    PlayerSide owner;       // dueño del bloque (izq / der)

    RectBlock(double x_=0, double y_=0, double w_=0, double h_=0,
              double res_=0, PlayerSide own_=PlayerSide::Left)
        : x(x_), y(y_), width(w_), height(h_),
        resistance(res_), destroyed(false), owner(own_) {}
};

class GameSimulation {
public:
    // Mundo (la "caja" del escenario)
    double worldWidth;
    double worldHeight;

    double dt;
    double gravity;              // aceleración en Y (negativa)
    double restitutionWalls;     // e = 1.0 (elástica)
    double restitutionInfra;     // e < 1 (inelástica)
    double damageFactor;         // factor constante para el daño

    PlayerSide currentTurn;

    // Proyectil
    Particle projectile;
    bool projectileActive;
    double projectileRadius;
    double projectileMass;

    // Parámetros de disparo de cada jugador
    Vec2 leftCannonPos;
    Vec2 rightCannonPos;
    double leftAngleDeg;
    double rightAngleDeg;
    double leftPower;
    double rightPower;

    // Puntuación opcional (acumulada por daño)
    double leftScore;
    double rightScore;

    // Infraestructura
    std::vector<RectBlock> blocks;

    // Representantes ("Rival")
    RectBlock leftRival;
    RectBlock rightRival;
    double rivalHealthLeft;
    double rivalHealthRight;

    bool gameOver;
    PlayerSide winner;

    // Control de tiempo del disparo (para no dejar el proyectil rebotando infinito)
    double shotTime;
    double maxShotTime;

    GameSimulation(double width, double height);

    // Avanza un paso de la simulación (si hay proyectil activo)
    void update();

    // Dispara el proyectil del jugador del turno actual
    void fireCurrentPlayer();

    // Ajustes de ángulo y potencia para el jugador actual
    void changeAngle(double deltaDeg);
    void changePower(double delta);

    double getCurrentAngleDeg() const;
    double getCurrentPower() const;

private:
    bool circleIntersectsRect(const Particle& p, const RectBlock& r) const;
    bool circleIntersectsRectWithNormal(const Particle& p,
                                        const RectBlock& r,
                                        Vec2& outNormal) const;

    void handleProjectileWallCollisions();
    void handleProjectileInfraCollisions();
    void handleProjectileRivalCollisions();
    void endShotAndChangeTurn();
};

#endif // GAMESIMULATION_H
