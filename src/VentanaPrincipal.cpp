#include "../include/VentanaPrincipal.h"
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QTimer>
#include <QTextEdit>
#include <QDialog>
#include <QFont>
#include <QDebug>

VentanaPrincipal::VentanaPrincipal(QWidget *parent)
    : QMainWindow(parent),
    simulacionEnEjecucion(false),
    tiempoSimulacion(0) {

    // Configurar ventana
    setWindowTitle("Simulador de Evacuación - Sistema Multiagente");
    setMinimumSize(1200, 800);

    // Crear componentes principales
    simulador = new Simulador(this);
    factoria = new FactoriaAgentes();
    vistaEscenario = new VistaEscenario(this);
    vistaEscenario->setFactoria(factoria);

    // Crear interfaz
    crearMenus();
    crearBarraHerramientas();
    crearPanelControl();
    crearPanelEstadisticas();
    configurarLayout();
    conectarSeñales();

    // Inicializar con un escenario por defecto
    simulador->cargarEscenario(20, 20);
    vistaEscenario->setEscenario(simulador->getEscenario());
    vistaEscenario->setAgentes(&simulador->getAgentes());

    statusBar()->showMessage("Listo. Dibuja el escenario o carga una configuración.");
}

VentanaPrincipal::~VentanaPrincipal() {
    delete factoria;
}

void VentanaPrincipal::crearMenus() {
    // Menú Archivo
    menuArchivo = menuBar()->addMenu("&Archivo");

    QAction* accionNuevo = menuArchivo->addAction("&Nuevo Escenario");
    accionNuevo->setShortcut(QKeySequence::New);
    connect(accionNuevo, &QAction::triggered, this, &VentanaPrincipal::nuevoEscenario);

    QAction* accionAbrir = menuArchivo->addAction("&Abrir Configuración");
    accionAbrir->setShortcut(QKeySequence::Open);
    connect(accionAbrir, &QAction::triggered, this, &VentanaPrincipal::abrirConfiguracion);

    QAction* accionGuardar = menuArchivo->addAction("&Guardar Configuración");
    accionGuardar->setShortcut(QKeySequence::Save);
    connect(accionGuardar, &QAction::triggered, this, &VentanaPrincipal::guardarConfiguracion);

    menuArchivo->addSeparator();

    QAction* accionSalir = menuArchivo->addAction("&Salir");
    accionSalir->setShortcut(QKeySequence::Quit);
    connect(accionSalir, &QAction::triggered, this, &VentanaPrincipal::salir);

    // Menú Simulación
    menuSimulacion = menuBar()->addMenu("&Simulación");

    QAction* accionIniciar = menuSimulacion->addAction("&Iniciar");
    accionIniciar->setShortcut(Qt::Key_F5);
    connect(accionIniciar, &QAction::triggered, this, &VentanaPrincipal::iniciarSimulacion);

    QAction* accionPausar = menuSimulacion->addAction("&Pausar");
    accionPausar->setShortcut(Qt::Key_F6);
    connect(accionPausar, &QAction::triggered, this, &VentanaPrincipal::pausarSimulacion);

    QAction* accionReiniciar = menuSimulacion->addAction("&Reiniciar");
    accionReiniciar->setShortcut(Qt::Key_F7);
    connect(accionReiniciar, &QAction::triggered, this, &VentanaPrincipal::reiniciarSimulacion);

    // Menú Ayuda
    menuAyuda = menuBar()->addMenu("&Ayuda");

    QAction* accionAcercaDe = menuAyuda->addAction("&Acerca de");
    connect(accionAcercaDe, &QAction::triggered, this, &VentanaPrincipal::mostrarAcercaDe);
}

void VentanaPrincipal::crearBarraHerramientas() {
    barraHerramientas = addToolBar("Herramientas principales");
    barraHerramientas->setMovable(false);

    // Botones de control rápido
    btnIniciar = new QPushButton("▶ Iniciar", this);
    btnPausar = new QPushButton("⏸ Pausar", this);
    btnReiniciar = new QPushButton("⟲ Reiniciar", this);

    btnPausar->setEnabled(false);

    barraHerramientas->addWidget(btnIniciar);
    barraHerramientas->addWidget(btnPausar);
    barraHerramientas->addWidget(btnReiniciar);

    connect(btnIniciar, &QPushButton::clicked, this, &VentanaPrincipal::iniciarSimulacion);
    connect(btnPausar, &QPushButton::clicked, this, &VentanaPrincipal::pausarSimulacion);
    connect(btnReiniciar, &QPushButton::clicked, this, &VentanaPrincipal::reiniciarSimulacion);
}

