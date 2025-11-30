#include "include/AgenteBase.h"
#include "include/Persona.h"
#include "include/Rescatista.h"
#include "include/ObservadorEvento.h"
#include "include/FactoriaAgentes.h"
#include <iostream>
#include <memory>
#include <vector>

/**
 * Programa de demostración del sistema de agentes
 * 
 * Este ejemplo muestra:
 * 1. Herencia y Polimorfismo (AgenteBase -> Persona/Rescatista)
 * 2. Patrón Observer (GestorEventos observa eventos de agentes)
 * 3. Patrón Prototype (clonación de agentes)
 * 4. Patrón Factory (creación centralizada de agentes)
 */
int main() {
    std::cout << "========================================\n";
    std::cout << "  SIMULADOR DE EVACUACIÓN - DEMO POO\n";
    std::cout << "========================================\n\n";
    
    // ===== 1. CREAR GESTOR DE EVENTOS (PATRÓN OBSERVER) =====
    std::cout << "1. Creando Gestor de Eventos (Patrón Observer)...\n";
    auto gestorEventos = std::make_shared<GestorEventos>();
    
    // ===== 2. CREAR FACTORÍA DE AGENTES (PATRÓN FACTORY) =====
    std::cout << "2. Inicializando Factoría de Agentes (Patrón Factory)...\n";
    FactoriaAgentes factoria;
    
    // ===== 3. CREAR AGENTES CON POLIMORFISMO =====
    std::cout << "3. Creando diferentes tipos de agentes (Polimorfismo)...\n";
    
    // Vector de punteros a la clase base (polimorfismo)
    std::vector<std::shared_ptr<AgenteBase>> agentes;
    
    // Crear personas normales
    auto persona1 = factoria.crearPersona(Posicion(10, 10), 25, false);
    auto persona2 = factoria.crearPersona(Posicion(20, 20), 65, false);
    auto persona3 = factoria.crearPersona(Posicion(30, 30), 45, true);  // Movilidad reducida
    
    // Crear rescatistas
    auto rescatista1 = factoria.crearRescatista(Posicion(5, 5));
    auto rescatista2 = factoria.crearRescatista(Posicion(15, 15));
    
    // Agregar observador a todos los agentes
    persona1->agregarObservador(gestorEventos);
    persona2->agregarObservador(gestorEventos);
    persona3->agregarObservador(gestorEventos);
    rescatista1->agregarObservador(gestorEventos);
    rescatista2->agregarObservador(gestorEventos);
    
    // Agregar al vector polimórfico
    agentes.push_back(persona1);
    agentes.push_back(persona2);
    agentes.push_back(persona3);
    agentes.push_back(rescatista1);
    agentes.push_back(rescatista2);
    
    std::cout << "   - Creadas 3 personas y 2 rescatistas\n";
    
    // ===== 4. DEMOSTRAR PATRÓN PROTOTYPE (CLONACIÓN) =====
    std::cout << "\n4. Demostrando Patrón Prototype (Clonación)...\n";
    
    // Clonar desde prototipo
    auto personaClonada = factoria.clonarDesdePrototipo("PersonaNormal", Posicion(40, 40));
    if (personaClonada) {
        personaClonada->agregarObservador(gestorEventos);
        agentes.push_back(personaClonada);
        std::cout << "   - Persona clonada desde prototipo en posición (40, 40)\n";
    }
    
    // Clonar directamente un agente existente
    auto personaCopia = persona1->clonar();
    personaCopia->setPosicion(Posicion(50, 50));
    personaCopia->agregarObservador(gestorEventos);
    agentes.push_back(personaCopia);
    std::cout << "   - Persona copiada directamente en posición (50, 50)\n";
    
    // ===== 5. CREAR GRUPO DE PERSONAS =====
    std::cout << "\n5. Creando grupo de personas dispersas (Factory Method)...\n";
    auto grupo = factoria.crearGrupoPersonas(5, Posicion(100, 100), 35, 20.0);
    std::cout << "   - Creadas " << grupo.size() << " personas en grupo\n";
    
    for (auto& persona : grupo) {
        persona->agregarObservador(gestorEventos);
        agentes.push_back(persona);
    }
    
    // ===== 6. ESTABLECER RUTAS DE EVACUACIÓN =====
    std::cout << "\n6. Estableciendo rutas de evacuación...\n";
    
    // Ruta simple hacia una salida
    std::vector<Posicion> ruta = {
        Posicion(50, 50),
        Posicion(100, 100),
        Posicion(150, 150),
        Posicion(200, 200)  // Salida
    };
    
    for (auto& agente : agentes) {
        agente->establecerRuta(ruta);
    }
    
    std::cout << "   - Rutas asignadas a todos los agentes\n";
    
    // ===== 7. SIMULAR EVACUACIÓN =====
    std::cout << "\n7. Iniciando simulación de evacuación...\n";
    std::cout << "   (Simulando 10 iteraciones)\n\n";
    
    const double deltaTime = 0.5;  // Medio segundo por iteración
    
    for (int i = 0; i < 10; ++i) {
        std::cout << "--- Iteración " << (i + 1) << " ---\n";
        
        // Actualizar todos los agentes (polimorfismo en acción)
        for (auto& agente : agentes) {
            agente->actualizar(deltaTime);
            
            // Los rescatistas buscan personas que necesitan ayuda
            if (auto rescatista = std::dynamic_pointer_cast<Rescatista>(agente)) {
                rescatista->buscarPersonasQueNecesitanAyuda(agentes);
            }
        }
        
        // Simular pánico en algunas personas
        if (i == 3) {
            std::cout << "\n[SIMULACIÓN] ¡Evento de pánico!\n";
            if (auto persona = std::dynamic_pointer_cast<Persona>(agentes[0])) {
                persona->incrementarPanico(0.8);
            }
        }
        
        // Mostrar estado de algunos agentes
        std::cout << "Estado agente 1: ";
        switch (agentes[0]->getEstado()) {
            case EstadoAgente::NORMAL: std::cout << "NORMAL"; break;
            case EstadoAgente::EVACUANDO: std::cout << "EVACUANDO"; break;
            case EstadoAgente::PANICO: std::cout << "PÁNICO"; break;
            case EstadoAgente::EVACUADO: std::cout << "EVACUADO"; break;
            case EstadoAgente::BLOQUEADO: std::cout << "BLOQUEADO"; break;
        }
        std::cout << " - Posición: (" << agentes[0]->getPosicion().x 
                  << ", " << agentes[0]->getPosicion().y << ")\n";
        
        std::cout << "\n";
    }
    
    // ===== 8. MOSTRAR ESTADÍSTICAS FINALES =====
    std::cout << "\n8. Reporte final de la simulación:\n";
    std::cout << gestorEventos->generarReporte();
    
    // ===== 9. DEMOSTRAR DIFERENTES COMPORTAMIENTOS =====
    std::cout << "\n9. Análisis de comportamientos (Polimorfismo):\n";
    
    for (size_t i = 0; i < std::min(size_t(3), agentes.size()); ++i) {
        auto& agente = agentes[i];
        std::cout << "\n   Agente #" << agente->getId() << ":\n";
        std::cout << "   - Velocidad base: " << agente->getVelocidad() << " m/s\n";
        std::cout << "   - Velocidad efectiva: " << agente->calcularVelocidadEfectiva() << " m/s\n";
        std::cout << "   - Tipo: ";
        
        if (auto persona = std::dynamic_pointer_cast<Persona>(agente)) {
            std::cout << "Persona (edad: " << persona->getEdad();
            if (persona->tieneMovilidadReducida()) {
                std::cout << ", movilidad reducida";
            }
            std::cout << ", pánico: " << (persona->getNivelPanico() * 100) << "%)";
        } else if (std::dynamic_pointer_cast<Rescatista>(agente)) {
            std::cout << "Rescatista";
        }
        std::cout << "\n";
    }
    
    std::cout << "\n========================================\n";
    std::cout << "  DEMOSTRACIÓN COMPLETADA\n";
    std::cout << "========================================\n";
    
    std::cout << "\nConceptos de POO demostrados:\n";
    std::cout << "✓ Herencia (Persona y Rescatista heredan de AgenteBase)\n";
    std::cout << "✓ Polimorfismo (métodos virtuales actualizar, calcularVelocidadEfectiva)\n";
    std::cout << "✓ Patrón Observer (GestorEventos recibe notificaciones)\n";
    std::cout << "✓ Patrón Prototype (clonación de agentes)\n";
    std::cout << "✓ Patrón Factory (FactoriaAgentes crea instancias)\n";
    std::cout << "✓ Punteros inteligentes (shared_ptr, weak_ptr)\n";
    std::cout << "✓ Encapsulación (getters/setters, métodos privados)\n";
    
    return 0;
}
