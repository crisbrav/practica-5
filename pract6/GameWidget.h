#ifndef GAMEWIDGET_H
#define GAMEWIDGET_H

#include <QWidget>
#include <QTimer>
#include "GameSimulation.h"

class GameWidget : public QWidget {
    Q_OBJECT
public:
    explicit GameWidget(GameSimulation* sim, QWidget* parent = nullptr);

protected:
    void paintEvent(QPaintEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

private slots:
    void onUpdate();

private:
    GameSimulation* simulation;
    QTimer timer;
};

#endif // GAMEWIDGET_H