void VentanaPrincipal::crearPanelControl() {
    panelControl = new QGroupBox("Control de Simulación", this);
    QVBoxLayout* layout = new QVBoxLayout();

    // Velocidad de simulación
    QLabel* lblTitulo = new QLabel("Velocidad:", this);
    sliderVelocidad = new QSlider(Qt::Horizontal, this);
    sliderVelocidad->setMinimum(1);
    sliderVelocidad->setMaximum(10);
    sliderVelocidad->setValue(5);

    lblVelocidad = new QLabel("Normal (500ms)", this);
    lblVelocidad->setAlignment(Qt::AlignCenter);

    layout->addWidget(lblTitulo);
    layout->addWidget(sliderVelocidad);
    layout->addWidget(lblVelocidad);

    connect(sliderVelocidad, &QSlider::valueChanged,
            this, &VentanaPrincipal::cambiarVelocidadSimulacion);

    panelControl->setLayout(layout);
    panelControl->setMaximumWidth(250);
}

void VentanaPrincipal::crearPanelEstadisticas() {
    panelEstadisticas = new QGroupBox("Estadísticas", this);
    QVBoxLayout* layout = new QVBoxLayout();

    lblTotalAgentes = new QLabel("Total de agentes: 0", this);
    lblEvacuados = new QLabel("Evacuados: 0", this);
    lblEnRuta = new QLabel("En ruta: 0", this);
    lblBloqueados = new QLabel("Bloqueados: 0", this);
    lblTiempoTranscurrido = new QLabel("Tiempo: 0s", this);
    
    // Nuevo: resumen de estadísticas en tiempo real
    lblEstadisticasResumen = new QLabel("Esperando simulación...", this);
    lblEstadisticasResumen->setWordWrap(true);
    lblEstadisticasResumen->setStyleSheet("QLabel { color: #0066cc; font-size: 9pt; }");
    
    // Botón para ver reporte completo
    btnVerEstadisticas = new QPushButton("📊 Ver Reporte Completo", this);
    connect(btnVerEstadisticas, &QPushButton::clicked, this, &VentanaPrincipal::mostrarReporteCompleto);

    layout->addWidget(lblTotalAgentes);
    layout->addWidget(lblEvacuados);
    layout->addWidget(lblEnRuta);
    layout->addWidget(lblBloqueados);
    layout->addWidget(lblTiempoTranscurrido);
    layout->addWidget(new QLabel("─────────────", this));
    layout->addWidget(lblEstadisticasResumen);
    layout->addWidget(btnVerEstadisticas);
    layout->addStretch();

    panelEstadisticas->setLayout(layout);
    panelEstadisticas->setMaximumWidth(250);

    // Panel de herramientas de dibujo
    panelHerramientas = new QGroupBox("Herramientas de Dibujo", this);
    QVBoxLayout* layoutHerr = new QVBoxLayout();

    QLabel* lblHerr = new QLabel("Herramienta:", this);
    comboHerramientas = new QComboBox(this);
    comboHerramientas->addItem("🟩 Piso");
    comboHerramientas->addItem("🧱 Pared");
    comboHerramientas->addItem("🚪 Salida");
    comboHerramientas->addItem("🚶 Persona");
    comboHerramientas->addItem("🚑 Rescatista");
    comboHerramientas->addItem("🗑️ Borrar");

    layoutHerr->addWidget(lblHerr);
    layoutHerr->addWidget(comboHerramientas);

    connect(comboHerramientas, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &VentanaPrincipal::cambiarHerramienta);

    // Dimensiones del escenario
    QLabel* lblDim = new QLabel("Nuevo escenario:", this);
    QHBoxLayout* layoutDim = new QHBoxLayout();

    spinFilas = new QSpinBox(this);
    spinFilas->setMinimum(10);
    spinFilas->setMaximum(100);
    spinFilas->setValue(20);
    spinFilas->setPrefix("Filas: ");

    spinColumnas = new QSpinBox(this);
    spinColumnas->setMinimum(10);
    spinColumnas->setMaximum(100);
    spinColumnas->setValue(20);
    spinColumnas->setPrefix("Cols: ");

    layoutDim->addWidget(spinFilas);
    layoutDim->addWidget(spinColumnas);

    btnNuevoEscenario = new QPushButton("Crear Escenario", this);
    connect(btnNuevoEscenario, &QPushButton::clicked, this, &VentanaPrincipal::nuevoEscenario);

    layoutHerr->addWidget(lblDim);
    layoutHerr->addLayout(layoutDim);
    layoutHerr->addWidget(btnNuevoEscenario);

    panelHerramientas->setLayout(layoutHerr);
    panelHerramientas->setMaximumWidth(250);

    // Panel de agentes
    panelAgentes = new QGroupBox("Configuración de Agentes", this);
    QVBoxLayout* layoutAg = new QVBoxLayout();

    QLabel* lblEdad = new QLabel("Edad del agente:", this);
    spinEdad = new QSpinBox(this);
    spinEdad->setMinimum(1);
    spinEdad->setMaximum(100);
    spinEdad->setValue(30);

    QLabel* lblTipo = new QLabel("Tipo:", this);
    comboTipoAgente = new QComboBox(this);
    comboTipoAgente->addItem("Normal");
    comboTipoAgente->addItem("Movilidad Reducida");

    layoutAg->addWidget(lblEdad);
    layoutAg->addWidget(spinEdad);
    layoutAg->addWidget(lblTipo);
    layoutAg->addWidget(comboTipoAgente);

    panelAgentes->setLayout(layoutAg);
    panelAgentes->setMaximumWidth(250);
}

