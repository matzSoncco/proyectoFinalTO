# 📊 Guía de Análisis de Estadísticas - Simulador de Evacuación

## 📌 Descripción General

El sistema de estadísticas del simulador recopila automáticamente datos en tiempo real durante la simulación y genera reportes detallados al finalizar. Esto permite analizar la eficiencia de la evacuación y detectar problemas en el diseño del escenario.

---

## 🎯 Métricas Recopiladas

### 1. **Métricas Temporales**
| Métrica | Descripción | Utilidad |
|---------|-------------|----------|
| **Tiempo Total** | Duración completa de la simulación | Evaluar eficiencia global |
| **Tiempo Promedio** | Tiempo medio de evacuación por agente | Comparar escenarios |
| **Tiempo Mínimo** | Agente más rápido en evacuar | Identificar ruta óptima |
| **Tiempo Máximo** | Agente más lento en evacuar | Detectar problemas |
| **Tasa de Evacuación** | Personas evacuadas por segundo | Medir flujo de evacuación |

### 2. **Distribución por Salida**
- Cantidad de personas que usaron cada salida
- Tiempo promedio de evacuación por salida
- Identificación de salidas sobrecargadas o subutilizadas

### 3. **Comportamiento de Agentes**
- Personas con pánico durante la evacuación
- Personas con movilidad reducida
- Personas rescatadas por rescatistas
- Cambios de comportamiento durante la simulación

### 4. **Métricas de Eficiencia**
- **Distancia promedio recorrida**: Detecta rutas ineficientes
- **Colisiones totales**: Indica congestión
- **Densidad promedio**: Relación agentes/espacio
- **Cuellos de botella**: Ubicaciones problemáticas

---

## 💻 Uso Programático

### Acceder a las Estadísticas desde el Código

```cpp
// En tu código de simulación
Simulador* sim = new Simulador();

// ... configurar escenario y agentes ...

sim->iniciar();

// Después de la simulación
EstadisticasSimulacion* stats = sim->getEstadisticas();

// Obtener datos específicos
EstadisticasGlobales globales = stats->getEstadisticas();
std::cout << "Total evacuados: " << globales.totalEvacuados << std::endl;
std::cout << "Tiempo total: " << globales.tiempoTotalSimulacion << "s" << std::endl;

// Consultas específicas
int evacuadosSalida1 = stats->getPersonasEvacuadasPorSalida(QPoint(10, 0));
double tiempoPromSalida = stats->getTiempoPromedioSalida(QPoint(10, 0));

// Obtener cuellos de botella
std::vector<QPoint> cuellos = stats->getCuellosBotellaDetectados();
for (const auto& punto : cuellos) {
    std::cout << "Cuello de botella en: (" << punto.x() << ", " << punto.y() << ")" << std::endl;
}
```

### Mostrar Estadísticas en Consola

```cpp
// Al finalizar la simulación
sim->mostrarEstadisticas();
```

**Salida esperada:**
```
═══════════════════════════════════════════════════════════════
        REPORTE DE ESTADÍSTICAS - SIMULACIÓN DE EVACUACIÓN     
═══════════════════════════════════════════════════════════════

━━━ INFORMACIÓN GENERAL ━━━
Total de agentes:            50
Agentes evacuados:           50 (100%)
  - Personas:                45
  - Rescatistas:             5
Agentes en proceso:          0

━━━ MÉTRICAS TEMPORALES ━━━
Tiempo total simulación:     2m 35.50s
Tiempo promedio evacuación:  1m 15.20s
Tiempo mínimo evacuación:    45.30s
Tiempo máximo evacuación:    2m 30.10s
Tasa de evacuación:          0.32 personas/segundo

━━━ DISTRIBUCIÓN POR SALIDA ━━━
Salida 10,0: 25 personas (tiempo promedio: 1m 12.5s)
Salida 30,0: 20 personas (tiempo promedio: 1m 18.3s)
Salida 20,40: 5 personas (tiempo promedio: 2m 5.1s)

━━━ COMPORTAMIENTO DE AGENTES ━━━
Personas con pánico:         12
Personas movilidad reducida: 8
Personas rescatadas:         3

━━━ MÉTRICAS DE EFICIENCIA ━━━
Distancia promedio recorrida: 28.45 celdas
Colisiones totales:          67
Densidad promedio:           0.0312 agentes/celda
Cuellos de botella:          3 detectados
  Ubicaciones: (15,10), (25,15), (18,22)

═══════════════════════════════════════════════════════════════
```

---

## 📁 Exportar Estadísticas

### Formato TXT (Reporte Completo)
```cpp
sim->exportarEstadisticas("resultados/evacuacion_001.txt");
```

