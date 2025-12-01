cmake_minimum_required(VERSION 3.16)
project(SimuladorEvacuacion VERSION 1.0 LANGUAGES CXX)

# Especificar el estándar de C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

# Directorios de include
include_directories(${PROJECT_SOURCE_DIR}/include)

# Archivos fuente
set(SOURCES
    src/AgenteBase.cpp
    src/Persona.cpp
    src/Rescatista.cpp
    src/ObservadorEvento.cpp
    src/FactoriaAgentes.cpp
    src/main.cpp
    src/Escenario.cpp
    src/Simulador.cpp
    src/PathFinder.cpp
)

# Crear ejecutable
add_executable(simulador_agentes WIN32 ${SOURCES})

target_link_libraries(simulador_agentes PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets)

# Opciones de compilación
if(MSVC)
    target_compile_options(simulador_agentes PRIVATE /W4)
else()
    target_compile_options(simulador_agentes PRIVATE -Wall -Wextra -pedantic)
endif()

# Para debugging
set(CMAKE_BUILD_TYPE Debug)
