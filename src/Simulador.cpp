#include "../include/Simulador.h"
#include "../include/Persona.h"
#include "../include/Rescatista.h"
#include <QDebug>
#include <iostream>

Simulador::Simulador(QObject *parent)
    : QObject(parent), escenario(nullptr), esActivo(false), tiempoSimulacion(0.0),
    ticksSinMovimiento(0), maxTicksSinMovimiento(10) {
    timer = new QTimer(this);
    estadisticas = new EstadisticasSimulacion();
    connect(timer, &QTimer::timeout, this, &Simulador::loopPrincipal);
}

Simulador::~Simulador() {
    delete escenario;
    delete estadisticas;
}

void Simulador::cargarEscenario(int filas, int cols) {
    if (escenario) delete escenario;
    escenario = new Escenario(filas, cols);
}

void Simulador::agregarAgente(std::shared_ptr<AgenteBase> agente) {
    agentes.push_back(agente);
    pasosPorAgente[agente->getId()] = 0;
    posicionAnterior[agente->getId()] = agente->getPosicion();

    // NUEVO: Si es una Persona, asignarle referencia al escenario
    if (auto persona = std::dynamic_pointer_cast<Persona>(agente)) {
        persona->setEscenario(escenario);
    }
}

void Simulador::iniciar() {
    if (!escenario) {
        qDebug() << "Error: No hay escenario cargado.";
        return;
    }

    if (agentes.empty()) {
        qDebug() << "Advertencia: No hay agentes en el escenario.";
    }

    esActivo = true;
    tiempoSimulacion = 0.0;
    ticksSinMovimiento = 0;

    // Inicializar estadísticas
    estadisticas->iniciar(agentes.size(), escenario->filas, escenario->columnas);

    // Registrar estado inicial de los agentes
    estadisticas->actualizarEstadoAgentes(agentes);

    // Iniciar temporizador de simulación
    tiempoTranscurrido.start();
    timer->start(500); // 500ms = 0.5 segundos por tick

    qDebug() << "✅ Simulación iniciada con" << agentes.size() << "agentes.";
    qDebug() << "📍 Escenario:" << escenario->filas << "x" << escenario->columnas;
}

void Simulador::pausar() {
    esActivo = false;
    timer->stop();
    qDebug() << "Simulación pausada.";
}

void Simulador::reiniciar() {
    pausar();
    agentes.clear();
    pasosPorAgente.clear();
    posicionAnterior.clear();
    tiempoSimulacion = 0.0;
    ticksSinMovimiento = 0;
    estadisticas->reiniciar();
    qDebug() << "Simulación reiniciada.";
}

