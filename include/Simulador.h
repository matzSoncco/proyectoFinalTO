#ifndef SIMULADOR_H
#define SIMULADOR_H

#include <QObject>
#include <QTimer>
#include <QElapsedTimer>
#include <vector>
#include "Escenario.h"
#include "AgenteBase.h"
#include "PathFinder.h"
#include "EstadisticasSimulacion.h"
#include <memory>

class Simulador : public QObject {
    Q_OBJECT

public:
    explicit Simulador(QObject *parent = nullptr);
    ~Simulador();

    // Configuración
    void cargarEscenario(int filas, int cols);
    void agregarAgente(std::shared_ptr<AgenteBase> agente);

    // Control
    void iniciar();
    void pausar();
    void reiniciar();
    
    // Acceso para la GUI
    Escenario* getEscenario();
    const std::vector<std::shared_ptr<AgenteBase>>& getAgentes() const;
    
    // Estadísticas
    EstadisticasSimulacion* getEstadisticas();
    void exportarEstadisticas(const std::string& rutaArchivo);
    void mostrarEstadisticas();

signals:
    void mundoActualizado();
    void simulacionTerminada();
    void estadisticasActualizadas(QString resumen);

private slots:
    void loopPrincipal();

private:
    Escenario* escenario;
    std::vector<std::shared_ptr<AgenteBase>> agentes;
    QTimer* timer;
    QElapsedTimer tiempoTranscurrido;
    bool esActivo;
    
    // Sistema de estadísticas
    EstadisticasSimulacion* estadisticas;
    std::map<int, int> pasosPorAgente;  // agenteId -> cantidad de pasos
    std::map<int, QPoint> posicionAnterior;  // Para detectar movimientos
    double tiempoSimulacion;
    
    // Detección de estancamiento
    int ticksSinMovimiento;
    int maxTicksSinMovimiento;
    
    void detectarEstancamiento();
};

#endif
