#include <QCoreApplication>
#include <QTimer>
#include <QDebug>
#include <iostream>
#include <vector>
#include <memory>

//el motor
#include "../include/Simulador.h"
#include "../include/Escenario.h"
#include "../include/PathFinder.h"

//los actores
#include "../include/AgenteBase.h"
#include "../include/Persona.h"
#include "../include/Rescatista.h"
#include "../include/ObservadorEvento.h"
#include "../include/FactoriaAgentes.h"

int main(int argc, char *argv[])
{
    // Necesario para que funcione el QTimer de tu Simulador
    QCoreApplication a(argc, argv);

    std::cout << "========================================\n";
    std::cout << "  SIMULADOR INTEGRADO: MOTOR + AGENTES  \n";
    std::cout << "========================================\n\n";

    // ---------------------------------------------------------
    // 1. CONFIGURACIÓN DEL ENTORNO (TU PARTE - LÍDER TÉCNICO)
    // ---------------------------------------------------------
    std::cout << "[MOTOR] Inicializando Simulador y Escenario...\n";
    
    Simulador* miSimulador = new Simulador(&a);
    
    // Creamos un mapa de 20x20 para la prueba
    miSimulador->cargarEscenario(20, 20);
    
    // Ponemos una SALIDA en la esquina (18, 18)
    // Asumiendo que 2 es el código de salida en tu Escenario.cpp
    miSimulador->getEscenario()->setCelda(18, 18, 2); 

    // Ponemos un MURO en el medio para probar que tu BFS lo esquiva
    miSimulador->getEscenario()->setCelda(5, 5, 1);
    miSimulador->getEscenario()->setCelda(5, 6, 1);
    miSimulador->getEscenario()->setCelda(5, 7, 1);

    // ---------------------------------------------------------
    // 2. CREACIÓN DE AGENTES (PARTE DE TU COMPAÑERO)
    // ---------------------------------------------------------
    std::cout << "[FACTORY] Creando Agentes usando Patrones...\n";
    
    auto gestorEventos = std::make_shared<GestorEventos>();
    FactoriaAgentes factoria;
    
    // Creamos una persona en (0,0)
    // NOTA: Asegúrate que Posicion(x,y) sea compatible o conviértelo a QPoint
    auto persona1 = factoria.crearPersona(Posicion(0, 0), 25, false);
    persona1->agregarObservador(gestorEventos);
    
    // Creamos un rescatista en (0, 5)
    auto rescatista1 = factoria.crearRescatista(Posicion(0, 5));
    rescatista1->agregarObservador(gestorEventos);

    // ---------------------------------------------------------
    // 3. LA INTEGRACIÓN (AQUÍ SE UNEN LOS DOS MUNDOS)
    // ---------------------------------------------------------
    std::cout << "[INTEGRACION] Insertando agentes en el Simulador...\n";

    // En lugar de darles una ruta fija, los metemos al simulador
    // El simulador calculará la ruta paso a paso con BFS
    miSimulador->agregarAgente(persona1.get());    // .get() porque tu vector usa punteros raw, o ajusta tu Simulador
    miSimulador->agregarAgente(rescatista1.get());

    // ---------------------------------------------------------
    // 4. VISUALIZACIÓN DE CONSOLA (PARA VER QUE FUNCIONA)
    // ---------------------------------------------------------
    // Conectamos la señal de tu simulador para imprimir el estado
    QObject::connect(miSimulador, &Simulador::mundoActualizado, [&]() {
        // Esto se ejecuta cada vez que el Timer hace tic (cada 500ms)
        
        QPoint posP = persona1->getPosicion(); // Asumiendo que devuelve QPoint o tiene .x .y
        QPoint posR = rescatista1->getPosicion();

        std::cout << ">> TIC: Persona(" << posP.x() << "," << posP.y() << ") "
                  << "| Rescatista(" << posR.x() << "," << posR.y() << ")" << std::endl;

        // Condición de victoria simple para cerrar la prueba
        if (miSimulador->getEscenario()->esSalida(posP.x(), posP.y())) {
            std::cout << "\n[EXITO] ¡La persona llego a la salida usando BFS!\n";
            QCoreApplication::quit(); // Cierra el programa
        }
    });

    // ---------------------------------------------------------
    // 5. INICIAR MOTOR
    // ---------------------------------------------------------
    std::cout << "[MOTOR] Iniciando Loop Principal (QTimer)...\n";
    miSimulador->iniciar();

    return a.exec(); // Arranca el bucle de eventos de Qt
}
