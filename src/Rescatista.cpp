#include "../include/Rescatista.h"
#include "../include/Persona.h"
#include <cmath>
#include <algorithm> // Para std::max o lógica adicional si se requiere

/**
 * Constructor de Rescatista
 */
Rescatista::Rescatista(int id, Posicion posInicial)
    : AgenteBase(id, posInicial, 2.5, TipoComportamiento::RESCATISTA),
      estaAsistiendo(false),
      capacidadCarga(0.7) {
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
    
    notificarEvento("ASISTIENDO_PERSONA");
}

/**
 * Libera a la persona asistida
 */
void Rescatista::liberarPersona() {
    if (estaAsistiendo) {
        auto persona = personaAsistida.lock();
        if (persona) {
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
            
            // CORRECCIÓN MATEMÁTICA: Usamos hypot para calcular distancia entre QPoints
            // QPoint usa .x() y .y(), no .x y .y
            double dx = posicion.x() - persona->getPosicion().x();
            double dy = posicion.y() - persona->getPosicion().y();
            double dist = std::hypot(dx, dy);
            
            if (dist < distanciaMinima) {
                distanciaMinima = dist;
                personaMasCercana = persona;
            }
        }
    }
    
    if (personaMasCercana) {
        asistirPersona(personaMasCercana);
    }
}