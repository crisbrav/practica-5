#include "GameWidget.h"
#include <QPainter>
#include <QKeyEvent>
#include <QFont>
#include <QtMath>

GameWidget::GameWidget(GameSimulation* sim, QWidget* parent)
    : QWidget(parent),
    simulation(sim),
    cannonSprite(":/images/Canon.png"),
    rivalSprite(":/images/rival.png"),
    backgroundSprite(":/images/fondo.jpg")
{

    // redimensionamiento sprites

    // Cañón a 80x80 píxeles
    cannonSprite = cannonSprite.scaled(
        80, 80,
        Qt::KeepAspectRatio,
        Qt::FastTransformation
        );

    // Rival a 48x48 píxeles
    rivalSprite = rivalSprite.scaled(
        48, 48,
        Qt::KeepAspectRatio,
        Qt::FastTransformation
        );

    // -----------------------------

    setFocusPolicy(Qt::StrongFocus);
    connect(&timer, &QTimer::timeout, this, &GameWidget::onTick);
    timer.start(16);
}


void GameWidget::onTick() {
    if (simulation) {
        simulation->update();
        update();
    }
}

void GameWidget::paintEvent(QPaintEvent*) {
    if (!simulation) return;

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);

    // Fondo
    if (!backgroundSprite.isNull()) {
        // lo escalamos al tamaño del widget
        p.drawPixmap(rect(), backgroundSprite);
    } else {
        // por si falla la carga, ponemos un color
        p.fillRect(rect(), Qt::black);
    }


    double sx = width()  / simulation->worldWidth;
    double sy = height() / simulation->worldHeight;

    auto toScreenX = [sx](double x) { return x * sx; };
    auto toScreenY = [sy, this](double y) { return height() - y * sy; };

    // Piso
    p.setPen(Qt::white);
    p.drawLine(0, toScreenY(0.0), width(), toScreenY(0.0));

    // Bloques (infraestructura)
    for (const RectBlock& b : simulation->blocks) {
        drawRectBlock(p, b, QColor(255, 230, 180), true);
    }

    // Rivales (sprites)
    drawRival(p, simulation->leftRival);
    drawRival(p, simulation->rightRival);

    // Cañones (sprites)
    drawCannon(p, simulation->leftCannonPos,
               simulation->leftAngleDeg, true);
    drawCannon(p, simulation->rightCannonPos,
               simulation->rightAngleDeg, false);

    // Proyectil
    if (simulation->projectileActive && simulation->projectile.active) {
        double r = simulation->projectile.radius;
        double cx = toScreenX(simulation->projectile.position.x);
        double cy = toScreenY(simulation->projectile.position.y);
        double d  = 2.0 * r * sx;

        p.setBrush(Qt::red);
        p.setPen(Qt::NoPen);
        p.drawEllipse(QRectF(cx - d/2.0, cy - d/2.0, d, d));
    }

    // HUD
    p.setPen(Qt::white);
    p.setFont(QFont("Arial", 10));


    QString turnText = (simulation->currentTurn == PlayerSide::Left)
                           ? "Turno: Jugador 1"
                           : "Turno: Jugador 2";
    p.drawText(width()/2 - 60, 20, turnText);

    p.drawText(10, 40,
               QString("Ángulo: %1°").arg(simulation->getCurrentAngleDeg(), 0, 'f', 1));
    p.drawText(10, 55,
               QString("Potencia: %1").arg(simulation->getCurrentPower(), 0, 'f', 1));

    if (simulation->gameOver) {
        p.setFont(QFont("Arial", 24, QFont::Bold));
        p.setPen(Qt::red);
        QString msg = (simulation->winner == PlayerSide::Left)
                          ? "GANADOR: JUGADOR 1"
                          : "GANADOR: JUGADOR 2";
        p.drawText(rect(), Qt::AlignCenter, msg);
    }
}

void GameWidget::drawRectBlock(QPainter& p, const RectBlock& b,
                               const QColor& color, bool drawResistance) {
    if (b.destroyed || b.resistance <= 0.0) return;

    double sx = width()  / simulation->worldWidth;
    double sy = height() / simulation->worldHeight;

    double x = b.x * sx;
    double y = height() - (b.y + b.height) * sy;
    double w = b.width  * sx;
    double h = b.height * sy;

    p.setBrush(color);
    p.setPen(Qt::white);
    p.drawRect(QRectF(x, y, w, h));

    if (drawResistance) {
        p.setFont(QFont("Arial", 10, QFont::Bold));
        p.drawText(QRectF(x, y, w, h),
                   Qt::AlignCenter,
                   QString::number((int)std::round(b.resistance)));
    }
}

void GameWidget::drawRival(QPainter& p, const RectBlock& rival) {
    if (rivalSprite.isNull()) return;

    double sx = width()  / simulation->worldWidth;
    double sy = height() / simulation->worldHeight;

    double centerX = (rival.x + rival.width / 2.0) * sx;
    double bottomY = height() - rival.y * sy;

    int w = rivalSprite.width();
    int h = rivalSprite.height();

    int x = int(centerX - w / 2);
    int y = int(bottomY - h);   // que toque el “suelo” del bloque

    p.drawPixmap(x, y, w, h, rivalSprite);
}


void GameWidget::drawCannon(QPainter& p, const Vec2& pos,
                            double angleDeg, bool leftSide) {
    if (cannonSprite.isNull()) return;

    double sx = width()  / simulation->worldWidth;
    double sy = height() / simulation->worldHeight;

    double baseX = pos.x * sx;
    double baseY = height() - pos.y * sy;

    // plataforma
    p.setBrush(Qt::gray);
    p.setPen(Qt::white);
    p.drawRect(QRectF(baseX - 25, baseY + 10, 50, 8));

    // sprite (ya viene redimensionado)
    QPixmap sprite = cannonSprite;
    if (!leftSide) {
        sprite = cannonSprite.transformed(QTransform().scale(1, -1));
    }

    int w = sprite.width();
    int h = sprite.height();

    p.save();
    p.translate(baseX, baseY);

    double drawAngle = angleDeg;
    if (!leftSide) {
        drawAngle = 180.0 - angleDeg;
    }
    p.rotate(-drawAngle);

    int x = -int(w * 0.2);
    int y = -h / 2;
    p.drawPixmap(x, y, w, h, sprite);

    p.restore();
}


void GameWidget::keyPressEvent(QKeyEvent* e) {
    if (!simulation || simulation->gameOver) {
        QWidget::keyPressEvent(e);
        return;
    }

    switch (e->key()) {
    case Qt::Key_Up:
        simulation->changeAngle(2.0);
        break;
    case Qt::Key_Down:
        simulation->changeAngle(-2.0);
        break;
    case Qt::Key_Left:
        simulation->changePower(-5.0);
        break;
    case Qt::Key_Right:
        simulation->changePower(5.0);
        break;
    case Qt::Key_Space:
        simulation->fireCurrentPlayer();
        break;
    default:
        QWidget::keyPressEvent(e);
        return;
    }

    update();
}
