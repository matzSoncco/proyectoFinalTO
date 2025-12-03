# 🧪 Guía de Pruebas del Simulador

## 🎯 Cómo Probar el Sistema de Estadísticas

### ✅ Escenario 1: Evacuación Exitosa (Todo funciona)

```cpp
// Crear escenario simple con salida
Simulador sim;
sim.cargarEscenario(20, 20);
Escenario* esc = sim.getEscenario();

// Colocar una salida
esc->setCelda(10, 0, 2);  // Salida en la parte superior

// Agregar personas sin obstáculos
for (int i = 0; i < 10; i++) {
    auto persona = std::make_shared<Persona>(i, QPoint(8 + i, 15), 30, false);
    sim.agregarAgente(persona);
}

sim.iniciar();
// Resultado esperado: Todas las personas evacúan correctamente
```

**Estadísticas esperadas:**
- ✅ Evacuados: 10/10 (100%)
- ✅ Tiempo promedio: Variable según distancia
- ✅ Colisiones: Mínimas o ninguna

---

### ❌ Escenario 2: Agentes Atrapados (Tu caso actual)

```cpp
// Crear escenario
Simulador sim;
sim.cargarEscenario(20, 20);
Escenario* esc = sim.getEscenario();

// NO colocar salidas (o colocar paredes alrededor)
// Crear una prisión de paredes
for (int i = 5; i < 15; i++) {
    esc->setCelda(i, 5, 1);   // Pared arriba
    esc->setCelda(i, 15, 1);  // Pared abajo
    esc->setCelda(5, i, 1);   // Pared izquierda
    esc->setCelda(15, i, 1);  // Pared derecha
}

// Agregar personas DENTRO de la prisión
auto p1 = std::make_shared<Persona>(0, QPoint(10, 10), 30, false);
auto p2 = std::make_shared<Persona>(1, QPoint(11, 10), 30, false);
sim.agregarAgente(p1);
sim.agregarAgente(p2);

sim.iniciar();
// Resultado: Detectará estancamiento después de 10 ticks
```

**Estadísticas esperadas:**
- ❌ Evacuados: 0/2 (0%)
- ⚠️ Advertencia: "Simulación estancada. Agentes bloqueados o sin salida"
- 💡 Sugerencias de diagnóstico incluidas

---

### 🔄 Escenario 3: Cuello de Botella

```cpp
// Crear escenario
Simulador sim;
sim.cargarEscenario(30, 40);
Escenario* esc = sim.getEscenario();

// Crear pasillo estrecho (1 celda de ancho)
for (int i = 0; i < 30; i++) {
    if (i != 15) {  // Dejar solo UN espacio
        esc->setCelda(i, 20, 1);
    }
}

// Salida al otro lado
esc->setCelda(15, 0, 2);

// Muchos agentes que deben pasar por el pasillo
for (int i = 0; i < 30; i++) {
    auto persona = std::make_shared<Persona>(i, QPoint(5 + (i%10), 25 + (i/10)), 30, false);
    sim.agregarAgente(persona);
}

sim.iniciar();
// Resultado: Muchas colisiones, cuello de botella detectado
```

**Estadísticas esperadas:**
- ✅ Evacuados: 30/30 (eventualmente)
- ⚠️ Colisiones: ALTAS (50+)
- ⚠️ Cuellos de botella detectados: Posición (15, 20)
- 📊 Tiempo muy alto debido a congestión

---

### 🚶 Escenario 4: Comparar Movilidad

```cpp
// Grupo A: Personas jóvenes sin problemas
for (int i = 0; i < 10; i++) {
    auto persona = std::make_shared<Persona>(i, QPoint(5, 15+i), 25, false);
    sim.agregarAgente(persona);
}

// Grupo B: Personas con movilidad reducida
for (int i = 10; i < 20; i++) {
    auto persona = std::make_shared<Persona>(i, QPoint(15, 15+(i-10)), 70, true);
    sim.agregarAgente(persona);
}

sim.iniciar();
// Resultado: Diferencias claras en tiempos de evacuación
```

**Estadísticas esperadas:**
- ✅ Tiempo mínimo: Grupo A (más rápido)
- ✅ Tiempo máximo: Grupo B (más lento)
- 📊 Personas con movilidad reducida: 10

---

## 🔍 Verificación de Estadísticas

### Comprobar que el sistema funciona:

1. **Ejecuta la simulación**
2. **Espera a que termine** (automáticamente o por estancamiento)
3. **Verifica la salida en consola**

### ¿Qué deberías ver?

#### ✅ Si hay evacuaciones:
```
═══════════════════════════════════════════════
  REPORTE DE ESTADÍSTICAS
═══════════════════════════════════════════════

━━━ INFORMACIÓN GENERAL ━━━
Total de agentes:            10
Agentes evacuados:           10 (100%)
...

━━━ MÉTRICAS TEMPORALES ━━━
Tiempo total simulación:     45.5s
Tiempo promedio evacuación:  22.3s
...
```

