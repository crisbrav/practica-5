#ifndef BOX_H
#define BOX_H

#include <fstream>

using namespace std;


class Particle;


class Box
{
public:
    double width;
    double height;

    Box(double w, double h);

    // maneja la colisopn con paredes, escribe en el log si ocurre
    bool handleWallCollision(Particle& p, ofstream& log, double time);
};

#endif // BOX_H
