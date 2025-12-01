#include "GameSimulation.h"
#include <algorithm>
#include <cmath>

GameSimulation::GameSimulation(double width, double height)
    : worldWidth(width),
    worldHeight(height),
    dt(0.016),
    gravity(-9.8 * 30.0),   // gravedad escalada
    restitutionWalls(1.0),  // colisiones elásticas con la caja
    restitutionInfra(0.6),  // colisiones inelásticas con infraestructura
    damageFactor(0.08),
    currentTurn(PlayerSide::Left),
    projectile(),
    projectileActive(false),
    projectileRadius(8.0),
    projectileMass(1.0),
    leftCannonPos(60.0, 260.0),
    rightCannonPos(width - 60.0, 260.0),
    leftAngleDeg(45.0),
    rightAngleDeg(45.0),
    leftPower(140.0),
    rightPower(140.0),
    leftScore(0.0),
    rightScore(0.0),
    leftRival(),
    rightRival(),
    rivalHealthLeft(100.0),
    rivalHealthRight(100.0),
    gameOver(false),
    winner(PlayerSide::Left),
    shotTime(0.0),
    maxShotTime(8.0)        // tiempo proyectil en scena
{

    double buildingWidth = 160.0;
    double topHeight     = 40.0;
    double colHeight     = 120.0;
    double colWidth      = buildingWidth / 3.0;
    double baseY         = 0.0;

    double leftCenterX  = 200.0;
    double rightCenterX = worldWidth - 200.0;

    // Bloques superiores (resistencia 100)
    RectBlock leftTop(
        leftCenterX - buildingWidth / 2.0,
        baseY + colHeight,
        buildingWidth,
        topHeight,
        100.0,
        PlayerSide::Left
        );
    RectBlock rightTop(
        rightCenterX - buildingWidth / 2.0,
        baseY + colHeight,
        buildingWidth,
        topHeight,
        100.0,
        PlayerSide::Right
        );

    // Columnas laterales (resistencia 200)
    RectBlock leftColL(
        leftCenterX - buildingWidth / 2.0,
        baseY,
        colWidth,
        colHeight,
        200.0,
        PlayerSide::Left
        );
    RectBlock leftColR(
        leftCenterX + buildingWidth / 2.0 - colWidth,
        baseY,
        colWidth,
        colHeight,
        200.0,
        PlayerSide::Left
        );

    RectBlock rightColL(
        rightCenterX - buildingWidth / 2.0,
        baseY,
        colWidth,
        colHeight,
        200.0,
        PlayerSide::Right
        );
    RectBlock rightColR(
        rightCenterX + buildingWidth / 2.0 - colWidth,
        baseY,
        colWidth,
        colHeight,
        200.0,
        PlayerSide::Right
        );

    blocks.push_back(leftTop);
    blocks.push_back(leftColL);
    blocks.push_back(leftColR);
    blocks.push_back(rightTop);
    blocks.push_back(rightColL);
    blocks.push_back(rightColR);

    // Rivales: rectángulo central (sin resistencia propia: se maneja con rivalHealth)
    double rivalWidth  = colWidth;
    double rivalHeight = colHeight;

    leftRival = RectBlock(
        leftCenterX - rivalWidth / 2.0,
        baseY,
        rivalWidth,
        rivalHeight,
        0.0,
        PlayerSide::Left
        );

    rightRival = RectBlock(
        rightCenterX - rivalWidth / 2.0,
        baseY,
        rivalWidth,
        rivalHeight,
        0.0,
        PlayerSide::Right
        );
}

void GameSimulation::update() {
    if (gameOver) return;
    if (!projectileActive || !projectile.active) return;

    shotTime += dt;

    // Actualizar velocidad por gravedad y posición
    projectile.velocity.y += gravity * dt;
    projectile.update(dt);

    handleProjectileWallCollisions();
    handleProjectileInfraCollisions();
    handleProjectileRivalCollisions();

    // Fin del disparo por tiempo o velocidad muy baja
    double speed = std::sqrt(projectile.velocity.x * projectile.velocity.x +
                             projectile.velocity.y * projectile.velocity.y);
    if (shotTime > maxShotTime || speed < 5.0) {
        endShotAndChangeTurn();
    }
}

