#include "../include/AgenteBase.h"
#include "../include/ObservadorEvento.h"
#include <cmath>
#include <algorithm>
#include <iostream> // Para debug si hace falta

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
 * ADAPTADO PARA QT (QPoint usa enteros)
 */
void AgenteBase::moverSiguientePunto(double deltaTime) {
    if (ruta.empty() || indiceRutaActual >= (int)ruta.size()) {
        return;
    }
    
    // 1. Obtener el siguiente punto de destino
    Posicion objetivo = ruta[indiceRutaActual];
    
    // 2. Calcular dirección y distancia (Usando getters de Qt .x() y .y())
    double dx = objetivo.x() - posicion.x();
    double dy = objetivo.y() - posicion.y();
    double distancia = std::hypot(dx, dy); // hypot es más seguro que sqrt(dx*dx...)
    
    // 3. Si estamos "cerca" (distancia menor a 1 celda), avanzar al siguiente nodo
    // Como es una grid de enteros, distancia < 1.0 significa "ya llegué"
    if (distancia < 0.5) { 
        // Forzamos la posición exacta al nodo para evitar deriva
        posicion = objetivo; 

        indiceRutaActual++;
        if (indiceRutaActual >= (int)ruta.size()) {
            estado = EstadoAgente::EVACUADO;
            notificarEvento("EVACUADO");
            return;
        }
        // Recalcular para el nuevo objetivo
        objetivo = ruta[indiceRutaActual];
        dx = objetivo.x() - posicion.x();
        dy = objetivo.y() - posicion.y();
        distancia = std::hypot(dx, dy);
    }
    
    // 4. Moverse
    if (distancia > 0) {
        // Normalizar dirección
        dx /= distancia;
        dy /= distancia;
        
        // Calcular cuánto nos movemos
        double velocidadEfectiva = calcularVelocidadEfectiva();
        double movimiento = velocidadEfectiva * deltaTime;

        // --- ADAPTACIÓN CRÍTICA PARA QPOINT (ENTEROS) ---
        // QPoint no guarda decimales. Si sumamos 0.2, se queda igual.
        // Solución para Grid: Si el movimiento acumulado es suficiente, damos un paso.
        // O más simple para el deadline: Usar round() para saltar al siguiente pixel.
        
        int nuevoX = posicion.x() + std::round(dx * movimiento);
        int nuevoY = posicion.y() + std::round(dy * movimiento);

        // Seguridad: Si la velocidad es muy baja (<1) y round da 0, el agente se atasca.
        // Truco: Si hay que moverse y el cálculo da 0, forzamos 1 unidad en la dirección correcta.
        if (movimiento > 0.1) {
            if (nuevoX == posicion.x() && dx != 0) nuevoX += (dx > 0 ? 1 : -1);
            if (nuevoY == posicion.y() && dy != 0) nuevoY += (dy > 0 ? 1 : -1);
        }

        posicion.setX(nuevoX);
        posicion.setY(nuevoY);
    }
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
    // Limpiar observadores expirados usando lambda
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