void VentanaPrincipal::configurarLayout() {
    QWidget* centralWidget = new QWidget(this);
    QHBoxLayout* layoutPrincipal = new QHBoxLayout();

    // Panel izquierdo con controles
    QVBoxLayout* layoutIzquierdo = new QVBoxLayout();
    layoutIzquierdo->addWidget(panelHerramientas);
    layoutIzquierdo->addWidget(panelAgentes);
    layoutIzquierdo->addWidget(panelControl);
    layoutIzquierdo->addWidget(panelEstadisticas);
    layoutIzquierdo->addStretch();

    // Área central con el escenario
    vistaEscenario->setMinimumSize(600, 600);

    layoutPrincipal->addLayout(layoutIzquierdo);
    layoutPrincipal->addWidget(vistaEscenario, 1);

    centralWidget->setLayout(layoutPrincipal);
    setCentralWidget(centralWidget);
}

void VentanaPrincipal::conectarSeñales() {
    connect(simulador, &Simulador::mundoActualizado,
            this, &VentanaPrincipal::actualizarVista);
    connect(simulador, &Simulador::simulacionTerminada,
            this, &VentanaPrincipal::onSimulacionTerminada);
    connect(simulador, &Simulador::estadisticasActualizadas,
            this, &VentanaPrincipal::actualizarEstadisticasTexto);
    connect(vistaEscenario, &VistaEscenario::escenarioModificado,
            this, &VentanaPrincipal::actualizarEstadisticas);
    
    // CRÍTICO: Conectar señal de agentes agregados desde la vista al simulador
    connect(vistaEscenario, &VistaEscenario::agenteAgregado,
            [this](std::shared_ptr<AgenteBase> agente) {
                simulador->agregarAgente(agente);
                qDebug() << "🎯 Agente agregado al simulador - ID:" << agente->getId();
                actualizarEstadisticas();
            });
}

void VentanaPrincipal::nuevoEscenario() {
    if (simulacionEnEjecucion) {
        QMessageBox::warning(this, "Advertencia",
                             "Detén la simulación antes de crear un nuevo escenario.");
        return;
    }

    int filas = spinFilas->value();
    int columnas = spinColumnas->value();

    simulador->cargarEscenario(filas, columnas);
    vistaEscenario->redimensionarEscenario(filas, columnas);
    vistaEscenario->limpiarAgentes();

    statusBar()->showMessage(QString("Nuevo escenario creado: %1x%2").arg(filas).arg(columnas));
    actualizarEstadisticas();
}