### Formato CSV (Para Excel/hojas de cálculo)
```cpp
EstadisticasSimulacion* stats = sim->getEstadisticas();
stats->exportarReporte("resultados/evacuacion_001.csv");
```

**Ejemplo de CSV generado:**
```csv
Métrica,Valor
Total Agentes,50
Total Evacuados,50
Personas Evacuadas,45
Rescatistas Evacuados,5
Tiempo Total (s),155.50
Tiempo Promedio Evacuación (s),75.20
Tasa Evacuación (p/s),0.32

Salida,Personas,Tiempo Promedio (s)
10,0,25,72.5
30,0,20,78.3
```

### Formato JSON (Para análisis programático)
```cpp
stats->exportarReporte("resultados/evacuacion_001.json");
```

**Ejemplo de JSON generado:**
```json
{
  "simulacion": {
    "totalAgentes": 50,
    "totalEvacuados": 50,
    "personasEvacuadas": 45,
    "rescatistasEvacuados": 5
  },
  "tiempos": {
    "total": 155.50,
    "promedioEvacuacion": 75.20,
    "minimo": 45.30,
    "maximo": 150.10,
    "tasaEvacuacion": 0.32
  },
  "salidas": [
    {
      "ubicacion": "10,0",
      "personas": 25,
      "tiempoPromedio": 72.5
    }
  ]
}
```

---

## 📈 Ejemplos de Análisis

### Análisis 1: Comparación de Escenarios

```cpp
// Ejecutar múltiples simulaciones con diferentes configuraciones
void compararEscenarios() {
    std::vector<std::string> resultados;
    
    // Escenario 1: Con 2 salidas
    Simulador* sim1 = crearEscenario(2);
    sim1->iniciar();
    // ... esperar a que termine ...
    sim1->exportarEstadisticas("resultados/escenario_2salidas.csv");
    
    // Escenario 2: Con 3 salidas
    Simulador* sim2 = crearEscenario(3);
    sim2->iniciar();
    // ... esperar a que termine ...
    sim2->exportarEstadisticas("resultados/escenario_3salidas.csv");
    
    // Comparar los CSV en Excel o Python
}
```

### Análisis 2: Identificar Rutas Óptimas

```cpp
void analizarRutas() {
    EstadisticasSimulacion* stats = simulador->getEstadisticas();
    
    // Obtener eventos de evacuación
    std::vector<EventoEvacuacion> eventos = stats->getEventosEvacuacion();
    
    // Encontrar los 5 agentes más rápidos
    std::sort(eventos.begin(), eventos.end(), 
              [](const EventoEvacuacion& a, const EventoEvacuacion& b) {
                  return a.tiempoEvacuacion < b.tiempoEvacuacion;
              });
    
    std::cout << "Top 5 evacuaciones más rápidas:\n";
    for (int i = 0; i < 5 && i < eventos.size(); i++) {
        std::cout << "Agente " << eventos[i].agenteId 
                  << " - Tiempo: " << eventos[i].tiempoEvacuacion << "s"
                  << " - Salida: (" << eventos[i].salida.x() << "," << eventos[i].salida.y() << ")"
                  << " - Pasos: " << eventos[i].pasosRealizados << "\n";
    }
}
```

### Análisis 3: Detectar Problemas de Diseño

```cpp
void detectarProblemas() {
    EstadisticasSimulacion* stats = simulador->getEstadisticas();
    EstadisticasGlobales globales = stats->getEstadisticas();
    
    std::cout << "ANÁLISIS DE PROBLEMAS:\n\n";
    
    // 1. Cuellos de botella
    if (globales.cuellosBotellaDetectados.size() > 0) {
        std::cout << "⚠️  ADVERTENCIA: " << globales.cuellosBotellaDetectados.size() 
                  << " cuellos de botella detectados\n";
        std::cout << "   Solución: Ampliar pasillos o agregar rutas alternativas\n\n";
    }
    
    // 2. Colisiones excesivas
    double colisionesPorAgente = (double)globales.colisionesTotales / globales.totalAgentes;
    if (colisionesPorAgente > 2.0) {
        std::cout << "⚠️  ADVERTENCIA: Alto número de colisiones (" 
                  << colisionesPorAgente << " por agente)\n";
        std::cout << "   Solución: Mejorar distribución de agentes o agregar salidas\n\n";
    }
    
    // 3. Salidas desbalanceadas
    for (const auto& par : globales.personasPorSalida) {
        double porcentaje = (par.second * 100.0) / globales.totalEvacuados;
        if (porcentaje > 60.0) {
            std::cout << "⚠️  ADVERTENCIA: Salida " << par.first 
                      << " sobrecargada (" << porcentaje << "%)\n";
            std::cout << "   Solución: Mejorar señalización o redistribuir salidas\n\n";
        } else if (porcentaje < 10.0 && globales.personasPorSalida.size() > 2) {
            std::cout << "ℹ️  NOTA: Salida " << par.first 
                      << " subutilizada (" << porcentaje << "%)\n\n";
        }
    }
    
    // 4. Tiempo de evacuación
    if (globales.tiempoMaximoEvacuacion > globales.tiempoPromedioEvacuacion * 2) {
        std::cout << "⚠️  ADVERTENCIA: Variación alta en tiempos de evacuación\n";
        std::cout << "   Algunos agentes tardan más del doble del promedio\n";
        std::cout << "   Solución: Revisar ubicación de agentes iniciales\n\n";
    }
}
```

