#include "../include/ObservadorEvento.h"
#include "../include/AgenteBase.h"
#include <iostream>
#include <sstream>

/**
 * Constructor de GestorEventos
 */
GestorEventos::GestorEventos()
    : totalEvacuados(0),
      totalBloqueados(0),
      totalEnPanico(0) {
}

/**
 * Implementación del método de notificación
 */
void GestorEventos::onEventoAgente(AgenteBase* agente, const std::string& tipoEvento) {
    if (!agente) return;
    
    // Procesar el evento y actualizar contadores
    if (tipoEvento == "EVACUADO") {
        totalEvacuados++;
        std::cout << "[EVENTO] Agente " << agente->getId() 
                  << " ha sido evacuado exitosamente." << std::endl;
    }
    else if (tipoEvento == "BLOQUEADO") {
        totalBloqueados++;
        std::cout << "[EVENTO] Agente " << agente->getId() 
                  << " está bloqueado." << std::endl;
    }
    else if (tipoEvento == "PANICO") {
        totalEnPanico++;
        std::cout << "[EVENTO] Agente " << agente->getId() 
                  << " entró en pánico." << std::endl;
    }
    else if (tipoEvento == "ASISTIENDO_PERSONA") {
        std::cout << "[EVENTO] Rescatista " << agente->getId() 
                  << " está asistiendo a una persona." << std::endl;
    }
    else if (tipoEvento == "OBSTACULO_ENCONTRADO") {
        std::cout << "[EVENTO] Agente " << agente->getId() 
                  << " encontró un obstáculo." << std::endl;
    }
}

/**
 * Reinicia los contadores de eventos
 */
void GestorEventos::reiniciarContadores() {
    totalEvacuados = 0;
    totalBloqueados = 0;
    totalEnPanico = 0;
}

/**
 * Genera un reporte de eventos
 */
std::string GestorEventos::generarReporte() const {
    std::ostringstream reporte;
    reporte << "========== REPORTE DE EVENTOS ==========\n";
    reporte << "Total de agentes evacuados: " << totalEvacuados << "\n";
    reporte << "Total de agentes bloqueados: " << totalBloqueados << "\n";
    reporte << "Total de agentes en pánico: " << totalEnPanico << "\n";
    reporte << "========================================\n";
    return reporte.str();
}
