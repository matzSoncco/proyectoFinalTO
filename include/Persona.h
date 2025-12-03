#ifndef PERSONA_H
#define PERSONA_H

#include "AgenteBase.h"
#include "Escenario.h"

/**
 * @brief Clase que representa una persona en la simulación
 * 
 * Hereda de AgenteBase e implementa comportamientos específicos
 * como pánico, movilidad reducida, etc.
 */
class Persona : public AgenteBase {
private:
    int edad;
    double nivelPanico;
    bool movilidadReducida;
    double factorVelocidadBase;
    Escenario* escenario;
    
public:
    /**
     * @brief Constructor de Persona
     */
    Persona(int id, Posicion posInicial, int edad = 30, bool movilidadReducida = false);
    
    /**
     * @brief Destructor
     */
    ~Persona() override = default;
    
    /**
     * @brief Actualiza el estado de la persona en cada frame
     */
    void actualizar(double deltaTime) override;
    
    /**
     * @brief Calcula la velocidad efectiva considerando edad, pánico y movilidad
     */
    double calcularVelocidadEfectiva() const override;
    
    /**
     * @brief Define cómo reacciona la persona ante un obstáculo
     */
    void reaccionarObstaculo() override;
    
    /**
     * @brief Clona la persona (Patrón Prototype)
     */
    std::shared_ptr<AgenteBase> clonar() const override;
    
    /**
     * @brief Incrementa el nivel de pánico
     */
    void incrementarPanico(double cantidad);
    
    /**
     * @brief Reduce el nivel de pánico
     */
    void reducirPanico(double cantidad);
    
    /**
     * @brief Verifica si la persona está en pánico
     */
    bool estEnPanico() const { return nivelPanico > 0.7; }
    
    // Getters específicos
    int getEdad() const { return edad; }
    double getNivelPanico() const { return nivelPanico; }
    bool tieneMovilidadReducida() const { return movilidadReducida; }
    void setEscenario(Escenario* esc);
    bool verificarLlegadaSalida() const;
    bool estaCercaDeSalida() const;
    
private:
    /**
     * @brief Calcula el factor de velocidad basado en la edad
     */
    double calcularFactorEdad() const;
    
    /**
     * @brief Actualiza el comportamiento según el nivel de pánico
     */
    void actualizarComportamientoPanico();
};

#endif // PERSONA_H
