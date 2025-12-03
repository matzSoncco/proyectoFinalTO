#include "../include/Persona.h"
#include <algorithm>
#include <random>

/**
 * Constructor de Persona
 */
Persona::Persona(int id, Posicion posInicial, int edad, bool movilidadReducida)
    : AgenteBase(id, posInicial, 1.5,
                 movilidadReducida ? TipoComportamiento::MOVILIDAD_REDUCIDA : TipoComportamiento::NORMAL),
    edad(edad),
    nivelPanico(0.0),
    movilidadReducida(movilidadReducida),
    factorVelocidadBase(1.0),
    escenario(nullptr) {  // NUEVO: Puntero al escenario

    // Calcular velocidad base según edad y movilidad
    factorVelocidadBase = calcularFactorEdad();

    // Ajustar velocidad base
    if (movilidadReducida) {
        velocidad = 0.8;  // Velocidad reducida
    } else if (edad < 18) {
        velocidad = 1.8;  // Jóvenes más rápidos
    } else if (edad >= 18 && edad < 60) {
        velocidad = 1.5;  // Adultos velocidad normal
    } else {
        velocidad = 1.0;  // Ancianos más lentos
    }
}

/**
 * NUEVO: Establece la referencia al escenario
 */
void Persona::setEscenario(Escenario* esc) {
    escenario = esc;
}

/**
 * Actualiza el estado de la persona en cada frame
 */
void Persona::actualizar(double deltaTime) {
    if (estado == EstadoAgente::EVACUADO) {
        return;
    }

    // NUEVO: Verificar si llegó a una salida ANTES de actualizar
    if (escenario != nullptr && verificarLlegadaSalida()) {
        setEstado(EstadoAgente::EVACUADO);
        notificarEvento("EVACUACION_EXITOSA");
        return;  // No seguir actualizando, la persona ya evacuó
    }

    // Actualizar comportamiento según pánico
    actualizarComportamientoPanico();

    // Mover hacia el siguiente punto de la ruta
    moverSiguientePunto(deltaTime);

    // NUEVO: Verificar nuevamente después del movimiento
    if (escenario != nullptr && verificarLlegadaSalida()) {
        setEstado(EstadoAgente::EVACUADO);
        notificarEvento("EVACUACION_EXITOSA");
        return;
    }

    // Decrementar pánico gradualmente si no hay factores externos
    if (nivelPanico > 0.0) {
        reducirPanico(0.1 * deltaTime);
    }
}

/**
 * NUEVO: Verifica si la persona llegó a una salida
 */
bool Persona::verificarLlegadaSalida() const {
    if (escenario == nullptr) {
        return false;
    }

    // Obtener posición actual en la grilla
    int x = static_cast<int>(std::round(posicion.x()));
    int y = static_cast<int>(std::round(posicion.y()));

    // Verificar si está en una celda de salida
    return escenario->esSalida(x, y);
}

/**
 * NUEVO: Verifica si está cerca de una salida (radio de 0.5 unidades)
 */
bool Persona::estaCercaDeSalida() const {
    if (escenario == nullptr) {
        return false;
    }

    // Verificar posición exacta
    int x = static_cast<int>(std::round(posicion.x()));
    int y = static_cast<int>(std::round(posicion.y()));

    if (escenario->esSalida(x, y)) {
        return true;
    }

    // Verificar celdas adyacentes (para detección más suave)
    const int dx[] = {-1, 0, 1, -1, 1, -1, 0, 1};
    const int dy[] = {-1, -1, -1, 0, 0, 1, 1, 1};

    for (int i = 0; i < 8; ++i) {
        int nx = x + dx[i];
        int ny = y + dy[i];

        if (escenario->esSalida(nx, ny)) {
            // Calcular distancia real
            double distX = posicion.x() - nx;
            double distY = posicion.y() - ny;
            double dist = std::sqrt(distX * distX + distY * distY);

            if (dist < 0.7) {  // Umbral de cercanía
                return true;
            }
        }
    }

    return false;
}

