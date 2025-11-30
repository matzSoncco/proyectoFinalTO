#ifndef PATHFINDER_H
#define PATHFINDER_H

#include "Escenario.h"
#include <list>
#include <QPoint>

class PathFinder {
public:
    //retorna el siguiente punto al que debe moverse el agente
    static QPoint calcularSiguientePaso(Escenario* mapa, QPoint inicio, QPoint fin);
};

#endif