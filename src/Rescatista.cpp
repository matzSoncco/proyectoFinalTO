#include "../include/Rescatista.h"
#include "../include/Persona.h"
#include <cmath>

/**
 * Constructor de Rescatista
 */
Rescatista::Rescatista(int id, Posicion posInicial)
    : AgenteBase(id, posInicial, 2.5, TipoComportamiento::RESCATISTA),
      estaAsistiendo(false),
      capacidadCarga(0.7) {
    // Los rescatistas son más rápidos que las personas normales
}

/**
 * Actualiza el estado del rescatista en cada frame
 */
void Rescatista::actualizar(double deltaTime) {
    if (estado == EstadoAgente::EVACUADO) {
        return;
    }
    
    // Si está asistiendo a alguien, actualizar su posición también
    if (estaAsistiendo) {
        auto persona = personaAsistida.lock();
        if (persona) {
            // La persona asistida se mueve con el rescatista
            persona->setPosicion(posicion);
        } else {
            // Si la persona ya no existe, liberar
            estaAsistiendo = false;
        }
    }
    
    // Mover hacia el siguiente punto de la ruta
    moverSiguientePunto(deltaTime);
}

/**
 * Calcula la velocidad efectiva (considerando si está asistiendo)
 */
double Rescatista::calcularVelocidadEfectiva() const {
    if (estaAsistiendo) {
        // Velocidad reducida al asistir a alguien
        return velocidad * capacidadCarga;
    }
    return velocidad;
}

/**
 * Define cómo reacciona el rescatista ante un obstáculo
 */
void Rescatista::reaccionarObstaculo() {
    // Los rescatistas no entran en pánico
    // Simplemente buscan rutas alternativas (esto se manejará en el algoritmo de ruteo)
    notificarEvento("RESCATISTA_OBSTACULO");
}

/**
 * Clona el rescatista (Patrón Prototype)
 */
std::shared_ptr<AgenteBase> Rescatista::clonar() const {
    auto nuevoRescatista = std::make_shared<Rescatista>(-1, posicion);
    nuevoRescatista->capacidadCarga = this->capacidadCarga;
    return nuevoRescatista;
}

/**
 * Asigna una persona para asistir
 */
void Rescatista::asistirPersona(std::shared_ptr<Persona> persona) {
    if (!persona) return;
    
    personaAsistida = persona;
    estaAsistiendo = true;
    
    // Notificar que está asistiendo
    notificarEvento("ASISTIENDO_PERSONA");
}

/**
 * Libera a la persona asistida
 */
void Rescatista::liberarPersona() {
    if (estaAsistiendo) {
        auto persona = personaAsistida.lock();
        if (persona) {
            // Reducir el pánico de la persona asistida
            persona->reducirPanico(0.5);
        }
        estaAsistiendo = false;
        personaAsistida.reset();
        notificarEvento("PERSONA_LIBERADA");
    }
}

/**
 * Busca personas cercanas que necesiten asistencia
 */
void Rescatista::buscarPersonasQueNecesitanAyuda(const std::vector<std::shared_ptr<AgenteBase>>& agentes) {
    // Si ya está asistiendo, no buscar más
    if (estaAsistiendo) {
        return;
    }
    
    const double RADIO_BUSQUEDA = 50.0;
    std::shared_ptr<Persona> personaMasCercana = nullptr;
    double distanciaMinima = RADIO_BUSQUEDA;
    
    for (const auto& agente : agentes) {
        // Intentar convertir a Persona
        auto persona = std::dynamic_pointer_cast<Persona>(agente);
        
        if (persona && 
            (persona->tieneMovilidadReducida() || 
             persona->estEnPanico() || 
             persona->getEstado() == EstadoAgente::BLOQUEADO)) {
            
            double dist = posicion.distancia(persona->getPosicion());
            
            if (dist < distanciaMinima) {
                distanciaMinima = dist;
                personaMasCercana = persona;
            }
        }
    }
    
    // Si encontró alguien que necesita ayuda, ir a asistir
    if (personaMasCercana) {
        asistirPersona(personaMasCercana);
    }
}
