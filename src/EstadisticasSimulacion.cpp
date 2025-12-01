#include "EstadisticasSimulacion.h"
#include "Persona.h"
#include "Rescatista.h"
#include <sstream>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <iomanip>
#include <QDebug>

EstadisticasSimulacion::EstadisticasSimulacion() 
    : tiempoActual(0.0), filas(0), columnas(0), simulacionIniciada(false) {
    reiniciar();
}

EstadisticasSimulacion::~EstadisticasSimulacion() {
}

void EstadisticasSimulacion::iniciar(int totalAgentes, int f, int c) {
    reiniciar();
    estadisticas.totalAgentes = totalAgentes;
    estadisticas.totalEnProceso = totalAgentes; // Inicialmente todos en proceso
    filas = f;
    columnas = c;
    simulacionIniciada = true;
    tiempoActual = 0.0;
    
    qDebug() << "📊 Estadísticas inicializadas:" << totalAgentes << "agentes";
}

void EstadisticasSimulacion::reiniciar() {
    estadisticas = EstadisticasGlobales();
    eventosEvacuacion.clear();
    trayectoriasAgentes.clear();
    contadorCuellosBottela.clear();
    tiempoActual = 0.0;
    simulacionIniciada = false;
}

void EstadisticasSimulacion::registrarEvacuacion(std::shared_ptr<AgenteBase> agente, 
                                                  QPoint salida, double tiempo, int pasos) {
    EventoEvacuacion evento;
    evento.agenteId = agente->getId();
    evento.salida = salida;
    evento.tiempoEvacuacion = tiempo;
    evento.pasosRealizados = pasos;
    
    // Determinar el tipo de agente usando dynamic_cast
    Persona* persona = dynamic_cast<Persona*>(agente.get());
    Rescatista* rescatista = dynamic_cast<Rescatista*>(agente.get());
    
    if (rescatista != nullptr) {
        evento.tipoAgente = "Rescatista";
        estadisticas.rescatistasEvacuados++;
        qDebug() << "📊 Rescatista" << agente->getId() << "evacuado";
    } else if (persona != nullptr) {
        evento.tipoAgente = "Persona";
        estadisticas.personasEvacuadas++;
        qDebug() << "📊 Persona" << agente->getId() << "evacuada";
    } else {
        evento.tipoAgente = "Desconocido";
    }
    
    eventosEvacuacion.push_back(evento);
    estadisticas.totalEvacuados++;
    
    // Actualizar estadísticas por salida
    std::string salidaKey = puntoToString(salida);
    estadisticas.personasPorSalida[salidaKey]++;
    
    // Calcular distancia recorrida aproximada
    if (trayectoriasAgentes.find(evento.agenteId) != trayectoriasAgentes.end()) {
        double distancia = 0.0;
        const auto& trayectoria = trayectoriasAgentes[evento.agenteId];
        for (size_t i = 1; i < trayectoria.size(); i++) {
            distancia += calcularDistancia(trayectoria[i-1], trayectoria[i]);
        }
        estadisticas.distanciaPromedioRecorrida += distancia;
    }
    
    qDebug() << "📊 Total evacuados:" << estadisticas.totalEvacuados << "/" << estadisticas.totalAgentes;
}

void EstadisticasSimulacion::registrarColision(QPoint posicion) {
    estadisticas.colisionesTotales++;
    registrarCuelloBotella(posicion);
}

void EstadisticasSimulacion::registrarCuelloBotella(QPoint posicion) {
    std::string key = puntoToString(posicion);
    contadorCuellosBottela[key]++;
    
    // Si una posición tiene muchas colisiones, es un cuello de botella
    if (contadorCuellosBottela[key] > 5) {
        // Verificar si ya está en la lista
        auto it = std::find(estadisticas.cuellosBotellaDetectados.begin(), 
                           estadisticas.cuellosBotellaDetectados.end(), posicion);
        if (it == estadisticas.cuellosBotellaDetectados.end()) {
            estadisticas.cuellosBotellaDetectados.push_back(posicion);
        }
    }
}

void EstadisticasSimulacion::registrarMovimiento(std::shared_ptr<AgenteBase> agente, 
                                                  QPoint /*desde*/, QPoint hasta) {
    int id = agente->getId();
    trayectoriasAgentes[id].push_back(hasta);
}

void EstadisticasSimulacion::registrarCambioPanico(int /*agenteId*/, bool entroPanico) {
    if (entroPanico) {
        estadisticas.personasConPanico++;
        estadisticas.eventosRegistrados["panico"]++;
    }
}

