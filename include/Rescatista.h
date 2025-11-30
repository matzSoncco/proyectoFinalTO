#ifndef RESCATISTA_H
#define RESCATISTA_H

#include "AgenteBase.h"
#include <memory>

// Forward declaration
class Persona;

/**
 * @brief Clase que representa un rescatista en la simulación
 * 
 * Los rescatistas tienen comportamientos especiales:
 * - Mayor velocidad
 * - No entran en pánico
 * - Pueden asistir a personas con movilidad reducida
 */
class Rescatista : public AgenteBase {
private:
    std::weak_ptr<Persona> personaAsistida;
    bool estaAsistiendo;
    double capacidadCarga; // Factor que afecta velocidad al asistir
    
public:
    /**
     * @brief Constructor de Rescatista
     */
    Rescatista(int id, Posicion posInicial);
    
    /**
     * @brief Destructor
     */
    ~Rescatista() override = default;
    
    /**
     * @brief Actualiza el estado del rescatista en cada frame
     */
    void actualizar(double deltaTime) override;
    
    /**
     * @brief Calcula la velocidad efectiva (considerando si está asistiendo)
     */
    double calcularVelocidadEfectiva() const override;
    
    /**
     * @brief Define cómo reacciona el rescatista ante un obstáculo
     */
    void reaccionarObstaculo() override;
    
    /**
     * @brief Clona el rescatista (Patrón Prototype)
     */
    std::shared_ptr<AgenteBase> clonar() const override;
    
    /**
     * @brief Asigna una persona para asistir
     */
    void asistirPersona(std::shared_ptr<Persona> persona);
    
    /**
     * @brief Libera a la persona asistida
     */
    void liberarPersona();
    
    /**
     * @brief Busca personas cercanas que necesiten asistencia
     */
    void buscarPersonasQueNecesitanAyuda(const std::vector<std::shared_ptr<AgenteBase>>& agentes);
    
    // Getters
    bool getEstaAsistiendo() const { return estaAsistiendo; }
    std::shared_ptr<Persona> getPersonaAsistida() const { return personaAsistida.lock(); }
};

#endif // RESCATISTA_H
