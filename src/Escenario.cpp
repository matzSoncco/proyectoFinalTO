#include "../include/Escenario.h"
#include <cmath>
#include <limits>

Escenario::Escenario(int f, int c) : filas(f), columnas(c) {
    //inicializa la grid con 0 (Piso)
    grid.resize(filas, std::vector<int>(columnas, 0));
}

void Escenario::setCelda(int x, int y, int tipo) {
    if (x >= 0 && x < filas && y >= 0 && y < columnas) {
        grid[x][y] = tipo;
    }
}

bool Escenario::esTransitable(int x, int y) {
    //es transitable si esta dentro de los límites y NO es pared (1)
    if (x < 0 || x >= filas || y < 0 || y >= columnas) return false;
    // CAMBIO IMPORTANTE: Las salidas (2) también son transitables
    // para permitir que múltiples personas puedan pasar por ellas
    return grid[x][y] != 1;
}

bool Escenario::esSalida(int x, int y) {
    if (x < 0 || x >= filas || y < 0 || y >= columnas) return false;
    return grid[x][y] == 2;
}

QPoint Escenario::getSalidaMasCercana(QPoint origen) {
    //logica simple: buscar la salida con menor distancia euclidiana
    QPoint mejorSalida(-1, -1);
    double menorDistancia = std::numeric_limits<double>::max();

    for (int i = 0; i < filas; ++i) {
        for (int j = 0; j < columnas; ++j) {
            if (grid[i][j] == 2) { //si es salida
                double dist = std::hypot(origen.x() - i, origen.y() - j);
                if (dist < menorDistancia) {
                    menorDistancia = dist;
                    mejorSalida = QPoint(i, j);
                }
            }
        }
    }
    return mejorSalida;
}

bool Escenario::puedeEvacuar(int x, int y) {
    return esSalida(x, y);
}
