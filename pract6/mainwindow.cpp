#include "mainwindow.h"
#include "GameSimulation.h"
#include "GameWidget.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
    simulation(new GameSimulation(800.0, 400.0)),
    widget(new GameWidget(simulation, this))
{
    setCentralWidget(widget);
    resize(800, 400);
    setWindowTitle("Pract 5, juego del cañon");
}

MainWindow::~MainWindow() {
    delete simulation;
}