void VentanaPrincipal::guardarConfiguracion() {
    QString nombreArchivo = QFileDialog::getSaveFileName(
        this, "Guardar Configuración", "", "JSON Files (*.json)");

    if (nombreArchivo.isEmpty()) return;

    QJsonObject config;

    // Guardar dimensiones del escenario
    Escenario* esc = simulador->getEscenario();
    config["filas"] = esc->filas;
    config["columnas"] = esc->columnas;

    // Guardar grid
    QJsonArray gridArray;
    for (int i = 0; i < esc->filas; ++i) {
        QJsonArray fila;
        for (int j = 0; j < esc->columnas; ++j) {
            fila.append(esc->grid[i][j]);
        }
        gridArray.append(fila);
    }
    config["grid"] = gridArray;

    // Guardar agentes
    QJsonArray agentesArray;
    for (const auto& agente : vistaEscenario->getAgentesCreados()) {
        QJsonObject agenteObj;
        agenteObj["id"] = agente->getId();
        agenteObj["x"] = agente->getPosicion().x();
        agenteObj["y"] = agente->getPosicion().y();
        agenteObj["tipo"] = static_cast<int>(agente->getTipoComportamiento());

        // Si es persona, guardar edad
        if (const auto persona_ptr = std::dynamic_pointer_cast<Persona>(agente)) {
            agenteObj["edad"] = persona_ptr->getEdad();
            agenteObj["movilidadReducida"] = persona_ptr->tieneMovilidadReducida();
        }

        agentesArray.append(agenteObj);
    }
    config["agentes"] = agentesArray;

    // Escribir archivo
    QJsonDocument doc(config);
    QFile file(nombreArchivo);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson());
        file.close();
        archivoActual = nombreArchivo;
        statusBar()->showMessage("Configuración guardada: " + nombreArchivo);
    } else {
        QMessageBox::critical(this, "Error", "No se pudo guardar el archivo.");
    }
}

void VentanaPrincipal::abrirConfiguracion() {
    if (simulacionEnEjecucion) {
        QMessageBox::warning(this, "Advertencia",
                             "Detén la simulación antes de cargar una configuración.");
        return;
    }

    QString nombreArchivo = QFileDialog::getOpenFileName(
        this, "Abrir Configuración", "", "JSON Files (*.json)");

    if (nombreArchivo.isEmpty()) return;

    QFile file(nombreArchivo);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, "Error", "No se pudo abrir el archivo.");
        return;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonObject config = doc.object();

    // Cargar dimensiones
    int filas = config["filas"].toInt();
    int columnas = config["columnas"].toInt();

    simulador->cargarEscenario(filas, columnas);
    vistaEscenario->redimensionarEscenario(filas, columnas);
    vistaEscenario->limpiarAgentes();

    // Cargar grid
    QJsonArray gridArray = config["grid"].toArray();
    Escenario* esc = simulador->getEscenario();
    for (int i = 0; i < filas && i < gridArray.size(); ++i) {
        QJsonArray fila = gridArray[i].toArray();
        for (int j = 0; j < columnas && j < fila.size(); ++j) {
            esc->setCelda(i, j, fila[j].toInt());
        }
    }

    // Cargar agentes
    QJsonArray agentesArray = config["agentes"].toArray();
    for (const auto& agenteVal : agentesArray) {
        QJsonObject agenteObj = agenteVal.toObject();
        QPoint pos(agenteObj["x"].toInt(), agenteObj["y"].toInt());
        int tipo = agenteObj["tipo"].toInt();

        if (tipo == static_cast<int>(TipoComportamiento::RESCATISTA)) {
            auto rescatista = factoria->crearRescatista(pos);
            simulador->agregarAgente(rescatista); // Pasamos el shared_ptr directo!
        } else {
            int edad = agenteObj["edad"].toInt(30);
            bool movilidadReducida = agenteObj["movilidadReducida"].toBool(false);
            auto persona = factoria->crearPersona(pos, edad, movilidadReducida);
            simulador->agregarAgente(persona); // Pasamos el shared_ptr directo!
        }
    }

    archivoActual = nombreArchivo;
    vistaEscenario->update();
    actualizarEstadisticas();
    statusBar()->showMessage("Configuración cargada: " + nombreArchivo);
}

