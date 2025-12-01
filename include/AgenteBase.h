#ifndef AGENTEBASE_H
#define AGENTEBASE_H

#include <memory>
#include <string>
#include <vector>
#include <QPoint>

/**
 * @brief Enumeración para los diferentes estados de un agente
 */
enum class EstadoAgente {
    NORMAL,
    EVACUANDO,
    PANICO,
    EVACUADO,
    BLOQUEADO
};

/**
 * @brief Enumeración para los tipos de comportamiento
 */
enum class TipoComportamiento {
    NORMAL,
    PANICO,
    MOVILIDAD_REDUCIDA,
    RESCATISTA
};

typedef QPoint Posicion;

// Forward declaration para el patrón Observer
class ObservadorEvento;

/**
 * @brief Clase abstracta base para todos los agentes en la simulación
 * 
 * Esta clase define la interfaz común para todos los tipos de agentes
 * (personas, rescatistas, etc.) aplicando el principio de polimorfismo.
 */
class AgenteBase {
protected:
    int id;
    Posicion posicion;
    Posicion destino;
    double velocidad;
    EstadoAgente estado;
    TipoComportamiento tipoComportamiento;
    std::vector<Posicion> ruta;
    int indiceRutaActual;
    
    // Para el patrón Observer
    std::vector<std::weak_ptr<ObservadorEvento>> observadores;
    
public:
    /**
     * @brief Constructor de la clase base
     */
    AgenteBase(int id, Posicion posInicial, double velocidad, TipoComportamiento tipo);
    
    /**
     * @brief Destructor virtual para permitir polimorfismo
     */
    virtual ~AgenteBase() = default;
    
    /**
     * @brief Método virtual puro para actualizar el estado del agente
     * @param deltaTime Tiempo transcurrido desde la última actualización
     */
    virtual void actualizar(double deltaTime) = 0;
    
    /**
     * @brief Método virtual puro para calcular la velocidad efectiva
     * @return Velocidad ajustada según el comportamiento del agente
     */
    virtual double calcularVelocidadEfectiva() const = 0;
    
    /**
     * @brief Método virtual puro para reaccionar ante obstáculos
     */
    virtual void reaccionarObstaculo() = 0;
    
    /**
     * @brief Método virtual para clonar el agente (Patrón Prototype)
     * @return Puntero compartido a un nuevo agente clonado
     */
    virtual std::shared_ptr<AgenteBase> clonar() const = 0;
    
    /**
     * @brief Establece la ruta que debe seguir el agente
     */
    void establecerRuta(const std::vector<Posicion>& nuevaRuta);
    
    /**
     * @brief Mueve al agente hacia el siguiente punto de la ruta
     */
    void moverSiguientePunto(double deltaTime);
    
    /**
     * @brief Verifica si el agente ha llegado a su destino
     */
    bool haLlegadoDestino() const;
    
    // Getters
    int getId() const { return id; }
    Posicion getPosicion() const { return posicion; }
    double getVelocidad() const { return velocidad; }
    EstadoAgente getEstado() const { return estado; }
    TipoComportamiento getTipoComportamiento() const { return tipoComportamiento; }
    
    // Setters
    void setPosicion(const Posicion& pos) { posicion = pos; }
    void setEstado(EstadoAgente nuevoEstado);
    void setVelocidad(double nuevaVelocidad) { velocidad = nuevaVelocidad; }
    
    // Métodos para el patrón Observer
    void agregarObservador(std::weak_ptr<ObservadorEvento> observador);
    void notificarEvento(const std::string& evento);
};

#endif // AGENTEBASE_H
