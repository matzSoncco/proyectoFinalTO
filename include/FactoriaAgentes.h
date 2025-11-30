#ifndef FACTORIAAGENTES_H
#define FACTORIAAGENTES_H

#include "AgenteBase.h"
#include "Persona.h"
#include "Rescatista.h"
#include <memory>
#include <map>
#include <string>

/**
 * @brief Factory para crear y clonar agentes
 * 
 * Esta clase implementa el patrón Factory Method combinado con Prototype
 * para facilitar la creación y clonación de agentes.
 */
class FactoriaAgentes {
private:
    static int contadorId;
    std::map<std::string, std::shared_ptr<AgenteBase>> prototipos;
    
public:
    FactoriaAgentes();
    
    /**
     * @brief Crea una nueva persona con parámetros específicos
     */
    std::shared_ptr<Persona> crearPersona(Posicion pos, int edad, bool movilidadReducida = false);
    
    /**
     * @brief Crea un nuevo rescatista
     */
    std::shared_ptr<Rescatista> crearRescatista(Posicion pos);
    
    /**
     * @brief Registra un prototipo para clonación posterior
     */
    void registrarPrototipo(const std::string& nombre, std::shared_ptr<AgenteBase> prototipo);
    
    /**
     * @brief Clona un agente a partir de un prototipo registrado
     */
    std::shared_ptr<AgenteBase> clonarDesdePrototipo(const std::string& nombre, Posicion nuevaPos);
    
    /**
     * @brief Crea múltiples personas con características similares
     */
    std::vector<std::shared_ptr<Persona>> crearGrupoPersonas(
        int cantidad, 
        Posicion posInicial, 
        int edadPromedio, 
        double radioDispersion
    );
    
    /**
     * @brief Obtiene el siguiente ID disponible
     */
    static int obtenerSiguienteId();
};

#endif // FACTORIAAGENTES_H