void VentanaPrincipal::salir() {
    close();
}

void VentanaPrincipal::mostrarAcercaDe() {
    QMessageBox::about(this, "Acerca de",
                       "<h2>Simulador de Evacuación</h2>"
                       "<p>Sistema Multiagente para Simulación de Evacuación de Edificios</p>"
                       "<p><b>Características:</b></p>"
                       "<ul>"
                       "<li>Patrones de diseño: Factory, Prototype, Observer</li>"
                       "<li>Algoritmo BFS para pathfinding</li>"
                       "<li>Agentes con comportamientos diferenciados</li>"
                       "<li>Interfaz gráfica interactiva en Qt</li>"
                       "</ul>"
                       "<p>Versión 1.0 - 2024</p>");
}

void VentanaPrincipal::iniciarSimulacion() {
    if (vistaEscenario->getAgentesCreados().empty()) {
        QMessageBox::information(this, "Información",
                                 "Agrega agentes al escenario antes de iniciar la simulación.");
        return;
    }

    if (simulador->getEscenario()->getSalidaMasCercana(QPoint(0,0)) == QPoint(-1, -1)) {
        QMessageBox::critical(this, "Error de Configuración",
                              "¡Debes definir al menos una Salida (🚪) en el mapa!");
        return; //aborta la simulación si no hay destino
    }

    simulador->iniciar();
    simulacionEnEjecucion = true;
    actualizarEstadoBotones(true);
    statusBar()->showMessage("Simulación en ejecución...");
}

void VentanaPrincipal::pausarSimulacion() {
    simulador->pausar();
    simulacionEnEjecucion = false;
    actualizarEstadoBotones(false);
    statusBar()->showMessage("Simulación pausada.");
}

void VentanaPrincipal::reiniciarSimulacion() {
    // 1. Pausar y limpiar el simulador completamente
    simulador->pausar();
    simulador->reiniciar();
    
    // 2. Limpiar la vista del escenario
    vistaEscenario->limpiarAgentes();
    
    // 3. Reiniciar las dimensiones del escenario (mantener el tamaño actual)
    Escenario* esc = simulador->getEscenario();
    if (esc) {
        int filas = esc->filas;
        int columnas = esc->columnas;
        simulador->cargarEscenario(filas, columnas);
        vistaEscenario->setEscenario(simulador->getEscenario());
        vistaEscenario->setAgentes(&simulador->getAgentes());
    }
    
    // 4. Reiniciar variables de estado
    simulacionEnEjecucion = false;
    tiempoSimulacion = 0;
    archivoActual.clear(); // Limpiar referencia al archivo
    
    // 5. Actualizar interfaz
    actualizarEstadoBotones(false);
    actualizarEstadisticas();
    
    // 6. Reiniciar etiquetas de estadísticas
    lblTotalAgentes->setText("Total de agentes: 0");
    lblEvacuados->setText("Evacuados: 0");
    lblEnRuta->setText("En ruta: 0");
    lblBloqueados->setText("Bloqueados: 0");
    lblTiempoTranscurrido->setText("Tiempo: 0s");
    lblEstadisticasResumen->setText("Esperando simulación...");
    
    statusBar()->showMessage("Todo reiniciado. El escenario está vacío y listo para dibujar.");
}

void VentanaPrincipal::actualizarVista() {
    vistaEscenario->update();
    actualizarEstadisticas();

    tiempoSimulacion++;
    lblTiempoTranscurrido->setText(QString("Tiempo: %1s").arg(tiempoSimulacion / 2));
}

void VentanaPrincipal::onSimulacionTerminada() {
    simulacionEnEjecucion = false;
    actualizarEstadoBotones(false);
    statusBar()->showMessage("¡Simulación completada! Todos los agentes evacuados.");

    QMessageBox::information(this, "Simulación Completada",
                             QString("Todos los agentes han sido evacuados exitosamente.\n\n"
                                     "Tiempo total: %1 segundos").arg(tiempoSimulacion / 2));
}

void VentanaPrincipal::cambiarHerramienta(int index) {
    vistaEscenario->setHerramientaActual(static_cast<HerramientaDibujo>(index));
    statusBar()->showMessage("Herramienta: " + obtenerNombreHerramienta(index));
}