#### ⚠️ Si NO hay evacuaciones (tu caso):
```
═══════════════════════════════════════════════
  REPORTE DE ESTADÍSTICAS
═══════════════════════════════════════════════

━━━ INFORMACIÓN GENERAL ━━━
Total de agentes:            2
Agentes evacuados:           0 (0%)

⚠️  ADVERTENCIA: Ningún agente pudo evacuar
    Posibles causas:
    - No hay salidas en el escenario
    - Los agentes están rodeados por paredes
    - Las rutas hacia las salidas están bloqueadas

━━━ MÉTRICAS TEMPORALES ━━━
Tiempo total simulación:     5.0s
(No se registraron evacuaciones completadas)

━━━ DISTRIBUCIÓN POR SALIDA ━━━
(No se registraron evacuaciones por ninguna salida)

━━━ COMPORTAMIENTO DE AGENTES ━━━
Personas con pánico:         0
Personas movilidad reducida: 0

━━━ DIAGNÓSTICO ━━━
💡 SUGERENCIAS:
   1. Verifica que haya al menos una salida
   2. Asegúrate de que los agentes no estén rodeados
   3. Comprueba que exista un camino válido
```

---

## 🛠️ Soluciones a Problemas Comunes

### Problema: "Evacuados: 0/X"

**Causa:** Los agentes no tienen ruta hacia una salida

**Solución:**
```cpp
// 1. Asegúrate de colocar al menos UNA salida
esc->setCelda(10, 0, 2);  // o cualquier posición en el borde

// 2. Verifica que los agentes no estén rodeados
// NO hagas esto:
for (int i = 0; i < 20; i++) {
    for (int j = 0; j < 20; j++) {
        esc->setCelda(i, j, 1);  // ¡Todo lleno de paredes!
    }
}

// 3. Asegúrate de que hay un camino
// Deja espacios entre las paredes
```

### Problema: "Simulación se detiene inmediatamente"

**Causa:** Detección de estancamiento (10 ticks sin movimiento)

**Solución:**
- Verifica que el PathFinder esté funcionando
- Asegúrate de que `getSalidaMasCercana()` devuelva una salida válida
- Comprueba que `calcularSiguientePaso()` calcule rutas correctas

### Problema: "Todas las estadísticas en 0"

**Causa:** El simulador no se ejecutó o se detuvo antes de tiempo

**Solución:**
```cpp
// Asegúrate de iniciar correctamente
sim.iniciar();

// Y dejar que corra (en GUI esto es automático con QTimer)
// En tests manuales, necesitas un loop o event loop de Qt
```

---

## 📋 Checklist de Verificación

Antes de probar, asegúrate de:

- [ ] El escenario tiene al menos una salida (tipo 2)
- [ ] Los agentes están en posiciones válidas (no dentro de paredes)
- [ ] Hay un camino despejado entre agentes y salida
- [ ] El PathFinder está implementado correctamente
- [ ] El QTimer está corriendo (en GUI)
- [ ] La simulación se inició con `sim.iniciar()`

---

## 🎓 Casos de Prueba para el Informe

### Test 1: Evacuación Básica
- **Objetivo:** Verificar funcionamiento básico
- **Setup:** 10 agentes, 1 salida, sin obstáculos
- **Resultado esperado:** 100% evacuación, tiempo < 1 minuto

### Test 2: Múltiples Salidas
- **Objetivo:** Verificar distribución por salidas
- **Setup:** 30 agentes, 3 salidas equidistantes
- **Resultado esperado:** Distribución ~33% por salida

### Test 3: Obstáculos
- **Objetivo:** Verificar manejo de obstáculos
- **Setup:** Paredes que obligan a rodear
- **Resultado esperado:** Rutas más largas, más tiempo

### Test 4: Movilidad Reducida
- **Objetivo:** Verificar diferencias de velocidad
- **Setup:** 50% normales, 50% movilidad reducida
- **Resultado esperado:** Tiempo máximo >> tiempo mínimo

### Test 5: Cuello de Botella
- **Objetivo:** Detectar congestión
- **Setup:** Pasillo de 1 celda, 40 agentes
- **Resultado esperado:** Colisiones > 30, cuello detectado

---

## 💡 Tips Finales

1. **Empieza simple:** Primero prueba con 1 agente y 1 salida
2. **Incrementa gradualmente:** Añade complejidad paso a paso
3. **Usa la consola:** Los mensajes `qDebug()` te ayudarán
4. **Exporta CSV:** Más fácil analizar en Excel
5. **Compara escenarios:** Ejecuta múltiples pruebas y compara

---

Universidad Nacional de San Agustín de Arequipa  
Tópicos en Objetos - 2025
