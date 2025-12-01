#ifndef SIMULADOR_H
#define SIMULADOR_H

#include <QObject>
#include <QTimer>
#include <vector>
#include "Escenario.h"
#include "AgenteBase.h"
#include "PathFinder.h"

class Simulador : public QObject {
    Q_OBJECT //macro obligatoria para que funcionen las señales

public:
    explicit Simulador(QObject *parent = nullptr);
    ~Simulador();

    //configuración
    void cargarEscenario(int filas, int cols);
    void agregarAgente(AgenteBase* agente);

    //control
    void iniciar();
    void pausar();
    
    //acceso para la GUI
    Escenario* getEscenario();
    const std::vector<AgenteBase*>& getAgentes() const;

signals:
    //el GUI escuchará esto para saber cuándo repintar la pantalla
    void mundoActualizado();
    void simulacionTerminada();

private slots:
    void loopPrincipal();

private:
    Escenario* escenario;
    std::vector<AgenteBase*> agentes;
    QTimer* timer;
    bool esActivo;
};

#endif
