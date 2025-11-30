#include "../include/AgenteBase.h"
#include "../include/ObservadorEvento.h"
#include <cmath>
#include <algorithm>

/**
 * Constructor de AgenteBase
 */
AgenteBase::AgenteBase(int id, Posicion posInicial, double velocidad, TipoComportamiento tipo)
    : id(id), 
      posicion(posInicial), 
      destino(posInicial),
      velocidad(velocidad), 
      estado(EstadoAgente::NORMAL),
      tipoComportamiento(tipo),
      indiceRutaActual(0) {
}

/**
 * Establece la ruta que debe seguir el agente
 */
void AgenteBase::establecerRuta(const std::vector<Posicion>& nuevaRuta) {
    ruta = nuevaRuta;
    indiceRutaActual = 0;
    if (!ruta.empty()) {
        destino = ruta[0];
        estado = EstadoAgente::EVACUANDO;
        notificarEvento("INICIO_EVACUACION");
    }
}

/**
 * Mueve al agente hacia el siguiente punto de la ruta
 */
void AgenteBase::moverSiguientePunto(double deltaTime) {
    if (ruta.empty() || indiceRutaActual >= ruta.size()) {
        return;
    }
    
    // Obtener el siguiente punto de destino
    Posicion objetivo = ruta[indiceRutaActual];
    
    // Calcular dirección
    double dx = objetivo.x - posicion.x;
    double dy = objetivo.y - posicion.y;
    double distancia = std::sqrt(dx * dx + dy * dy);
    
    // Si estamos cerca del punto objetivo, avanzar al siguiente
    if (distancia < 1.0) {
        indiceRutaActual++;
        if (indiceRutaActual >= ruta.size()) {
            estado = EstadoAgente::EVACUADO;
            notificarEvento("EVACUADO");
            return;
        }
        objetivo = ruta[indiceRutaActual];
        dx = objetivo.x - posicion.x;
        dy = objetivo.y - posicion.y;
        distancia = std::sqrt(dx * dx + dy * dy);
    }
    
    // Normalizar dirección
    if (distancia > 0) {
        dx /= distancia;
        dy /= distancia;
    }
    
    // Calcular movimiento usando la velocidad efectiva
    double velocidadEfectiva = calcularVelocidadEfectiva();
    double movimiento = velocidadEfectiva * deltaTime;
    
    // Actualizar posición
    posicion.x += dx * movimiento;
    posicion.y += dy * movimiento;
}

/**
 * Verifica si el agente ha llegado a su destino
 */
bool AgenteBase::haLlegadoDestino() const {
    return estado == EstadoAgente::EVACUADO;
}

/**
 * Cambia el estado del agente y notifica
 */
void AgenteBase::setEstado(EstadoAgente nuevoEstado) {
    if (estado != nuevoEstado) {
        estado = nuevoEstado;
        
        // Notificar cambio de estado
        switch (nuevoEstado) {
            case EstadoAgente::EVACUANDO:
                notificarEvento("EVACUANDO");
                break;
            case EstadoAgente::PANICO:
                notificarEvento("PANICO");
                break;
            case EstadoAgente::EVACUADO:
                notificarEvento("EVACUADO");
                break;
            case EstadoAgente::BLOQUEADO:
                notificarEvento("BLOQUEADO");
                break;
            default:
                break;
        }
    }
}

/**
 * Agrega un observador para recibir notificaciones
 */
void AgenteBase::agregarObservador(std::weak_ptr<ObservadorEvento> observador) {
    observadores.push_back(observador);
}

/**
 * Notifica a todos los observadores sobre un evento
 */
void AgenteBase::notificarEvento(const std::string& evento) {
    // Limpiar observadores expirados
    observadores.erase(
        std::remove_if(observadores.begin(), observadores.end(),
                      [](const std::weak_ptr<ObservadorEvento>& wp) { return wp.expired(); }),
        observadores.end()
    );
    
    // Notificar a todos los observadores válidos
    for (auto& weakObs : observadores) {
        if (auto obs = weakObs.lock()) {
            obs->onEventoAgente(this, evento);
        }
    }
}