void GameSimulation::fireCurrentPlayer() {
    if (projectileActive || gameOver) return;

    double angleDeg = (currentTurn == PlayerSide::Left) ? leftAngleDeg : rightAngleDeg;
    double power    = (currentTurn == PlayerSide::Left) ? leftPower    : rightPower;
    Vec2   cannonPos = (currentTurn == PlayerSide::Left) ? leftCannonPos : rightCannonPos;

    double angleRad = angleDeg * M_PI / 180.0;
    Vec2 dir(std::cos(angleRad), std::sin(angleRad));

    // El cañón derecho dispara hacia la izquierda
    if (currentTurn == PlayerSide::Right) {
        dir.x = -dir.x;
    }

    Vec2 vel = dir * power;
    Vec2 startPos = cannonPos + dir * 20.0; // un poco adelante del cañón

    projectile.id = 0;
    projectile.position = startPos;
    projectile.velocity = vel;
    projectile.mass = projectileMass;
    projectile.radius = projectileRadius;
    projectile.active = true;

    projectileActive = true;
    shotTime = 0.0;
}

void GameSimulation::changeAngle(double deltaDeg) {
    double* angle = (currentTurn == PlayerSide::Left) ? &leftAngleDeg : &rightAngleDeg;
    *angle += deltaDeg;
    if (*angle < 5.0)  *angle = 5.0;
    if (*angle > 85.0) *angle = 85.0;
}

void GameSimulation::changePower(double delta) {
    double* power = (currentTurn == PlayerSide::Left) ? &leftPower : &rightPower;
    *power += delta;
    if (*power < 30.0)  *power = 30.0;
    if (*power > 350) *power = 350.0;
}

double GameSimulation::getCurrentAngleDeg() const {
    return (currentTurn == PlayerSide::Left) ? leftAngleDeg : rightAngleDeg;
}

double GameSimulation::getCurrentPower() const {
    return (currentTurn == PlayerSide::Left) ? leftPower : rightPower;
}

// Intersección círculo-rectángulo (sin normal)
bool GameSimulation::circleIntersectsRect(const Particle& p, const RectBlock& r) const {
    double closestX = std::max(r.x, std::min(p.position.x, r.x + r.width));
    double closestY = std::max(r.y, std::min(p.position.y, r.y + r.height));

    double dx = p.position.x - closestX;
    double dy = p.position.y - closestY;
    double dist2 = dx*dx + dy*dy;

    return dist2 <= p.radius * p.radius;
}

// aproximacion de devolucion de la normal
bool GameSimulation::circleIntersectsRectWithNormal(const Particle& p,
                                                    const RectBlock& r,
                                                    Vec2& outNormal) const {
    double closestX = std::max(r.x, std::min(p.position.x, r.x + r.width));
    double closestY = std::max(r.y, std::min(p.position.y, r.y + r.height));

    double dx = p.position.x - closestX;
    double dy = p.position.y - closestY;
    double dist2 = dx*dx + dy*dy;

    if (dist2 > p.radius * p.radius) {
        return false;
    }

    // Si entra exactamente por un vertice, usamos vector radial
    if (dist2 > 0.0) {
        Vec2 n(dx, dy);
        outNormal = n.normalized();
        return true;
    }

    // Si estamos pegados al borde, elegimos la cara más cercana
    double leftDist   = std::abs(p.position.x - r.x);
    double rightDist  = std::abs(p.position.x - (r.x + r.width));
    double bottomDist = std::abs(p.position.y - r.y);
    double topDist    = std::abs(p.position.y - (r.y + r.height));

    double minDist = std::min(std::min(leftDist, rightDist),
                              std::min(bottomDist, topDist));

    if (minDist == leftDist)      outNormal = Vec2(-1.0, 0.0);
    else if (minDist == rightDist) outNormal = Vec2(1.0, 0.0);
    else if (minDist == bottomDist) outNormal = Vec2(0.0, -1.0);
    else                           outNormal = Vec2(0.0, 1.0);

    return true;
}

