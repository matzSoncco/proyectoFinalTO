# Diagrama de Clases - Módulo de Agentes

## Jerarquía de Clases

```
┌─────────────────────────────────────────────────────┐
│              AgenteBase (abstract)                  │
│─────────────────────────────────────────────────────│
│ # id: int                                           │
│ # posicion: Posicion                                │
│ # destino: Posicion                                 │
│ # velocidad: double                                 │
│ # estado: EstadoAgente                              │
│ # tipoComportamiento: TipoComportamiento            │
│ # ruta: vector<Posicion>                            │
│ # indiceRutaActual: int                             │
│ # observadores: vector<weak_ptr<ObservadorEvento>>  │
│─────────────────────────────────────────────────────│
│ + actualizar(deltaTime): void = 0 (virtual puro)    │
│ + calcularVelocidadEfectiva(): double = 0           │
│ + reaccionarObstaculo(): void = 0                   │
│ + clonar(): shared_ptr<AgenteBase> = 0              │
│ + establecerRuta(ruta): void                        │
│ + moverSiguientePunto(deltaTime): void              │
│ + haLlegadoDestino(): bool                          │
│ + agregarObservador(obs): void                      │
│ + notificarEvento(evento): void                     │
│ + getters y setters...                              │
└─────────────────────────────────────────────────────┘
                        △
                        │
        ┌───────────────┴───────────────┐
        │                               │
┌───────────────────┐          ┌────────────────────┐
│     Persona       │          │    Rescatista      │
│───────────────────│          │────────────────────│
│ - edad: int       │          │ - personaAsistida: │
│ - nivelPanico:    │          │   weak_ptr<Persona>│
│   double          │          │ - estaAsistiendo:  │
│ - movilidadRed.:  │          │   bool             │
│   bool            │          │ - capacidadCarga:  │
│ - factorVel.:     │          │   double           │
│   double          │          │────────────────────│
│───────────────────│          │ + actualizar()     │
│ + actualizar()    │          │ + calcularVeloc.() │
│ + calcularVeloc.()│          │ + reaccionarObst.()│
│ + reaccionarObst()│          │ + clonar()         │
│ + clonar()        │          │ + asistirPersona() │
│ + incrementarPan()│          │ + liberarPersona() │
│ + reducirPanico() │          │ + buscarPersonas() │
│ + estEnPanico()   │          └────────────────────┘
└───────────────────┘
```

## Patrón Observer

```
┌──────────────────────────────┐
│  ObservadorEvento (interface) │
│──────────────────────────────│
│ + onEventoAgente(agente,     │
│   tipoEvento): void = 0      │
└──────────────────────────────┘
                △
                │
        ┌───────┴────────┐
        │                │
┌───────────────┐  ┌─────────────┐
│ GestorEventos │  │ Otros...    │
│───────────────│  │ (GUI, logs) │
│ - totalEvac.  │  └─────────────┘
│ - totalBloq.  │
│ - totalPanic. │
│───────────────│
│ + onEvento()  │
│ + reiniciar() │
│ + genReport() │
└───────────────┘

     observa
        ↓
┌──────────────┐
│  AgenteBase  │
│  (Subject)   │
└──────────────┘
```

## Patrón Factory + Prototype

```
┌─────────────────────────────────┐
│      FactoriaAgentes            │
│─────────────────────────────────│
│ - contadorId: static int        │
│ - prototipos: map<string,       │
│   shared_ptr<AgenteBase>>       │
│─────────────────────────────────│
│ + crearPersona(pos, edad, ...): │
│   shared_ptr<Persona>           │
│ + crearRescatista(pos):         │
│   shared_ptr<Rescatista>        │
│ + registrarPrototipo(nombre,    │
│   prototipo): void              │
│ + clonarDesdePrototipo(nombre,  │
│   pos): shared_ptr<AgenteBase>  │
│ + crearGrupoPersonas(...):      │
│   vector<shared_ptr<Persona>>   │
└─────────────────────────────────┘
          │
          │ crea/clona
          ↓
    ┌──────────┐
    │ Agentes  │
    └──────────┘
```

## Enumeraciones

```
┌────────────────────┐
│   EstadoAgente     │
├────────────────────┤
│ NORMAL             │
│ EVACUANDO          │
│ PANICO             │
│ EVACUADO           │
│ BLOQUEADO          │
└────────────────────┘

┌────────────────────┐
│ TipoComportamiento │
├────────────────────┤
│ NORMAL             │
│ PANICO             │
│ MOVILIDAD_REDUCIDA │
│ RESCATISTA         │
└────────────────────┘
```

## Estructuras Auxiliares

