#include "../include/FactoriaAgentes.h"
#include <random>
#include <cmath> // Necesario para cos, sin

// Inicializar contador estático
int FactoriaAgentes::contadorId = 0;

FactoriaAgentes::FactoriaAgentes() {
    auto personaNormal = std::make_shared<Persona>(0, Posicion(0, 0), 30, false);
    registrarPrototipo("PersonaNormal", personaNormal);
    
    auto personaMayor = std::make_shared<Persona>(0, Posicion(0, 0), 70, false);
    registrarPrototipo("PersonaMayor", personaMayor);
    
    auto personaMovilidadReducida = std::make_shared<Persona>(0, Posicion(0, 0), 40, true);
    registrarPrototipo("PersonaMovilidadReducida", personaMovilidadReducida);
    
    auto rescatista = std::make_shared<Rescatista>(0, Posicion(0, 0));
    registrarPrototipo("Rescatista", rescatista);
}

std::shared_ptr<Persona> FactoriaAgentes::crearPersona(Posicion pos, int edad, bool movilidadReducida) {
    int nuevoId = obtenerSiguienteId();
    return std::make_shared<Persona>(nuevoId, pos, edad, movilidadReducida);
}

std::shared_ptr<Rescatista> FactoriaAgentes::crearRescatista(Posicion pos) {
    int nuevoId = obtenerSiguienteId();
    return std::make_shared<Rescatista>(nuevoId, pos);
}

void FactoriaAgentes::registrarPrototipo(const std::string& nombre, std::shared_ptr<AgenteBase> prototipo) {
    prototipos[nombre] = prototipo;
}

std::shared_ptr<AgenteBase> FactoriaAgentes::clonarDesdePrototipo(const std::string& nombre, Posicion nuevaPos) {
    auto it = prototipos.find(nombre);
    if (it != prototipos.end()) {
        auto clonado = it->second->clonar();
        clonado->setPosicion(nuevaPos);
        return clonado;
    }
    return nullptr;
}

std::vector<std::shared_ptr<Persona>> FactoriaAgentes::crearGrupoPersonas(
    int cantidad, 
    Posicion posInicial, 
    int edadPromedio, 
    double radioDispersion) {
    
    std::vector<std::shared_ptr<Persona>> grupo;
    grupo.reserve(cantidad);
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> disAngulo(0, 2 * 3.14159265359);
    std::uniform_real_distribution<> disRadio(0, radioDispersion);
    std::uniform_int_distribution<> disEdad(edadPromedio - 10, edadPromedio + 10);
    std::uniform_real_distribution<> disProbabilidad(0.0, 1.0);
    
    for (int i = 0; i < cantidad; ++i) {
        double angulo = disAngulo(gen);
        double radio = disRadio(gen);
        
        // CORRECCIÓN MATEMÁTICA: QPoint usa enteros.
        // Convertimos el cálculo trigonométrico a entero para la grid.
        int nuevoX = static_cast<int>(posInicial.x() + radio * std::cos(angulo));
        int nuevoY = static_cast<int>(posInicial.y() + radio * std::sin(angulo));
        
        Posicion pos(nuevoX, nuevoY);
        
        int edad = disEdad(gen);
        bool movilidadReducida = (disProbabilidad(gen) < 0.1);
        
        auto persona = crearPersona(pos, edad, movilidadReducida);
        grupo.push_back(persona);
    }
    
    return grupo;
}

int FactoriaAgentes::obtenerSiguienteId() {
    return ++contadorId;
}