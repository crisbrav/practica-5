#ifndef GAMEWIDGET_H
#define GAMEWIDGET_H

#include <QWidget>
#include <QTimer>
#include <QPixmap>
#include "GameSimulation.h"

class GameWidget : public QWidget {
    Q_OBJECT
public:
    explicit GameWidget(GameSimulation* sim, QWidget* parent = nullptr);

protected:
    void paintEvent(QPaintEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

private slots:
    void onTick();

private:
    GameSimulation* simulation;
    QTimer timer;

    QPixmap cannonSprite;
    QPixmap rivalSprite;
    QPixmap backgroundSprite;
    void drawRectBlock(QPainter& p, const RectBlock& b,
                       const QColor& color, bool drawResistance);
    void drawRival(QPainter& p, const RectBlock& rival);
    void drawCannon(QPainter& p, const Vec2& pos,
                    double angleDeg, bool leftSide);
};

#endif // GAMEWIDGET_H
