#ifndef ESCENARIO_H
#define ESCENARIO_H

#include <vector>
#include <QPoint>

class Escenario {
public:
    int filas;
    int columnas;
    bool puedeEvacuar(int x, int y);
    //0 = Piso, 1 = Pared, 2 = Salida
    std::vector<std::vector<int>> grid;

    Escenario(int f, int c);
    void setCelda(int x, int y, int tipo);
    bool esTransitable(int x, int y);
    bool esSalida(int x, int y);
    QPoint getSalidaMasCercana(QPoint origen);
};

#endif