void EstadisticasSimulacion::actualizarTiempoSimulacion(double tiempo) {
    tiempoActual = tiempo;
    estadisticas.tiempoTotalSimulacion = tiempo;
}

void EstadisticasSimulacion::actualizarEstadoAgentes(const std::vector<std::shared_ptr<AgenteBase>>& agentes) {
    estadisticas.totalEnProceso = 0;
    estadisticas.personasConPanico = 0;
    estadisticas.personasMovilidadReducida = 0;
    
    for (const auto& agente : agentes) {
        // Contar agentes en proceso (no evacuados)
        if (agente->getEstado() != EstadoAgente::EVACUADO) {
            estadisticas.totalEnProceso++;
        }
        
        // Verificar atributos específicos de Persona
        Persona* persona = dynamic_cast<Persona*>(agente.get());
        if (persona != nullptr) {
            if (persona->estEnPanico()) {
                estadisticas.personasConPanico++;
            }
            if (persona->tieneMovilidadReducida()) {
                estadisticas.personasMovilidadReducida++;
            }
        }
    }
}

void EstadisticasSimulacion::calcularEstadisticas() {
    if (eventosEvacuacion.empty()) {
        return;
    }
    
    // Calcular tiempos de evacuación
    double sumaTimpos = 0.0;
    estadisticas.tiempoMinimoEvacuacion = eventosEvacuacion[0].tiempoEvacuacion;
    estadisticas.tiempoMaximoEvacuacion = eventosEvacuacion[0].tiempoEvacuacion;
    
    for (const auto& evento : eventosEvacuacion) {
        sumaTimpos += evento.tiempoEvacuacion;
        estadisticas.tiempoMinimoEvacuacion = std::min(estadisticas.tiempoMinimoEvacuacion, 
                                                       evento.tiempoEvacuacion);
        estadisticas.tiempoMaximoEvacuacion = std::max(estadisticas.tiempoMaximoEvacuacion, 
                                                       evento.tiempoEvacuacion);
    }
    
    estadisticas.tiempoPromedioEvacuacion = sumaTimpos / eventosEvacuacion.size();
    
    // Calcular tiempo promedio por salida
    std::map<std::string, double> sumaTiemposSalida;
    std::map<std::string, int> conteoSalida;
    
    for (const auto& evento : eventosEvacuacion) {
        std::string key = puntoToString(evento.salida);
        sumaTiemposSalida[key] += evento.tiempoEvacuacion;
        conteoSalida[key]++;
    }
    
    for (const auto& par : sumaTiemposSalida) {
        estadisticas.tiempoPromedioSalida[par.first] = par.second / conteoSalida[par.first];
    }
    
    // Calcular tasa de evacuación
    if (estadisticas.tiempoTotalSimulacion > 0) {
        estadisticas.tasaEvacuacion = estadisticas.totalEvacuados / 
                                      estadisticas.tiempoTotalSimulacion;
    }
    
    // Calcular distancia promedio
    if (estadisticas.totalEvacuados > 0) {
        estadisticas.distanciaPromedioRecorrida /= estadisticas.totalEvacuados;
    }
    
    // Calcular densidad promedio
    int areaTotalTransitable = filas * columnas;
    if (areaTotalTransitable > 0) {
        estadisticas.densidadPromedio = static_cast<double>(estadisticas.totalAgentes) / 
                                        areaTotalTransitable;
    }
}

EstadisticasGlobales EstadisticasSimulacion::getEstadisticas() const {
    return estadisticas;
}