void Simulador::loopPrincipal() {
    if (!esActivo) return;

    // Actualizar tiempo de simulación
    tiempoSimulacion = tiempoTranscurrido.elapsed() / 1000.0; // Convertir a segundos
    estadisticas->actualizarTiempoSimulacion(tiempoSimulacion);

    bool alguienSeMovio = false;
    int agentesEvacuadosEnEsteFrame = 0;

    // CAMBIO CRÍTICO: Usar iterador para poder eliminar durante el recorrido
    for (auto it = agentes.begin(); it != agentes.end(); ) {
        std::shared_ptr<AgenteBase> agente_ptr = *it;
        AgenteBase* agente_raw = agente_ptr.get();
        QPoint posActual = agente_raw->getPosicion();
        int agenteId = agente_raw->getId();

        // 1. NUEVO: Verificar si ya está evacuado (para eliminarlo)
        if (agente_raw->getEstado() == EstadoAgente::EVACUADO) {
            // Ya fue evacuado en un frame anterior, eliminarlo
            pasosPorAgente.erase(agenteId);
            posicionAnterior.erase(agenteId);
            it = agentes.erase(it);
            agentesEvacuadosEnEsteFrame++;
            continue;
        }

        // 2. Verificar si llegó a la salida AHORA
        if (escenario->esSalida(posActual.x(), posActual.y())) {
            // Registrar evacuación
            agente_raw->setEstado(EstadoAgente::EVACUADO);
            estadisticas->registrarEvacuacion(
                agente_ptr,
                posActual,
                tiempoSimulacion,
                pasosPorAgente[agenteId]
                );
            qDebug() << "🚪 Agente" << agenteId << "evacuado en" << tiempoSimulacion << "s con" << pasosPorAgente[agenteId] << "pasos";

            // IMPORTANTE: Eliminar inmediatamente
            pasosPorAgente.erase(agenteId);
            posicionAnterior.erase(agenteId);
            it = agentes.erase(it);
            agentesEvacuadosEnEsteFrame++;
            continue;
        }

        // 3. Buscar salida y calcular ruta
        QPoint salida = escenario->getSalidaMasCercana(posActual);

        // Verificar que la salida es válida
        if (salida.x() == -1 || salida.y() == -1) {
            qDebug() << "⚠️  Agente" << agenteId << "no puede encontrar salida";
            agente_raw->setEstado(EstadoAgente::BLOQUEADO);
            ++it;
            continue;
        }

        QPoint siguientePaso = PathFinder::calcularSiguientePaso(escenario, posActual, salida);

        // 4. Mover al agente
        if (siguientePaso != posActual) {
            // Verificar colisiones con otros agentes
            bool colision = false;
            for (const auto& otro : agentes) {
                if (otro->getId() != agenteId && otro->getPosicion() == siguientePaso) {
                    // MEJORA: No contar como colisión si el otro está evacuado
                    if (otro->getEstado() != EstadoAgente::EVACUADO) {
                        colision = true;
                        estadisticas->registrarColision(siguientePaso);
                        break;
                    }
                }
            }

            if (!colision) {
                // Registrar movimiento
                estadisticas->registrarMovimiento(agente_ptr, posActual, siguientePaso);

                agente_raw->setPosicion(siguientePaso);
                pasosPorAgente[agenteId]++;
                posicionAnterior[agenteId] = siguientePaso;
                alguienSeMovio = true;

                // Actualizar estado del agente
                if (agente_raw->getEstado() != EstadoAgente::EVACUANDO) {
                    agente_raw->setEstado(EstadoAgente::EVACUANDO);
                }

                // NUEVO: Verificar si llegó a salida después de moverse
                if (escenario->esSalida(siguientePaso.x(), siguientePaso.y())) {
                    agente_raw->setEstado(EstadoAgente::EVACUADO);
                    estadisticas->registrarEvacuacion(
                        agente_ptr,
                        siguientePaso,
                        tiempoSimulacion,
                        pasosPorAgente[agenteId]
                        );
                    qDebug() << "🚪 Agente" << agenteId << "evacuado tras moverse en" << tiempoSimulacion << "s";

                    // Eliminar inmediatamente
                    pasosPorAgente.erase(agenteId);
                    posicionAnterior.erase(agenteId);
                    it = agentes.erase(it);
                    agentesEvacuadosEnEsteFrame++;
                    continue;
                }
            } else {
                // Incrementar pánico si hay colisión (opcional)
                if (auto persona = std::dynamic_pointer_cast<Persona>(agente_ptr)) {
                    persona->incrementarPanico(0.1);
                }
            }
        }

        ++it;
    }

    // Actualizar estado de los agentes restantes en estadísticas
    estadisticas->actualizarEstadoAgentes(agentes);

    // Notificar a la GUI
    emit mundoActualizado();

    // Emitir resumen de estadísticas
    QString resumen = QString::fromStdString(estadisticas->getResumenRapido());
    emit estadisticasActualizadas(resumen);

    // Detectar si no hay movimiento (agentes atrapados)
    if (!alguienSeMovio && !agentes.empty()) {
        ticksSinMovimiento++;

        // Detectar agentes bloqueados
        for (const auto& agente_ptr : agentes) {
            if (agente_ptr->getEstado() != EstadoAgente::EVACUADO) {
                agente_ptr->setEstado(EstadoAgente::BLOQUEADO);
            }
        }

        if (ticksSinMovimiento >= maxTicksSinMovimiento) {
            qDebug() << "⚠️  ADVERTENCIA: Simulación estancada. Agentes bloqueados o sin salida.";
            qDebug() << "⚠️  Agentes restantes:" << agentes.size();
            pausar();
            estadisticas->calcularEstadisticas();
            emit simulacionTerminada();
            mostrarEstadisticas();
            return;
        }
    } else {
        ticksSinMovimiento = 0;
    }

    // Verificar si todos evacuaron (la lista está vacía)
    if (agentes.empty()) {
        pausar();
        estadisticas->calcularEstadisticas();
        emit simulacionTerminada();
        qDebug() << "✅ ¡Todos los agentes han evacuado exitosamente!";
        mostrarEstadisticas();
    }
}

Escenario* Simulador::getEscenario() {
    return escenario;
}

const std::vector<std::shared_ptr<AgenteBase>>& Simulador::getAgentes() const {
    return agentes;
}

EstadisticasSimulacion* Simulador::getEstadisticas() {
    return estadisticas;
}

void Simulador::exportarEstadisticas(const std::string& rutaArchivo) {
    estadisticas->calcularEstadisticas();
    if (estadisticas->exportarReporte(rutaArchivo)) {
        qDebug() << "Estadísticas exportadas a:" << QString::fromStdString(rutaArchivo);
    } else {
        qDebug() << "Error al exportar estadísticas";
    }
}

void Simulador::mostrarEstadisticas() {
    // Actualizar estado actual antes de calcular
    estadisticas->actualizarEstadoAgentes(agentes);
    estadisticas->actualizarTiempoSimulacion(tiempoSimulacion);
    estadisticas->calcularEstadisticas();

    std::string reporte = estadisticas->generarReporte();
    std::cout << "\n" << reporte << "\n" << std::endl;

    // También mostrar en qDebug para que aparezca en la consola de Qt
    qDebug() << "📊 ESTADÍSTICAS GENERADAS";
    qDebug() << "Total agentes:" << estadisticas->getEstadisticas().totalAgentes;
    qDebug() << "Evacuados:" << estadisticas->getEstadisticas().totalEvacuados;
    qDebug() << "Restantes:" << agentes.size();
}
