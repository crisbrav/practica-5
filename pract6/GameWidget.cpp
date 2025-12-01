#include "GameWidget.h"
#include <QPainter>
#include <QKeyEvent>
#include <QFont>
#include <QDebug>

GameWidget::GameWidget(GameSimulation* sim, QWidget* parent)
    : QWidget(parent),
    simulation(sim)
{
    setFocusPolicy(Qt::StrongFocus);
    // Timer para ~60 FPS
    connect(&timer, &QTimer::timeout, this, &GameWidget::onUpdate);
    timer.start(16); // 16 ms aprox
}

void GameWidget::onUpdate() {
    if (!simulation) return;

    simulation->step();
    update();
}

void GameWidget::paintEvent(QPaintEvent* /*event*/) {
    if (!simulation) return;

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    // Escalamos coordenadas físicas al tamaño del widget
    double sx = width() / simulation->box.width;
    double sy = height() / simulation->box.height;

    // Fondo
    painter.fillRect(rect(), Qt::black);

    // Caja (borde)
    painter.setPen(Qt::white);
    painter.drawRect(0, 0, width()-1, height()-1);

    // Obstáculos (cuadrados)
    painter.setBrush(Qt::darkGray);
    painter.setPen(Qt::NoPen);
    for (const auto& o : simulation->getObstacles()) {
        double x = (o.center.x - o.halfSize) * sx;
        double y = (simulation->box.height - (o.center.y + o.halfSize)) * sy;
        double size = (o.halfSize * 2.0) * sx; // asumimos sx ~ sy
        painter.drawRect(QRectF(x, y, size, size));
    }

    // Partículas
    const auto& ps = simulation->getParticles();
    for (std::size_t i = 0; i < ps.size(); ++i) {
        const Particle& p = ps[i];
        if (!p.active) continue;

        double x = (p.position.x - p.radius) * sx;
        double y = (simulation->box.height - (p.position.y + p.radius)) * sy;
        double d = (p.radius * 2.0) * sx;

        if ((int)i == simulation->playerIndex) {
            painter.setBrush(Qt::green); // jugador
        } else {
            painter.setBrush(Qt::red);   // enemigos
        }
        painter.setPen(Qt::NoPen);
        painter.drawEllipse(QRectF(x, y, d, d));
    }

    // Mensaje de Game Over
    if (simulation->isGameOver()) {
        painter.setPen(Qt::yellow);
        painter.setFont(QFont("Arial", 24, QFont::Bold));
        painter.drawText(rect(), Qt::AlignCenter, "GAME OVER");
    }
}

void GameWidget::keyPressEvent(QKeyEvent* event) {
    if (!simulation) return;

    Particle* player = simulation->getPlayer();
    if (!player) return;

    double speed = 50.0; // cambio de velocidad

    switch (event->key()) {
    case Qt::Key_Left:
        player->velocity.x -= speed;
        break;
    case Qt::Key_Right:
        player->velocity.x += speed;
        break;
    case Qt::Key_Up:
        player->velocity.y += speed;  // Y positivo hacia arriba en coordenadas físicas
        break;
    case Qt::Key_Down:
        player->velocity.y -= speed;
        break;
    default:
        QWidget::keyPressEvent(event);
        break;
    }
}