// Colisiones del proyectil con las paredes de la caja (perfectamente elásticas)
void GameSimulation::handleProjectileWallCollisions() {
    if (!projectileActive) return;

    bool collided = false;

    // LEFT / RIGHT
    if (projectile.position.x - projectile.radius < 0.0) {
        projectile.position.x = projectile.radius;
        projectile.velocity.x = -projectile.velocity.x * restitutionWalls;
        collided = true;
    } else if (projectile.position.x + projectile.radius > worldWidth) {
        projectile.position.x = worldWidth - projectile.radius;
        projectile.velocity.x = -projectile.velocity.x * restitutionWalls;
        collided = true;
    }

    // FLOOR / CEILING
    if (projectile.position.y - projectile.radius < 0.0) {
        projectile.position.y = projectile.radius;
        projectile.velocity.y = -projectile.velocity.y * restitutionWalls;
        collided = true;
    } else if (projectile.position.y + projectile.radius > worldHeight) {
        projectile.position.y = worldHeight - projectile.radius;
        projectile.velocity.y = -projectile.velocity.y * restitutionWalls;
        collided = true;
    }

    if (collided) {
        // colisión elástica: solo invertimos la componente normal,
        // ya está hecho arriba con restitutionWalls = 1.0
    }
}

// Colisiones con infraestructura (inelásticas + daño)
void GameSimulation::handleProjectileInfraCollisions() {
    if (!projectileActive) return;

    for (RectBlock& b : blocks) {
        if (b.destroyed || b.resistance <= 0.0)
            continue;

        Vec2 normal;
        if (!circleIntersectsRectWithNormal(projectile, b, normal))
            continue;

        // Cálculo de daño: factor * masa * |v|
        double speed = std::sqrt(projectile.velocity.x * projectile.velocity.x +
                                 projectile.velocity.y * projectile.velocity.y);
        double damage = damageFactor * projectile.mass * speed;


        b.resistance -= damage;
        if (b.resistance <= 0.0) {
            b.destroyed = true;
        }

        // Rebote inelástico: aplicamos e a la componente normal
        Vec2 v = projectile.velocity;
        double v_n_scalar = v.dot(normal);
        Vec2 v_n = normal * v_n_scalar;
        Vec2 v_t = v - v_n;

        Vec2 v_n_prime = normal * (-restitutionInfra * v_n_scalar);
        projectile.velocity = v_n_prime + v_t;

        // Pequeña corrección de posición hacia afuera del bloque
        projectile.position += normal * (projectile.radius * 0.2);

        // Permitimos que siga volando y pueda golpear más cosas
    }
}

// Colisión contra el representante del rival
void GameSimulation::handleProjectileRivalCollisions() {
    if (!projectileActive) return;

    bool hitLeft  = circleIntersectsRect(projectile, leftRival);
    bool hitRight = circleIntersectsRect(projectile, rightRival);

    if (!hitLeft && !hitRight) return;

    // hit del muñecco
    if (hitLeft)  rivalHealthLeft  -= 100.0;
    if (hitRight) rivalHealthRight -= 100.0;

    if (rivalHealthLeft <= 0.0) {
        gameOver = true;
        winner = PlayerSide::Right;
        projectileActive = false;
        projectile.active = false;
    } else if (rivalHealthRight <= 0.0) {
        gameOver = true;
        winner = PlayerSide::Left;
        projectileActive = false;
        projectile.active = false;
    } else {
        // si por alguna razón no muere, simplemente acaba el disparo
        endShotAndChangeTurn();
    }
}

void GameSimulation::endShotAndChangeTurn() {
    projectileActive = false;
    projectile.active = false;
    shotTime = 0.0;

    if (gameOver) return;

    currentTurn = (currentTurn == PlayerSide::Left)
                      ? PlayerSide::Right
                      : PlayerSide::Left;
}