std::string EstadisticasSimulacion::generarReporte() const {
    std::stringstream ss;
    
    ss << "═══════════════════════════════════════════════════════════════\n";
    ss << "        REPORTE DE ESTADÍSTICAS - SIMULACIÓN DE EVACUACIÓN     \n";
    ss << "═══════════════════════════════════════════════════════════════\n\n";
    
    // Información general
    ss << "━━━ INFORMACIÓN GENERAL ━━━\n";
    ss << "Total de agentes:            " << estadisticas.totalAgentes << "\n";
    ss << "Agentes evacuados:           " << estadisticas.totalEvacuados;
    if (estadisticas.totalAgentes > 0) {
        ss << " (" << (estadisticas.totalEvacuados * 100.0 / estadisticas.totalAgentes) << "%)";
    }
    ss << "\n";
    ss << "  - Personas:                " << estadisticas.personasEvacuadas << "\n";
    ss << "  - Rescatistas:             " << estadisticas.rescatistasEvacuados << "\n";
    ss << "Agentes en proceso:          " << estadisticas.totalEnProceso << "\n";
    
    // Advertencia si no hay evacuados
    if (estadisticas.totalEvacuados == 0 && estadisticas.totalAgentes > 0) {
        ss << "\n⚠️  ADVERTENCIA: Ningún agente pudo evacuar\n";
        ss << "    Posibles causas:\n";
        ss << "    - No hay salidas en el escenario\n";
        ss << "    - Los agentes están rodeados por paredes\n";
        ss << "    - Las rutas hacia las salidas están bloqueadas\n";
    }
    ss << "\n";
    
    // Métricas temporales (solo si hay evacuaciones)
    if (estadisticas.totalEvacuados > 0) {
        ss << "━━━ MÉTRICAS TEMPORALES ━━━\n";
        ss << "Tiempo total simulación:     " << formatearTiempo(estadisticas.tiempoTotalSimulacion) << "\n";
        ss << "Tiempo promedio evacuación:  " << formatearTiempo(estadisticas.tiempoPromedioEvacuacion) << "\n";
        ss << "Tiempo mínimo evacuación:    " << formatearTiempo(estadisticas.tiempoMinimoEvacuacion) << "\n";
        ss << "Tiempo máximo evacuación:    " << formatearTiempo(estadisticas.tiempoMaximoEvacuacion) << "\n";
        ss << "Tasa de evacuación:          " << std::fixed << std::setprecision(2) 
           << estadisticas.tasaEvacuacion << " personas/segundo\n\n";
    } else {
        ss << "━━━ MÉTRICAS TEMPORALES ━━━\n";
        ss << "Tiempo total simulación:     " << formatearTiempo(estadisticas.tiempoTotalSimulacion) << "\n";
        ss << "(No se registraron evacuaciones completadas)\n\n";
    }
    
    // Distribución por salida
    if (!estadisticas.personasPorSalida.empty()) {
        ss << "━━━ DISTRIBUCIÓN POR SALIDA ━━━\n";
        for (const auto& par : estadisticas.personasPorSalida) {
            ss << "Salida " << par.first << ": " << par.second << " personas";
            if (estadisticas.tiempoPromedioSalida.find(par.first) != 
                estadisticas.tiempoPromedioSalida.end()) {
                ss << " (tiempo promedio: " 
                   << formatearTiempo(estadisticas.tiempoPromedioSalida.at(par.first)) << ")";
            }
            ss << "\n";
        }
        ss << "\n";
    } else {
        ss << "━━━ DISTRIBUCIÓN POR SALIDA ━━━\n";
        ss << "(No se registraron evacuaciones por ninguna salida)\n\n";
    }
    
    // Comportamiento de agentes
    ss << "━━━ COMPORTAMIENTO DE AGENTES ━━━\n";
    ss << "Personas con pánico:         " << estadisticas.personasConPanico << "\n";
    ss << "Personas movilidad reducida: " << estadisticas.personasMovilidadReducida << "\n";
    ss << "Personas rescatadas:         " << estadisticas.personasRescatadas << "\n\n";
    
    // Métricas de eficiencia
    ss << "━━━ MÉTRICAS DE EFICIENCIA ━━━\n";
    if (estadisticas.totalEvacuados > 0) {
        ss << "Distancia promedio recorrida:" << std::fixed << std::setprecision(2) 
           << estadisticas.distanciaPromedioRecorrida << " celdas\n";
    } else {
        ss << "Distancia promedio recorrida: N/A (sin evacuaciones)\n";
    }
    ss << "Colisiones totales:          " << estadisticas.colisionesTotales << "\n";
    ss << "Densidad promedio:           " << std::fixed << std::setprecision(4) 
       << estadisticas.densidadPromedio << " agentes/celda\n";
    ss << "Cuellos de botella:          " << estadisticas.cuellosBotellaDetectados.size() << " detectados\n";
    
    if (!estadisticas.cuellosBotellaDetectados.empty()) {
        ss << "  Ubicaciones: ";
        for (size_t i = 0; i < estadisticas.cuellosBotellaDetectados.size(); i++) {
            if (i > 0) ss << ", ";
            ss << "(" << estadisticas.cuellosBotellaDetectados[i].x() 
               << "," << estadisticas.cuellosBotellaDetectados[i].y() << ")";
        }
        ss << "\n";
    }
    ss << "\n";
    
    // Diagnóstico adicional si hay problemas
    if (estadisticas.totalEvacuados == 0 && estadisticas.totalAgentes > 0) {
        ss << "━━━ DIAGNÓSTICO ━━━\n";
        ss << "💡 SUGERENCIAS:\n";
        ss << "   1. Verifica que haya al menos una salida en el escenario\n";
        ss << "   2. Asegúrate de que los agentes no estén rodeados por paredes\n";
        ss << "   3. Comprueba que exista un camino válido hacia las salidas\n";
        ss << "   4. Revisa el algoritmo de pathfinding (PathFinder)\n\n";
    }
    
    ss << "═══════════════════════════════════════════════════════════════\n";
    ss << "              FIN DEL REPORTE - " << __DATE__ << "\n";
    ss << "═══════════════════════════════════════════════════════════════\n";
    
    return ss.str();
}

