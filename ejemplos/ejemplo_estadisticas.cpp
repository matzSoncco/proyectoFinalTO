/**
 * @file ejemplo_estadisticas.cpp
 * @brief Ejemplo de uso del sistema de estadísticas
 * @author Integrante 3 - Desarrollador de Agentes & POO
 */

#include <iostream>
#include <memory>
#include "../include/Simulador.h"
#include "../include/Escenario.h"
#include "../include/Persona.h"
#include "../include/Rescatista.h"
#include "../include/EstadisticasSimulacion.h"
#include "../include/FactoriaAgentes.h"

void imprimirSeparador() {
    std::cout << "\n" << std::string(70, '=') << "\n\n";
}

/**
 * @brief Ejemplo 1: Simulación básica con estadísticas
 */
void ejemplo1_SimulacionBasica() {
    std::cout << "EJEMPLO 1: Simulación Básica con Estadísticas\n";
    imprimirSeparador();
    
    // Crear simulador
    Simulador sim;
    sim.cargarEscenario(30, 40);
    
    // Configurar escenario simple
    Escenario* esc = sim.getEscenario();
    // Colocar paredes
    for (int i = 10; i < 20; i++) {
        esc->setCelda(i, 10, 1); // Pared horizontal
    }
    // Colocar salidas
    esc->setCelda(5, 0, 2);   // Salida norte
    esc->setCelda(25, 0, 2);  // Salida sur
    
    // Agregar 20 personas con diferentes características
    for (int i = 0; i < 20; i++) {
        auto persona = std::make_shared<Persona>(
            i,                              // ID
            QPoint(15 + (i % 5), 20 + (i / 5)), // Posición inicial
            1.0 + (rand() % 50) / 100.0,    // Velocidad variada
            20 + (rand() % 60),             // Edad aleatoria
            rand() % 2 == 0                 // 50% con movilidad reducida
        );
        
        // 30% de probabilidad de pánico
        if (rand() % 10 < 3) {
            persona->setPanico(true);
        }
        
        sim.agregarAgente(persona);
    }
    
    // Agregar 2 rescatistas
    for (int i = 0; i < 2; i++) {
        auto rescatista = std::make_shared<Rescatista>(
            20 + i,
            QPoint(10 + i * 10, 25),
            1.5,
            3
        );
        sim.agregarAgente(rescatista);
    }
    
    std::cout << "Configuración:\n";
    std::cout << "  - Escenario: 30x40\n";
    std::cout << "  - 20 personas (variadas características)\n";
    std::cout << "  - 2 rescatistas\n";
    std::cout << "  - 2 salidas\n\n";
    
    // NOTA: En una aplicación real con GUI, aquí se iniciaría el QTimer
    // Para este ejemplo, simularemos el ciclo manualmente
    std::cout << "Iniciando simulación...\n";
    std::cout << "(En versión GUI, esto sería automático con QTimer)\n\n";
    
    // Obtener estadísticas finales
    EstadisticasSimulacion* stats = sim.getEstadisticas();
    
    // Mostrar reporte completo
    std::cout << stats->generarReporte();
    
    // Exportar a archivo
    stats->exportarReporte("ejemplo1_resultado.txt");
    stats->exportarReporte("ejemplo1_resultado.csv");
    stats->exportarReporte("ejemplo1_resultado.json");
    
    std::cout << "\n📁 Archivos exportados:\n";
    std::cout << "  - ejemplo1_resultado.txt (Reporte completo)\n";
    std::cout << "  - ejemplo1_resultado.csv (Para Excel)\n";
    std::cout << "  - ejemplo1_resultado.json (Para análisis programático)\n";
}

/**
 * @brief Ejemplo 2: Comparación de múltiples escenarios
 */
