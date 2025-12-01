#include <QApplication>
#include "../include/VentanaPrincipal.h"
#include <QTimer>
#include <QDebug>
#include <iostream>
#include <vector>
#include <memory>

// Motor
#include "../include/Simulador.h"
#include "../include/Escenario.h"
#include "../include/PathFinder.h"

// Actores
#include "../include/AgenteBase.h"
#include "../include/Persona.h"
#include "../include/Rescatista.h"
#include "../include/ObservadorEvento.h"
#include "../include/FactoriaAgentes.h"

int main(int argc, char *argv[])
{
    // ESTE ES EL ÚNICO LOOP QT PERMITIDO QUE TRABAJAMOS
    QApplication app(argc, argv);

    /*
    std::cout << "========================================\n";
    std::cout << "  SIMULADOR INTEGRADO: MOTOR + AGENTES  \n";
    std::cout << "========================================\n\n";

    // ---------------------------------------------------------
    // 1. CONFIGURACIÓN DEL MOTOR
    // ---------------------------------------------------------
    std::cout << "[MOTOR] Inicializando Simulador y Escenario...\n";
    /*
    Simulador* miSimulador = new Simulador(&app);

    // Mapa 20x20
    miSimulador->cargarEscenario(20, 20);

    // Salida
    miSimulador->getEscenario()->setCelda(18, 18, 2);

    // Muros
    miSimulador->getEscenario()->setCelda(5, 5, 1);
    miSimulador->getEscenario()->setCelda(5, 6, 1);
    miSimulador->getEscenario()->setCelda(5, 7, 1);

    // ---------------------------------------------------------
    // 2. CREACIÓN DE AGENTES
    // ---------------------------------------------------------
    std::cout << "[FACTORY] Creando Agentes...\n";

    auto gestorEventos = std::make_shared<GestorEventos>();
    FactoriaAgentes factoria;

    auto persona1 = factoria.crearPersona(Posicion(0, 0), 25, false);
    persona1->agregarObservador(gestorEventos);

    auto rescatista1 = factoria.crearRescatista(Posicion(0, 5));
    rescatista1->agregarObservador(gestorEventos);

    // Insertarlos en el simulador
    miSimulador->agregarAgente(persona1.get());
    miSimulador->agregarAgente(rescatista1.get());

    // ---------------------------------------------------------
    // 3. IMPRIMIR ESTADO EN CONSOLA (podría ser opcional)
    // ---------------------------------------------------------
    QObject::connect(miSimulador, &Simulador::mundoActualizado, [&]() {
        // Esto se ejecuta cada vez que el Timer hace tic (cada 500ms)
        
        QPoint posP = persona1->getPosicion(); // Asumiendo que devuelve QPoint o tiene .x .y
        QPoint posR = rescatista1->getPosicion();

        std::cout << ">> TIC: Persona(" << posP.x() << "," << posP.y() << ") "
                  << "| Rescatista(" << posR.x() << "," << posR.y() << ")" << std::endl;

        // Condición de victoria simple para cerrar la prueba
        if (miSimulador->getEscenario()->esSalida(posP.x(), posP.y())) {
            std::cout << "\n[EXITO] ¡La persona llegó a la salida usando BFS!\n";
            QApplication::quit();
        }
    });

    // Iniciar motor
    std::cout << "[MOTOR] Iniciando QTimer...\n";
    miSimulador->iniciar();
    */
    // ---------------------------------------------------------
    // 4. INICIAR EL GUI
    // ---------------------------------------------------------
    VentanaPrincipal ventana;
    ventana.show();

    // EL ÚNICO RETURN (GUI + MOTOR)
    return app.exec();
}
