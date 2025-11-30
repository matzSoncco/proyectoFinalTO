#include "Simulador.h"
#include <QDebug> // Para imprimir en consola

Simulador::Simulador(QObject *parent) : QObject(parent), escenario(nullptr) {
    timer = new QTimer(this);
    esActivo = false;

    //conectamos el reloj a tu función loopPrincipal
    connect(timer, &QTimer::timeout, this, &Simulador::loopPrincipal);
}

Simulador::~Simulador() {
    delete escenario;
    //los agentes se deberían limpiar aquí o en el destructor de la ventana,
    //pero por ahora limpiamos el escenario
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
    timer->start(500); //500ms = medio segundo por paso
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
        
        //verificar si ya llegó a la salida
        if (escenario->esSalida(posActual.x(), posActual.y())) {
            agentesEnSalida++;
            continue; //no se mueve más
        }

        //buscar la salida más cercana
        QPoint salida = escenario->getSalidaMasCercana(posActual);

        //calcular el siguiente paso usando PathFinder
        QPoint siguientePaso = PathFinder::calcularSiguientePaso(escenario, posActual, salida);

        //mover al agente
        if (siguientePaso != posActual) {
            agente->setPosicion(siguientePaso);
            alguienSeMovio = true;
        }
    }

    //notificar a la GUI que hubo cambios
    emit mundoActualizado();

    //verificar condición de término
    if (agentesEnSalida == agentes.size()) {
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