```
┌─────────────────┐
│    Posicion     │
├─────────────────┤
│ + x: double     │
│ + y: double     │
├─────────────────┤
│ + distancia(    │
│   otra): double │
└─────────────────┘
```

## Diagrama de Secuencia: Evacuación

```
Usuario    FactoriaAgentes    Persona    GestorEventos    Simulador
  │              │              │              │              │
  │─crear()─────>│              │              │              │
  │              │─new()───────>│              │              │
  │              │<─────────────│              │              │
  │<─────────────│              │              │              │
  │                             │              │              │
  │─agregarObservador()────────>│              │              │
  │                             │──register───>│              │
  │                             │              │              │
  │──────────establecerRuta()──>│              │              │
  │                             │──notify()───>│              │
  │                             │(INICIO_EVAC) │              │
  │                             │              │              │
                                              ┌─┴──┐
                                              │Loop│
                                              └─┬──┘
  │                             │              │              │
  │───────────────────────────────────────actualizar()───────>│
  │                             │              │              │
  │                             │<─actualizar()───────────────│
  │                             │              │              │
  │                             │──notify()───>│              │
  │                             │(EVACUANDO)   │              │
  │                             │              │              │
  │                             │──mover()─────┤              │
  │                             │              │              │
  │                             │──notify()───>│              │
  │                             │(EVACUADO)    │              │
                                              ┌─┴──┐
                                              │End │
                                              └────┘
```

## Diagrama de Secuencia: Rescate

```
Simulador   Rescatista   Persona   GestorEventos
    │            │          │            │
    │─actualizar()────>│          │            │
    │            │          │            │
    │            │─buscarPersonasQueNecesitanAyuda()
    │            │          │            │
    │            │─asistirPersona()>│            │
    │            │          │            │
    │            │──notify()────────────>│
    │            │  (ASISTIENDO_PERSONA) │
    │            │          │            │
    │            │─mover()──┤            │
    │            │          │            │
    │            │─actualizar posición de persona asistida
    │            │          │            │
    │            │─liberarPersona()>│            │
    │            │          │            │
    │            │──notify()────────────>│
    │            │  (PERSONA_LIBERADA)   │
```

## Relaciones entre Clases

```
AgenteBase <|──── Persona (Herencia)
AgenteBase <|──── Rescatista (Herencia)

AgenteBase *──── Posicion (Composición)
AgenteBase o──── ObservadorEvento (Agregación)

Rescatista o──── Persona (Asociación débil - weak_ptr)

FactoriaAgentes ──> AgenteBase (Dependencia - crea)
FactoriaAgentes ──> Persona (Dependencia - crea)
FactoriaAgentes ──> Rescatista (Dependencia - crea)

GestorEventos ──|> ObservadorEvento (Realización)

ObservadorEvento <──── AgenteBase (Uso)
```

## Diagrama de Estados: Persona

```
     ┌─────────┐
     │ NORMAL  │
     └────┬────┘
          │
          │ establecerRuta()
          ↓
   ┌──────────────┐
   │  EVACUANDO   │
   └──┬───────┬───┘
      │       │
      │       │ nivel de pánico > 0.7
      │       ↓
      │    ┌──────┐
      │    │PÁNICO│
      │    └──┬───┘
      │       │
      │       │ bloqueado
      │       ↓
      │  ┌───────────┐
      │  │BLOQUEADO  │
      │  └───────────┘
      │
      │ llega a salida
      ↓
  ┌─────────┐
  │EVACUADO │ (estado final)
  └─────────┘
```

## Diagrama de Componentes

```
┌────────────────────────────────────────────┐
│         Módulo de Agentes (POO)            │
│                                            │
│  ┌──────────┐  ┌──────────┐  ┌──────────┐│
│  │AgenteBase│  │ Observer │  │ Factory  ││
│  │(abstract)│  │ Pattern  │  │ Pattern  ││
│  └────┬─────┘  └──────────┘  └──────────┘│
│       │                                    │
│  ┌────┴─────────┐                         │
│  │              │                          │
│┌─▼────┐  ┌──────▼─┐                       │
││Persona│  │Rescatis│                       │
││       │  │ta      │                       │
│└───────┘  └────────┘                       │
└────────────────────────────────────────────┘
         │                  │
         │ integra con      │
         ↓                  ↓
┌──────────────┐    ┌──────────────┐
│Motor de      │    │GUI (Qt)      │
│Simulación    │    │              │
│(Integrante 1)│    │(Integrante 2)│
└──────────────┘    └──────────────┘
```

---

**Notas**:
- △ = Herencia
- ◇ = Agregación
- ◆ = Composición
- ─> = Dependencia/Uso
- ──|> = Realización (implementa interfaz)
