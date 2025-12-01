# Simulador de Evacuación de Emergencias

## Descripción del Proyecto
Este proyecto consiste en el desarrollo de un simulador de evacuación basado en agentes, implementado en C++ utilizando el framework Qt. El sistema modela un entorno de edificio mediante una cuadrícula (grid) donde múltiples entidades autónomas (agentes) deben navegar en tiempo real hacia las salidas de emergencia designadas, evitando obstáculos y optimizando su ruta.

El propósito fundamental del proyecto es doble:
1.  **Propósito Académico:** Demostrar la implementación práctica de conceptos avanzados de Programación Orientada a Objetos (POO), gestión de memoria y la aplicación de patrones de diseño de software.
2.  **Propósito Funcional:** Proveer una herramienta visual para analizar el comportamiento de masas y rutas de evacuación en escenarios de riesgo simulados.

## Arquitectura del Sistema
El software sigue una **Arquitectura Monolítica de Escritorio** estructurada bajo una variante del patrón **Modelo-Vista-Controlador (MVC)**, permitiendo un desacoplamiento efectivo entre la lógica de simulación y la interfaz de usuario.

### 1. Modelo (Lógica del Dominio)
Encapsula las reglas de negocio, la física del movimiento y el estado del sistema. Es independiente de la interfaz gráfica.
* **Escenario:** Representa el entorno físico mediante una matriz de estados (celdas vacías, obstáculos, salidas).
* **Agentes (AgenteBase):** Entidades polimórficas con capacidad de movimiento y toma de decisiones.
* **PathFinder:** Módulo de lógica algorítmica encargado del cálculo de rutas.

### 2. Vista (Interfaz de Usuario)
Implementada con **Qt Widgets**. Su responsabilidad se limita a la representación visual del estado actual del modelo y la captura de eventos de entrada del usuario (configuración de paredes, inicio de simulación).

### 3. Controlador (Gestor de Simulación)
La clase `Simulador` actúa como el orquestador central. Gestiona el ciclo de vida de la simulación mediante un bucle de tiempo (Game Loop) controlado por un `QTimer`. En cada ciclo de actualización, coordina la interacción entre el mapa, los agentes y la actualización de la vista.

## Detalles Técnicos y Algoritmos

### Algoritmo de Navegación (Pathfinding)
Para la resolución de rutas, se ha implementado el algoritmo de **Búsqueda en Anchura (BFS - Breadth-First Search)**.
* **Justificación:** Dado que el entorno se modela como un grafo no ponderado (el costo de movimiento entre celdas adyacentes es constante), BFS garantiza matemáticamente el hallazgo de la ruta más corta posible hacia la salida más cercana con una complejidad computacional adecuada para la ejecución en tiempo real.

### Comportamiento de Agentes (Polimorfismo)
El sistema utiliza herencia y polimorfismo para diferenciar el comportamiento de los agentes:
* **Persona:** Agente reactivo. Su velocidad y eficiencia de evacuación se ven afectadas dinámicamente por atributos como edad, movilidad reducida y nivel de pánico. Su objetivo principal es la supervivencia individual.
* **Rescatista:** Agente proactivo. Posee capacidades extendidas como `buscarPersonas()` y `asistirPersona()`. Su velocidad varía dependiendo de su capacidad de carga y si está transportando a un herido. Prioriza la asistencia a terceros sobre su propia evacuación inmediata.

### Patrones de Diseño Aplicados
El desarrollo se ha guiado por los siguientes patrones para asegurar extensibilidad y mantenibilidad:

* **Observer (Observador):** Implementado mediante el sistema de Señales y Slots de Qt y la clase `ObservadorEvento`. Permite que la interfaz gráfica reaccione asíncronamente a los cambios en el modelo sin generar dependencias circulares.
* **Strategy (Estrategia):** La lógica de búsqueda de caminos está encapsulada en la clase `PathFinder`. Esto permite modificar o sustituir el algoritmo de navegación sin alterar la lógica de los agentes.
* **Factory Method (Factoría):** La clase `FactoriaAgentes` centraliza la instanciación de objetos, permitiendo la creación dinámica de diferentes tipos de agentes basada en parámetros de configuración.
* **Prototype (Prototipo):** Implementado a través del método `clonar()`, permite la duplicación eficiente de agentes complejos para poblar escenarios masivos rápidamente.

## Estructura del Proyecto

La organización de directorios sigue el estándar de proyectos C++/CMake:

* **src/**: Contiene la implementación de las clases (`.cpp`).
    * `Simulador.cpp`: Lógica del bucle principal.
    * `PathFinder.cpp`: Implementación del algoritmo BFS.
    * `Escenario.cpp`: Gestión de la matriz del entorno.
    * `AgenteBase.cpp`, `Persona.cpp`, `Rescatista.cpp`: Jerarquía de agentes.
* **include/**: Contiene los archivos de cabecera (`.h`) definiendo las interfaces y estructuras de datos.
* **docs/**: Documentación técnica y diagramas UML.
* **CMakeLists.txt**: Script de configuración para el sistema de construcción CMake.

## Requisitos e Instalación

### Dependencias
* Compilador C++ compatible con el estándar C++17.
* Qt 6 (Componentes: Core, Gui, Widgets).
* CMake 3.5 o superior.

### Instrucciones de Compilación
1.  Crear un directorio de construcción:
    ```bash
    mkdir build
    cd build
    ```
2.  Generar los archivos de compilación con CMake:
    ```bash
    cmake ..
    ```
3.  Compilar el proyecto:
    ```bash
    make
    # En Windows con NMake o JOM: nmake
    ```
4.  Ejecutar la aplicación:
    ```bash
    ./SimuladorEmergencias
    ```

## Guía de Uso e Interacción

El sistema permite la edición rápida de escenarios mediante interacción directa con el ratón sobre la cuadrícula:

1.  **Edición del Entorno:** Antes de iniciar la simulación, el usuario puede hacer clic izquierdo sobre cualquier celda para alternar su estado cíclicamente:
    * **Piso (Blanco):** Zona transitable.
    * **Pared (Gris/Negro):** Obstáculo intransitable.
    * **Salida (Verde):** Punto objetivo de evacuación.
2.  **Configuración de Agentes:** A través del panel de control, se puede definir la cantidad de personas y rescatistas a instanciar.
3.  **Control de Simulación:** Botones para Iniciar, Pausar y Reiniciar la simulación visual.