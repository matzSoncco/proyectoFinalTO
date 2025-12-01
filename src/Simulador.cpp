#include "../include/Simulador.h"
#include <QDebug> 

Simulador::Simulador(QObject *parent) : QObject(parent), escenario(nullptr) {
    timer = new QTimer(this);
    esActivo = false;
    connect(timer, &QTimer::timeout, this, &Simulador::loopPrincipal);
}

Simulador::~Simulador() {
    delete escenario;
}

void Simulador::cargarEscenario(int filas, int cols) {
    if (escenario) delete escenario;
    escenario = new Escenario(filas, cols);
}

void Simulador::agregarAgente(AgenteBase* agente) {
    agentes.push_back(agente);
}

void Simulador::iniciar() {
    if (!escenario) {
        qDebug() << "Error: No hay escenario cargado.";
        return;
    }
    esActivo = true;
    timer->start(500); 
    qDebug() << "Simulación iniciada.";
}

void Simulador::pausar() {
    esActivo = false;
    timer->stop();
    qDebug() << "Simulación pausada.";
}

void Simulador::loopPrincipal() {
    if (!esActivo) return;

    bool alguienSeMovio = false;
    int agentesEnSalida = 0;

    for (AgenteBase* agente : agentes) {
        QPoint posActual = agente->getPosicion();
        
        // 1. Verificar si ya llegó a la salida
        // Usamos .x() y .y() por ser QPoint
        if (escenario->esSalida(posActual.x(), posActual.y())) {
            agentesEnSalida++;
            continue; 
        }

        // 2. Buscar salida y calcular ruta
        QPoint salida = escenario->getSalidaMasCercana(posActual);
        QPoint siguientePaso = PathFinder::calcularSiguientePaso(escenario, posActual, salida);

        // 3. Mover al agente
        if (siguientePaso != posActual) {
            agente->setPosicion(siguientePaso);
            alguienSeMovio = true; // CORRECCIÓN: Ahora esta variable es útil
        }
    }

    // CORRECCIÓN: Solo notificamos a la GUI si realmente hubo cambios (Optimización)
    if (alguienSeMovio) {
        emit mundoActualizado();
    }

    // CORRECCIÓN: Casting para evitar warning de signed/unsigned
    if (agentesEnSalida == (int)agentes.size()) {
        pausar();
        emit simulacionTerminada();
        qDebug() << "Todos los agentes han evacuado.";
    }
}

Escenario* Simulador::getEscenario() {
    return escenario;
}

const std::vector<AgenteBase*>& Simulador::getAgentes() const {
    return agentes;
}