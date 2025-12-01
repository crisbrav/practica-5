#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class GameSimulation;
class GameWidget;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private:
    GameSimulation* simulation;
    GameWidget* widget;
};

#endif // MAINWINDOW_H