std::string EstadisticasSimulacion::generarReporteCSV() const {
    std::stringstream ss;
    
    // Encabezados
    ss << "Métrica,Valor\n";
    ss << "Total Agentes," << estadisticas.totalAgentes << "\n";
    ss << "Total Evacuados," << estadisticas.totalEvacuados << "\n";
    ss << "Personas Evacuadas," << estadisticas.personasEvacuadas << "\n";
    ss << "Rescatistas Evacuados," << estadisticas.rescatistasEvacuados << "\n";
    ss << "Tiempo Total (s)," << estadisticas.tiempoTotalSimulacion << "\n";
    ss << "Tiempo Promedio Evacuación (s)," << estadisticas.tiempoPromedioEvacuacion << "\n";
    ss << "Tiempo Mínimo (s)," << estadisticas.tiempoMinimoEvacuacion << "\n";
    ss << "Tiempo Máximo (s)," << estadisticas.tiempoMaximoEvacuacion << "\n";
    ss << "Tasa Evacuación (p/s)," << estadisticas.tasaEvacuacion << "\n";
    ss << "Personas con Pánico," << estadisticas.personasConPanico << "\n";
    ss << "Movilidad Reducida," << estadisticas.personasMovilidadReducida << "\n";
    ss << "Colisiones Totales," << estadisticas.colisionesTotales << "\n";
    ss << "Distancia Promedio," << estadisticas.distanciaPromedioRecorrida << "\n";
    ss << "Densidad Promedio," << estadisticas.densidadPromedio << "\n";
    ss << "Cuellos de Botella," << estadisticas.cuellosBotellaDetectados.size() << "\n";
    
    ss << "\nSalida,Personas,Tiempo Promedio (s)\n";
    for (const auto& par : estadisticas.personasPorSalida) {
        ss << par.first << "," << par.second << ",";
        if (estadisticas.tiempoPromedioSalida.find(par.first) != 
            estadisticas.tiempoPromedioSalida.end()) {
            ss << estadisticas.tiempoPromedioSalida.at(par.first);
        }
        ss << "\n";
    }
    
    return ss.str();
}

std::string EstadisticasSimulacion::generarReporteJSON() const {
    std::stringstream ss;
    
    ss << "{\n";
    ss << "  \"simulacion\": {\n";
    ss << "    \"totalAgentes\": " << estadisticas.totalAgentes << ",\n";
    ss << "    \"totalEvacuados\": " << estadisticas.totalEvacuados << ",\n";
    ss << "    \"personasEvacuadas\": " << estadisticas.personasEvacuadas << ",\n";
    ss << "    \"rescatistasEvacuados\": " << estadisticas.rescatistasEvacuados << ",\n";
    ss << "    \"enProceso\": " << estadisticas.totalEnProceso << "\n";
    ss << "  },\n";
    ss << "  \"tiempos\": {\n";
    ss << "    \"total\": " << estadisticas.tiempoTotalSimulacion << ",\n";
    ss << "    \"promedioEvacuacion\": " << estadisticas.tiempoPromedioEvacuacion << ",\n";
    ss << "    \"minimo\": " << estadisticas.tiempoMinimoEvacuacion << ",\n";
    ss << "    \"maximo\": " << estadisticas.tiempoMaximoEvacuacion << ",\n";
    ss << "    \"tasaEvacuacion\": " << estadisticas.tasaEvacuacion << "\n";
    ss << "  },\n";
    ss << "  \"comportamiento\": {\n";
    ss << "    \"personasConPanico\": " << estadisticas.personasConPanico << ",\n";
    ss << "    \"movilidadReducida\": " << estadisticas.personasMovilidadReducida << ",\n";
    ss << "    \"personasRescatadas\": " << estadisticas.personasRescatadas << "\n";
    ss << "  },\n";
    ss << "  \"eficiencia\": {\n";
    ss << "    \"distanciaPromedio\": " << estadisticas.distanciaPromedioRecorrida << ",\n";
    ss << "    \"colisionesTotales\": " << estadisticas.colisionesTotales << ",\n";
    ss << "    \"densidadPromedio\": " << estadisticas.densidadPromedio << ",\n";
    ss << "    \"cuellosBottela\": " << estadisticas.cuellosBotellaDetectados.size() << "\n";
    ss << "  },\n";
    ss << "  \"salidas\": [\n";
    
    bool primero = true;
    for (const auto& par : estadisticas.personasPorSalida) {
        if (!primero) ss << ",\n";
        ss << "    {\n";
        ss << "      \"ubicacion\": \"" << par.first << "\",\n";
        ss << "      \"personas\": " << par.second << ",\n";
        ss << "      \"tiempoPromedio\": ";
        if (estadisticas.tiempoPromedioSalida.find(par.first) != 
            estadisticas.tiempoPromedioSalida.end()) {
            ss << estadisticas.tiempoPromedioSalida.at(par.first);
        } else {
            ss << "0";
        }
        ss << "\n    }";
        primero = false;
    }
    
    ss << "\n  ]\n";
    ss << "}\n";
    
    return ss.str();
}