---

## 📊 Visualización en la GUI

### Integrar Estadísticas en Qt

Puedes conectar las señales del simulador para actualizar la GUI en tiempo real:

```cpp
// En tu VentanaPrincipal.cpp
connect(simulador, &Simulador::estadisticasActualizadas, 
        this, &VentanaPrincipal::actualizarLabelEstadisticas);

void VentanaPrincipal::actualizarLabelEstadisticas(QString resumen) {
    labelEstadisticas->setText(resumen);
    // Ejemplo de resumen: "Evacuados: 15/50 | Tiempo: 45.2s | Tasa: 0.33 p/s | Colisiones: 8"
}

void VentanaPrincipal::mostrarDialogoEstadisticas() {
    EstadisticasSimulacion* stats = simulador->getEstadisticas();
    QString reporte = QString::fromStdString(stats->generarReporte());
    
    QDialog* dialogo = new QDialog(this);
    QTextEdit* texto = new QTextEdit(reporte, dialogo);
    texto->setReadOnly(true);
    texto->setFont(QFont("Courier", 10));
    
    QVBoxLayout* layout = new QVBoxLayout(dialogo);
    layout->addWidget(texto);
    
    dialogo->setWindowTitle("Estadísticas de Evacuación");
    dialogo->resize(800, 600);
    dialogo->exec();
}
```

---

## 📝 Checklist para el Integrante 4 (Documentador)

Para el informe técnico, asegúrate de incluir:

- [ ] Tabla con todas las métricas recopiladas y su definición
- [ ] Gráficos de comparación entre diferentes escenarios
- [ ] Análisis de cuellos de botella detectados
- [ ] Recomendaciones basadas en las estadísticas
- [ ] Capturas de pantalla del reporte generado
- [ ] Ejemplos de exportación en CSV/JSON
- [ ] Interpretación de los resultados obtenidos

### Métricas Clave para el Informe

1. **Eficiencia Global**: Tiempo promedio y tasa de evacuación
2. **Distribución**: Porcentaje de uso de cada salida
3. **Problemas**: Cuellos de botella y colisiones
4. **Comportamiento**: Pánico y movilidad reducida

---

## 🔬 Scripts de Análisis Avanzado

### Script Python para Analizar CSV

```python
import pandas as pd
import matplotlib.pyplot as plt

# Leer CSV generado
df = pd.read_csv('evacuacion_001.csv')

# Visualizar tiempos
plt.figure(figsize=(10, 6))
plt.bar(['Mínimo', 'Promedio', 'Máximo'], 
        [df['Tiempo Mínimo (s)'][0], 
         df['Tiempo Promedio Evacuación (s)'][0],
         df['Tiempo Máximo (s)'][0]])
plt.title('Distribución de Tiempos de Evacuación')
plt.ylabel('Tiempo (segundos)')
plt.savefig('analisis_tiempos.png')

# Comparar múltiples escenarios
escenarios = ['2salidas.csv', '3salidas.csv', '4salidas.csv']
tiempos = []
for esc in escenarios:
    df = pd.read_csv(f'resultados/{esc}')
    tiempos.append(df['Tiempo Promedio Evacuación (s)'][0])

plt.figure()
plt.plot([2, 3, 4], tiempos, marker='o')
plt.title('Impacto del Número de Salidas')
plt.xlabel('Número de Salidas')
plt.ylabel('Tiempo Promedio (s)')
plt.savefig('comparacion_salidas.png')
```

---

## 🎓 Conclusiones

El sistema de estadísticas permite:

✅ **Evaluar objetivamente** la eficiencia de diferentes configuraciones  
✅ **Identificar automáticamente** problemas de diseño  
✅ **Comparar cuantitativamente** múltiples escenarios  
✅ **Exportar datos** para análisis externos  
✅ **Generar reportes** profesionales para presentaciones  

**Universidad Nacional de San Agustín de Arequipa**  
Tópicos en Objetos - 2025
