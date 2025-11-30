#!/bin/bash

# Script de compilación para el Simulador de Evacuación
# Autor: Integrante 3 - Desarrollador de Agentes & POO

echo "=========================================="
echo "  COMPILANDO SIMULADOR DE EVACUACIÓN"
echo "=========================================="
echo ""

# Colores para output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Verificar compilador
if ! command -v g++ &> /dev/null; then
    echo -e "${RED}Error: g++ no encontrado${NC}"
    echo "Por favor instala g++ primero"
    exit 1
fi

echo -e "${YELLOW}Compilador encontrado:${NC} $(g++ --version | head -n 1)"
echo ""

# Crear directorio build si no existe
if [ ! -d "build" ]; then
    echo -e "${YELLOW}Creando directorio build...${NC}"
    mkdir -p build
fi

# Opción de compilación
echo "Selecciona el método de compilación:"
echo "1) CMake (recomendado)"
echo "2) g++ directo"
read -p "Opción [1-2]: " opcion

echo ""

if [ "$opcion" = "1" ]; then
    # Compilación con CMake
    echo -e "${YELLOW}Compilando con CMake...${NC}"
    
    if ! command -v cmake &> /dev/null; then
        echo -e "${RED}Error: cmake no encontrado${NC}"
        echo "Usando g++ directo en su lugar..."
        opcion="2"
    else
        cd build
        cmake .. && make
        resultado=$?
        cd ..
        
        if [ $resultado -eq 0 ]; then
            echo ""
            echo -e "${GREEN}✓ Compilación exitosa${NC}"
            echo -e "Ejecutable generado: ${GREEN}build/simulador_agentes${NC}"
            echo ""
            read -p "¿Deseas ejecutar el programa? [s/n]: " ejecutar
            
            if [ "$ejecutar" = "s" ] || [ "$ejecutar" = "S" ]; then
                echo ""
                echo -e "${YELLOW}Ejecutando simulador...${NC}"
                echo "=========================================="
                ./build/simulador_agentes
            fi
        else
            echo -e "${RED}✗ Error en la compilación${NC}"
            exit 1
        fi
    fi
fi

if [ "$opcion" = "2" ]; then
    # Compilación directa con g++
    echo -e "${YELLOW}Compilando con g++ directo...${NC}"
    
    g++ -std=c++17 -Wall -Wextra -I./include \
        src/AgenteBase.cpp \
        src/Persona.cpp \
        src/Rescatista.cpp \
        src/ObservadorEvento.cpp \
        src/FactoriaAgentes.cpp \
        src/main.cpp \
        -o build/simulador_agentes
    
    resultado=$?
    
    if [ $resultado -eq 0 ]; then
        echo ""
        echo -e "${GREEN}✓ Compilación exitosa${NC}"
        echo -e "Ejecutable generado: ${GREEN}build/simulador_agentes${NC}"
        echo ""
        read -p "¿Deseas ejecutar el programa? [s/n]: " ejecutar
        
        if [ "$ejecutar" = "s" ] || [ "$ejecutar" = "S" ]; then
            echo ""
            echo -e "${YELLOW}Ejecutando simulador...${NC}"
            echo "=========================================="
            ./build/simulador_agentes
        fi
    else
        echo -e "${RED}✗ Error en la compilación${NC}"
        exit 1
    fi
fi

echo ""
echo "=========================================="
echo "  PROCESO COMPLETADO"
echo "=========================================="
