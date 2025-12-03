#!/bin/bash

echo "🔧 Compilando proyecto..."
echo ""

cd /home/gocardi/proyectoFinalTO/build

# Limpiar y compilar
make clean > /dev/null 2>&1
cmake .. > cmake_output.txt 2>&1
make -j$(nproc) > make_output.txt 2>&1

# Verificar resultado
if [ $? -eq 0 ]; then
    echo "✅ COMPILACIÓN EXITOSA"
    echo ""
    echo "Ejecutable generado: build/simulador_agentes"
    echo ""
    echo "Para ejecutar:"
    echo "  cd build && ./simulador_agentes"
    echo ""
else
    echo "❌ ERROR EN LA COMPILACIÓN"
    echo ""
    echo "Errores encontrados:"
    echo "===================="
    grep -i "error:" make_output.txt | head -20
    echo ""
    echo "Archivo completo de errores guardado en: build/make_output.txt"
fi