bool EstadisticasSimulacion::exportarReporte(const std::string& rutaArchivo) const {
    std::ofstream archivo(rutaArchivo);
    if (!archivo.is_open()) {
        return false;
    }
    
    // Determinar el formato por la extensión
    if (rutaArchivo.find(".csv") != std::string::npos) {
        archivo << generarReporteCSV();
    } else if (rutaArchivo.find(".json") != std::string::npos) {
        archivo << generarReporteJSON();
    } else {
        archivo << generarReporte();
    }
    
    archivo.close();
    return true;
}

int EstadisticasSimulacion::getPersonasEvacuadasPorSalida(QPoint salida) const {
    std::string key = puntoToString(salida);
    auto it = estadisticas.personasPorSalida.find(key);
    return (it != estadisticas.personasPorSalida.end()) ? it->second : 0;
}

double EstadisticasSimulacion::getTiempoPromedioSalida(QPoint salida) const {
    std::string key = puntoToString(salida);
    auto it = estadisticas.tiempoPromedioSalida.find(key);
    return (it != estadisticas.tiempoPromedioSalida.end()) ? it->second : 0.0;
}

std::vector<EventoEvacuacion> EstadisticasSimulacion::getEventosEvacuacion() const {
    return eventosEvacuacion;
}

std::vector<QPoint> EstadisticasSimulacion::getCuellosBotellaDetectados() const {
    return estadisticas.cuellosBotellaDetectados;
}

std::map<std::string, double> EstadisticasSimulacion::getMetricasResumen() const {
    std::map<std::string, double> metricas;
    metricas["total_agentes"] = estadisticas.totalAgentes;
    metricas["evacuados"] = estadisticas.totalEvacuados;
    metricas["tiempo_total"] = estadisticas.tiempoTotalSimulacion;
    metricas["tiempo_promedio"] = estadisticas.tiempoPromedioEvacuacion;
    metricas["tasa_evacuacion"] = estadisticas.tasaEvacuacion;
    metricas["colisiones"] = estadisticas.colisionesTotales;
    metricas["cuellos_botella"] = estadisticas.cuellosBotellaDetectados.size();
    return metricas;
}

std::string EstadisticasSimulacion::getResumenRapido() const {
    std::stringstream ss;
    ss << "Evacuados: " << estadisticas.totalEvacuados << "/" << estadisticas.totalAgentes;
    ss << " | Tiempo: " << formatearTiempo(estadisticas.tiempoTotalSimulacion);
    
    if (estadisticas.totalEvacuados > 0) {
        ss << " | Tasa: " << std::fixed << std::setprecision(1) << estadisticas.tasaEvacuacion << " p/s";
    } else {
        ss << " | Sin evacuaciones";
    }
    
    ss << " | Colisiones: " << estadisticas.colisionesTotales;
    
    if (estadisticas.totalEnProceso > 0) {
        ss << " | En proceso: " << estadisticas.totalEnProceso;
    }
    
    return ss.str();
}

// Métodos auxiliares privados
std::string EstadisticasSimulacion::puntoToString(QPoint punto) const {
    return std::to_string(punto.x()) + "," + std::to_string(punto.y());
}

double EstadisticasSimulacion::calcularDistancia(QPoint p1, QPoint p2) const {
    int dx = p2.x() - p1.x();
    int dy = p2.y() - p1.y();
    return std::sqrt(dx*dx + dy*dy);
}

std::string EstadisticasSimulacion::formatearTiempo(double segundos) const {
    int mins = static_cast<int>(segundos) / 60;
    double secs = segundos - (mins * 60);
    
    std::stringstream ss;
    if (mins > 0) {
        ss << mins << "m " << std::fixed << std::setprecision(1) << secs << "s";
    } else {
        ss << std::fixed << std::setprecision(2) << secs << "s";
    }
    return ss.str();
}
