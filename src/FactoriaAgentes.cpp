#include "../include/FactoriaAgentes.h"
#include <random>

// Inicializar contador estático
int FactoriaAgentes::contadorId = 0;

/**
 * Constructor
 */
FactoriaAgentes::FactoriaAgentes() {
    // Crear algunos prototipos por defecto
    auto personaNormal = std::make_shared<Persona>(0, Posicion(0, 0), 30, false);
    registrarPrototipo("PersonaNormal", personaNormal);
    
    auto personaMayor = std::make_shared<Persona>(0, Posicion(0, 0), 70, false);
    registrarPrototipo("PersonaMayor", personaMayor);
    
    auto personaMovilidadReducida = std::make_shared<Persona>(0, Posicion(0, 0), 40, true);
    registrarPrototipo("PersonaMovilidadReducida", personaMovilidadReducida);
    
    auto rescatista = std::make_shared<Rescatista>(0, Posicion(0, 0));
    registrarPrototipo("Rescatista", rescatista);
}

/**
 * Crea una nueva persona con parámetros específicos
 */
std::shared_ptr<Persona> FactoriaAgentes::crearPersona(Posicion pos, int edad, bool movilidadReducida) {
    int nuevoId = obtenerSiguienteId();
    return std::make_shared<Persona>(nuevoId, pos, edad, movilidadReducida);
}

/**
 * Crea un nuevo rescatista
 */
std::shared_ptr<Rescatista> FactoriaAgentes::crearRescatista(Posicion pos) {
    int nuevoId = obtenerSiguienteId();
    return std::make_shared<Rescatista>(nuevoId, pos);
}

/**
 * Registra un prototipo para clonación posterior
 */
void FactoriaAgentes::registrarPrototipo(const std::string& nombre, std::shared_ptr<AgenteBase> prototipo) {
    prototipos[nombre] = prototipo;
}

/**
 * Clona un agente a partir de un prototipo registrado
 */
std::shared_ptr<AgenteBase> FactoriaAgentes::clonarDesdePrototipo(const std::string& nombre, Posicion nuevaPos) {
    auto it = prototipos.find(nombre);
    if (it != prototipos.end()) {
        auto clonado = it->second->clonar();
        clonado->setPosicion(nuevaPos);
        return clonado;
    }
    return nullptr;
}

/**
 * Crea múltiples personas con características similares
 */
std::vector<std::shared_ptr<Persona>> FactoriaAgentes::crearGrupoPersonas(
    int cantidad, 
    Posicion posInicial, 
    int edadPromedio, 
    double radioDispersion) {
    
    std::vector<std::shared_ptr<Persona>> grupo;
    grupo.reserve(cantidad);
    
    // Generador de números aleatorios
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> disAngulo(0, 2 * 3.14159265359);
    std::uniform_real_distribution<> disRadio(0, radioDispersion);
    std::uniform_int_distribution<> disEdad(edadPromedio - 10, edadPromedio + 10);
    std::uniform_real_distribution<> disProbabilidad(0.0, 1.0);
    
    for (int i = 0; i < cantidad; ++i) {
        // Generar posición aleatoria dentro del radio
        double angulo = disAngulo(gen);
        double radio = disRadio(gen);
        Posicion pos(
            posInicial.x + radio * std::cos(angulo),
            posInicial.y + radio * std::sin(angulo)
        );
        
        // Generar edad aleatoria
        int edad = disEdad(gen);
        
        // 10% de probabilidad de tener movilidad reducida
        bool movilidadReducida = (disProbabilidad(gen) < 0.1);
        
        // Crear persona
        auto persona = crearPersona(pos, edad, movilidadReducida);
        grupo.push_back(persona);
    }
    
    return grupo;
}

/**
 * Obtiene el siguiente ID disponible
 */
int FactoriaAgentes::obtenerSiguienteId() {
    return ++contadorId;
}
