#ifndef ESTADISTICASSIMULACION_H
#define ESTADISTICASSIMULACION_H

#include <map>
#include <vector>
#include <string>
#include <memory>
#include <QPoint>  // Qt Core
#include "AgenteBase.h"

/**
 * @brief Estructura que almacena información sobre un evento de evacuación
 */
struct EventoEvacuacion {
    int agenteId;
    std::string tipoAgente;  // "Persona" o "Rescatista"
    QPoint salida;           // Posición de la salida utilizada
    double tiempoEvacuacion; // Tiempo en segundos que tardó en evacuar
    int pasosRealizados;     // Número de pasos/movimientos
};

/**
 * @brief Estructura que almacena las estadísticas globales de la simulación
 */
struct EstadisticasGlobales {
    // Métricas temporales
    double tiempoTotalSimulacion;
    double tiempoPromedioEvacuacion;
    double tiempoMinimoEvacuacion;
    double tiempoMaximoEvacuacion;
    
    // Contadores generales
    int totalAgentes;
    int totalEvacuados;
    int totalEnProceso;
    int personasEvacuadas;
    int rescatistasEvacuados;
    
    // Métricas por salida
    std::map<std::string, int> personasPorSalida;  // "x,y" -> cantidad
    std::map<std::string, double> tiempoPromedioSalida;
    
    // Métricas de comportamiento
    int personasConPanico;
    int personasMovilidadReducida;
    int personasRescatadas;
    
    // Métricas de eficiencia
    double tasaEvacuacion;  // Personas evacuadas por segundo
    double densidadPromedio; // Densidad de agentes en el escenario
    std::vector<QPoint> cuellosBotellaDetectados;
    
    // Estadísticas avanzadas
    std::map<std::string, int> eventosRegistrados; // tipo_evento -> cantidad
    double distanciaPromedioRecorrida;
    int colisionesTotales;
};

/**
 * @brief Clase para recopilar, calcular y exportar estadísticas de la simulación
 * 
 * Esta clase implementa el patrón Singleton y Observer para recopilar datos
 * en tiempo real durante la simulación y generar reportes al finalizar.
 */
class EstadisticasSimulacion {
public:
    // Constructor
    EstadisticasSimulacion();
    ~EstadisticasSimulacion();
    
    // Inicialización
    void iniciar(int totalAgentes, int filas, int columnas);
    void reiniciar();
    
    // Registro de eventos
    void registrarEvacuacion(std::shared_ptr<AgenteBase> agente, QPoint salida, double tiempo, int pasos);
    void registrarColision(QPoint posicion);
    void registrarCuelloBotella(QPoint posicion);
    void registrarMovimiento(std::shared_ptr<AgenteBase> agente, QPoint desde, QPoint hasta);
    void registrarCambioPanico(int agenteId, bool entroPanico);
    
    // Actualización continua
    void actualizarTiempoSimulacion(double tiempo);
    void actualizarEstadoAgentes(const std::vector<std::shared_ptr<AgenteBase>>& agentes);
    
    // Cálculo de métricas
    void calcularEstadisticas();
    EstadisticasGlobales getEstadisticas() const;
    
    // Exportación de datos
    std::string generarReporte() const;
    std::string generarReporteCSV() const;
    std::string generarReporteJSON() const;
    bool exportarReporte(const std::string& rutaArchivo) const;
    
    // Consultas específicas
    int getPersonasEvacuadasPorSalida(QPoint salida) const;
    double getTiempoPromedioSalida(QPoint salida) const;
    std::vector<EventoEvacuacion> getEventosEvacuacion() const;
    std::vector<QPoint> getCuellosBotellaDetectados() const;
    
    // Visualización de datos
    std::map<std::string, double> getMetricasResumen() const;
    std::string getResumenRapido() const;

private:
    // Datos internos
    EstadisticasGlobales estadisticas;
    std::vector<EventoEvacuacion> eventosEvacuacion;
    std::map<int, std::vector<QPoint>> trayectoriasAgentes; // agenteId -> ruta
    std::map<std::string, int> contadorCuellosBottela; // posición -> frecuencia
    
    // Métricas en tiempo real
    double tiempoActual;
    int filas, columnas;
    bool simulacionIniciada;
    
    // Métodos auxiliares
    std::string puntoToString(QPoint punto) const;
    double calcularDistancia(QPoint p1, QPoint p2) const;
    void detectarCuellosBottela();
    std::string formatearTiempo(double segundos) const;
};

#endif // ESTADISTICASSIMULACION_H