void ejemplo2_ComparacionEscenarios() {
    std::cout << "\n\nEJEMPLO 2: Comparación de Escenarios\n";
    imprimirSeparador();
    
    struct ResultadoComparacion {
        std::string nombre;
        int numSalidas;
        double tiempoPromedio;
        int colisiones;
        double tasaEvacuacion;
    };
    
    std::vector<ResultadoComparacion> resultados;
    
    // Simular 3 escenarios diferentes
    for (int numSalidas = 2; numSalidas <= 4; numSalidas++) {
        std::cout << "Ejecutando escenario con " << numSalidas << " salidas...\n";
        
        Simulador sim;
        sim.cargarEscenario(30, 40);
        Escenario* esc = sim.getEscenario();
        
        // Colocar salidas según el número
        for (int i = 0; i < numSalidas; i++) {
            int x = 5 + (i * 30 / numSalidas);
            esc->setCelda(x, 0, 2);
        }
        
        // Agregar 30 personas
        for (int i = 0; i < 30; i++) {
            auto persona = std::make_shared<Persona>(
                i,
                QPoint(10 + (i % 10), 15 + (i / 10)),
                1.0,
                30,
                false
            );
            sim.agregarAgente(persona);
        }
        
        // Simular (en GUI sería con timer)
        // ...
        
        EstadisticasGlobales stats = sim.getEstadisticas()->getEstadisticas();
        
        ResultadoComparacion resultado;
        resultado.nombre = "Escenario_" + std::to_string(numSalidas) + "_salidas";
        resultado.numSalidas = numSalidas;
        resultado.tiempoPromedio = stats.tiempoPromedioEvacuacion;
        resultado.colisiones = stats.colisionesTotales;
        resultado.tasaEvacuacion = stats.tasaEvacuacion;
        
        resultados.push_back(resultado);
        
        // Exportar cada escenario
        std::string archivo = "comparacion_" + std::to_string(numSalidas) + "salidas.csv";
        sim.getEstadisticas()->exportarReporte(archivo);
    }
    
    // Mostrar comparación
    std::cout << "\n📊 RESULTADOS DE COMPARACIÓN:\n\n";
    std::cout << "Salidas | Tiempo Prom. | Colisiones | Tasa Evac.\n";
    std::cout << "--------|--------------|------------|------------\n";
    
    for (const auto& r : resultados) {
        std::cout << "   " << r.numSalidas << "    | ";
        std::cout << std::fixed << std::setprecision(2) << r.tiempoPromedio << "s     | ";
        std::cout << std::setw(10) << r.colisiones << " | ";
        std::cout << r.tasaEvacuacion << " p/s\n";
    }
    
    std::cout << "\n💡 CONCLUSIÓN: ";
    
    // Encontrar el mejor escenario
    auto mejor = *std::min_element(resultados.begin(), resultados.end(),
        [](const ResultadoComparacion& a, const ResultadoComparacion& b) {
            return a.tiempoPromedio < b.tiempoPromedio;
        });
    
    std::cout << "El escenario con " << mejor.numSalidas 
              << " salidas fue el más eficiente.\n";
}

/**
 * @brief Ejemplo 3: Análisis de cuellos de botella
 */
void ejemplo3_CuellosBottella() {
    std::cout << "\n\nEJEMPLO 3: Detección de Cuellos de Botella\n";
    imprimirSeparador();
    
    Simulador sim;
    sim.cargarEscenario(30, 40);
    Escenario* esc = sim.getEscenario();
    
    // Crear un pasillo estrecho (cuello de botella intencional)
    for (int i = 0; i < 30; i++) {
        if (i != 15) { // Dejar solo un espacio
            esc->setCelda(i, 15, 1);
        }
    }
    
    // Salida al otro lado del pasillo
    esc->setCelda(15, 0, 2);
    
    // Muchos agentes del otro lado
    for (int i = 0; i < 40; i++) {
        auto persona = std::make_shared<Persona>(
            i,
            QPoint(5 + (i % 8), 20 + (i / 8)),
            1.0,
            30,
            false
        );
        sim.agregarAgente(persona);
    }
    
    std::cout << "Configuración:\n";
    std::cout << "  - Pasillo estrecho de 1 celda de ancho\n";
    std::cout << "  - 40 agentes tratando de pasar\n";
    std::cout << "  - 1 salida al otro lado\n\n";
    
    // Simular...
    
    EstadisticasSimulacion* stats = sim.getEstadisticas();
    std::vector<QPoint> cuellos = stats->getCuellosBotellaDetectados();
    
    std::cout << "⚠️  CUELLOS DE BOTELLA DETECTADOS: " << cuellos.size() << "\n\n";
    
    for (const auto& punto : cuellos) {
        std::cout << "  📍 Posición: (" << punto.x() << ", " << punto.y() << ")\n";
    }
    
    EstadisticasGlobales globales = stats->getEstadisticas();
    
    std::cout << "\n📊 Impacto del cuello de botella:\n";
    std::cout << "  - Colisiones totales: " << globales.colisionesTotales << "\n";
    std::cout << "  - Tiempo máximo: " << globales.tiempoMaximoEvacuacion << "s\n";
    std::cout << "  - Tiempo promedio: " << globales.tiempoPromedioEvacuacion << "s\n";
    
    std::cout << "\n💡 RECOMENDACIÓN: Ampliar el pasillo o agregar rutas alternativas\n";
}