/**
 * Calcula la velocidad efectiva considerando edad, pánico y movilidad
 */
double Persona::calcularVelocidadEfectiva() const {
    double velocidadEfectiva = velocidad * factorVelocidadBase;

    // Efecto del pánico en la velocidad
    if (estEnPanico()) {
        // En pánico, las personas pueden moverse más rápido pero de forma errática
        velocidadEfectiva *= 1.3;
    }

    // Efecto de movilidad reducida
    if (movilidadReducida) {
        velocidadEfectiva *= 0.6;
    }

    return velocidadEfectiva;
}

/**
 * Define cómo reacciona la persona ante un obstáculo
 */
void Persona::reaccionarObstaculo() {
    // Incrementar pánico al encontrar obstáculo
    incrementarPanico(0.2);

    // Cambiar estado si se bloquea
    if (nivelPanico > 0.8) {
        setEstado(EstadoAgente::BLOQUEADO);
    }

    // Notificar el evento
    notificarEvento("OBSTACULO_ENCONTRADO");
}

/**
 * Clona la persona (Patrón Prototype)
 */
std::shared_ptr<AgenteBase> Persona::clonar() const {
    // Crear una nueva persona con las mismas características
    auto nuevaPersona = std::make_shared<Persona>(
        -1,  // El ID se asignará después
        posicion,
        edad,
        movilidadReducida
        );

    // Copiar nivel de pánico y estado
    nuevaPersona->nivelPanico = this->nivelPanico;
    nuevaPersona->factorVelocidadBase = this->factorVelocidadBase;
    nuevaPersona->escenario = this->escenario;  // NUEVO: Copiar referencia al escenario

    return nuevaPersona;
}

/**
 * Incrementa el nivel de pánico
 */
void Persona::incrementarPanico(double cantidad) {
    nivelPanico = std::min(1.0, nivelPanico + cantidad);

    // Si alcanza el umbral de pánico, cambiar estado
    if (nivelPanico > 0.7 && estado != EstadoAgente::PANICO) {
        setEstado(EstadoAgente::PANICO);
    }
}

/**
 * Reduce el nivel de pánico
 */
void Persona::reducirPanico(double cantidad) {
    nivelPanico = std::max(0.0, nivelPanico - cantidad);

    // Si el pánico baja, volver a estado normal
    if (nivelPanico < 0.5 && estado == EstadoAgente::PANICO) {
        setEstado(EstadoAgente::EVACUANDO);
    }
}

/**
 * Calcula el factor de velocidad basado en la edad
 */
double Persona::calcularFactorEdad() const {
    if (edad < 12) {
        return 0.8;  // Niños pequeños
    } else if (edad < 18) {
        return 1.1;  // Adolescentes
    } else if (edad < 60) {
        return 1.0;  // Adultos
    } else if (edad < 75) {
        return 0.7;  // Adultos mayores
    } else {
        return 0.5;  // Ancianos
    }
}

/**
 * Actualiza el comportamiento según el nivel de pánico
 */
void Persona::actualizarComportamientoPanico() {
    if (estEnPanico()) {
        tipoComportamiento = TipoComportamiento::PANICO;

        // En pánico extremo, puede bloquearse
        if (nivelPanico > 0.9) {
            // Pequeña probabilidad de bloquearse
            static std::random_device rd;
            static std::mt19937 gen(rd());
            static std::uniform_real_distribution<> dis(0.0, 1.0);

            if (dis(gen) < 0.01) {  // 1% de probabilidad por frame
                setEstado(EstadoAgente::BLOQUEADO);
            }
        }
    } else {
        if (movilidadReducida) {
            tipoComportamiento = TipoComportamiento::MOVILIDAD_REDUCIDA;
        } else {
            tipoComportamiento = TipoComportamiento::NORMAL;
        }
    }
}