void VentanaPrincipal::cambiarVelocidadSimulacion(int valor) {
    // Mapear valor del slider a intervalos del timer
    int intervalo = 1100 - (valor * 100);

    QString texto;
    if (valor <= 3) texto = QString("Lenta (%1ms)").arg(intervalo);
    else if (valor <= 7) texto = QString("Normal (%1ms)").arg(intervalo);
    else texto = QString("Rápida (%1ms)").arg(intervalo);

    lblVelocidad->setText(texto);

    // Aquí deberías ajustar el intervalo del timer del simulador
    // Esto requiere agregar un método en Simulador para cambiar la velocidad
}

void VentanaPrincipal::actualizarEstadisticas() {
    const auto& agentes = simulador->getAgentes();

    int total = agentes.size();
    int evacuados = 0;
    int enRuta = 0;
    int bloqueados = 0;

    for (const auto& agente : agentes) {
        switch (agente->getEstado()) {
        case EstadoAgente::EVACUADO:
            evacuados++;
            break;
        case EstadoAgente::EVACUANDO:
            enRuta++;
            break;
        case EstadoAgente::BLOQUEADO:
            bloqueados++;
            break;
        default:
            break;
        }
    }

    lblTotalAgentes->setText(QString("Total de agentes: %1").arg(total));
    lblEvacuados->setText(QString("Evacuados: %1").arg(evacuados));
    lblEnRuta->setText(QString("En ruta: %1").arg(enRuta));
    lblBloqueados->setText(QString("Bloqueados: %1").arg(bloqueados));
}

void VentanaPrincipal::actualizarEstadisticasTexto(QString resumen) {
    lblEstadisticasResumen->setText(resumen);
}

void VentanaPrincipal::mostrarReporteCompleto() {
    EstadisticasSimulacion* stats = simulador->getEstadisticas();
    stats->calcularEstadisticas();
    QString reporte = QString::fromStdString(stats->generarReporte());
    
    // Crear diálogo para mostrar el reporte
    QDialog* dialogo = new QDialog(this);
    dialogo->setWindowTitle("📊 Reporte Completo de Estadísticas");
    dialogo->resize(800, 600);
    
    QVBoxLayout* layout = new QVBoxLayout(dialogo);
    
    QTextEdit* textoReporte = new QTextEdit(dialogo);
    textoReporte->setReadOnly(true);
    textoReporte->setPlainText(reporte);
    textoReporte->setFont(QFont("Courier", 10));
    
    QPushButton* btnExportar = new QPushButton("💾 Exportar a Archivo", dialogo);
    QPushButton* btnCerrar = new QPushButton("Cerrar", dialogo);
    
    QHBoxLayout* layoutBotones = new QHBoxLayout();
    layoutBotones->addWidget(btnExportar);
    layoutBotones->addStretch();
    layoutBotones->addWidget(btnCerrar);
    
    layout->addWidget(textoReporte);
    layout->addLayout(layoutBotones);
    
    connect(btnCerrar, &QPushButton::clicked, dialogo, &QDialog::accept);
    connect(btnExportar, &QPushButton::clicked, [this, stats]() {
        QString archivo = QFileDialog::getSaveFileName(
            this, "Exportar Estadísticas", "", 
            "Archivo de Texto (*.txt);;CSV (*.csv);;JSON (*.json)"
        );
        if (!archivo.isEmpty()) {
            stats->exportarReporte(archivo.toStdString());
            QMessageBox::information(this, "Éxito", 
                "Estadísticas exportadas correctamente a:\n" + archivo);
        }
    });
    
    dialogo->exec();
    delete dialogo;
}

void VentanaPrincipal::actualizarEstadoBotones(bool simulacionActiva) {
    btnIniciar->setEnabled(!simulacionActiva);
    btnPausar->setEnabled(simulacionActiva);
    btnNuevoEscenario->setEnabled(!simulacionActiva);
    comboHerramientas->setEnabled(!simulacionActiva);
}

QString VentanaPrincipal::obtenerNombreHerramienta(int index) const {
    QStringList nombres = {"Piso", "Pared", "Salida", "Persona", "Rescatista", "Borrar"};
    return nombres.value(index, "Desconocida");
}
