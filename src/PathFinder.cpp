#include "../include/PathFinder.h"
#include <queue>
#include <map>
#include <algorithm>

//usamos una estructura simple para comparar QPoints en el mapa de padres
struct PointCompare {
    bool operator()(const QPoint& a, const QPoint& b) const {
        if (a.x() != b.x()) return a.x() < b.x();
        return a.y() < b.y();
    }
};

QPoint PathFinder::calcularSiguientePaso(Escenario* mapa, QPoint inicio, QPoint fin) {
    if (inicio == fin) return inicio;

    //cola para el BFS
    std::queue<QPoint> frontera;
    frontera.push(inicio);

    //mapa para reconstruir el camino: hijo -> padre
    std::map<QPoint, QPoint, PointCompare> padres;
    padres[inicio] = QPoint(-1, -1); //marca de inicio

    bool encontrado = false;

    //direcciones: arriba, abajo, izquierda, derecha
    int dx[] = {-1, 1, 0, 0};
    int dy[] = {0, 0, -1, 1};

    while (!frontera.empty()) {
        QPoint actual = frontera.front();
        frontera.pop();

        if (actual == fin) {
            encontrado = true;
            break;
        }

        //explorar vecinos
        for (int i = 0; i < 4; ++i) {
            QPoint vecino(actual.x() + dx[i], actual.y() + dy[i]);

            //si es transitable y no lo hemos visitado aun
            if (mapa->esTransitable(vecino.x(), vecino.y()) && 
                padres.find(vecino) == padres.end()) {
                
                frontera.push(vecino);
                padres[vecino] = actual; //guardamos de donde vinimos
            }
        }
    }

    if (!encontrado) return inicio; //no hay camino, nos quedamos quietos

    //reconstruir el camino desde el FIN hacia el INICIO
    QPoint paso = fin;
    while (padres[paso] != inicio) {
        paso = padres[paso];
        //seguridad para evitar bucles infinitos si algo falla
        if (paso == QPoint(-1, -1)) return inicio; 
    }

    return paso; //este es el primer paso inmediato que debe dar el agente
}