/**
 * @brief Ejemplo 4: Uso de la Factory con estadísticas
 */
void ejemplo4_UsoFactory() {
    std::cout << "\n\nEJEMPLO 4: Uso de Factory con Estadísticas\n";
    imprimirSeparador();
    
    Simulador sim;
    sim.cargarEscenario(30, 40);
    Escenario* esc = sim.getEscenario();
    
    esc->setCelda(15, 0, 2); // Salida
    
    FactoriaAgentes fabrica;
    
    // Crear diferentes tipos de agentes
    std::cout << "Creando agentes usando Factory:\n";
    
    // 10 personas normales
    for (int i = 0; i < 10; i++) {
        auto agente = fabrica.crearPersona(i, QPoint(10 + i, 20), 1.0, 30, false);
        sim.agregarAgente(agente);
    }
    std::cout << "  ✓ 10 personas normales\n";
    
    // 5 personas con movilidad reducida
    for (int i = 10; i < 15; i++) {
        auto agente = fabrica.crearPersona(i, QPoint(10 + (i-10), 22), 0.5, 65, true);
        sim.agregarAgente(agente);
    }
    std::cout << "  ✓ 5 personas con movilidad reducida\n";
    
    // 3 rescatistas
    for (int i = 15; i < 18; i++) {
        auto agente = fabrica.crearRescatista(i, QPoint(15, 25), 1.5, 2);
        sim.agregarAgente(agente);
    }
    std::cout << "  ✓ 3 rescatistas\n";
    
    // Simular...
    
    EstadisticasGlobales stats = sim.getEstadisticas()->getEstadisticas();
    
    std::cout << "\n📊 Resultados por tipo:\n";
    std::cout << "  - Personas evacuadas: " << stats.personasEvacuadas << "\n";
    std::cout << "  - Rescatistas evacuados: " << stats.rescatistasEvacuados << "\n";
    std::cout << "  - Personas con movilidad reducida: " << stats.personasMovilidadReducida << "\n";
}

/**
 * @brief Menú principal
 */
int main() {
    std::cout << "\n";
    std::cout << "╔════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║  EJEMPLOS DE USO - SISTEMA DE ESTADÍSTICAS                    ║\n";
    std::cout << "║  Simulador de Evacuación                                      ║\n";
    std::cout << "╚════════════════════════════════════════════════════════════════╝\n";
    
    std::cout << "\nSelecciona un ejemplo:\n";
    std::cout << "1. Simulación básica con estadísticas\n";
    std::cout << "2. Comparación de múltiples escenarios\n";
    std::cout << "3. Detección de cuellos de botella\n";
    std::cout << "4. Uso de Factory con estadísticas\n";
    std::cout << "5. Ejecutar todos los ejemplos\n";
    std::cout << "0. Salir\n\n";
    std::cout << "Opción: ";
    
    int opcion;
    std::cin >> opcion;
    
    switch (opcion) {
        case 1:
            ejemplo1_SimulacionBasica();
            break;
        case 2:
            ejemplo2_ComparacionEscenarios();
            break;
        case 3:
            ejemplo3_CuellosBottella();
            break;
        case 4:
            ejemplo4_UsoFactory();
            break;
        case 5:
            ejemplo1_SimulacionBasica();
            ejemplo2_ComparacionEscenarios();
            ejemplo3_CuellosBottella();
            ejemplo4_UsoFactory();
            break;
        case 0:
            std::cout << "Saliendo...\n";
            break;
        default:
            std::cout << "Opción inválida\n";
    }
    
    imprimirSeparador();
    std::cout << "Universidad Nacional de San Agustín de Arequipa\n";
    std::cout << "Tópicos en Objetos - 2025\n\n";
    
    return 0;
}
