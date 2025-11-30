#ifndef OBSERVADOREVENTO_H
#define OBSERVADOREVENTO_H

#include <string>
#include <memory>

// Forward declaration
class AgenteBase;

/**
 * @brief Interfaz para el Patrón Observer
 * 
 * Los objetos que implementen esta interfaz podrán recibir
 * notificaciones de eventos de los agentes.
 */
class ObservadorEvento {
public:
    virtual ~ObservadorEvento() = default;
    
    /**
     * @brief Método llamado cuando ocurre un evento
     * @param agente Puntero al agente que generó el evento
     * @param tipoEvento Descripción del evento
     */
    virtual void onEventoAgente(AgenteBase* agente, const std::string& tipoEvento) = 0;
};

/**
 * @brief Clase concreta que implementa ObservadorEvento
 * 
 * Esta clase puede ser usada por el Simulador o la GUI para
 * recibir notificaciones de eventos importantes.
 */
class GestorEventos : public ObservadorEvento {
private:
    int totalEvacuados;
    int totalBloqueados;
    int totalEnPanico;
    
public:
    GestorEventos();
    
    /**
     * @brief Implementación del método de notificación
     */
    void onEventoAgente(AgenteBase* agente, const std::string& tipoEvento) override;
    
    /**
     * @brief Reinicia los contadores de eventos
     */
    void reiniciarContadores();
    
    /**
     * @brief Genera un reporte de eventos
     */
    std::string generarReporte() const;
    
    // Getters
    int getTotalEvacuados() const { return totalEvacuados; }
    int getTotalBloqueados() const { return totalBloqueados; }
    int getTotalEnPanico() const { return totalEnPanico; }
};

#endif // OBSERVADOREVENTO_H
