#ifndef VENTANAPRINCIPAL_H
#define VENTANAPRINCIPAL_H

#include <QMainWindow>
#include <QPushButton>
#include <QSpinBox>
#include <QComboBox>
#include <QLabel>
#include <QFileDialog>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QSlider>
#include "Simulador.h"
#include "VistaEscenario.h"
#include "FactoriaAgentes.h"

/**
 * @brief Ventana principal de la aplicación
 *
 * Gestiona la interfaz de usuario y coordina todos los componentes:
 * - Vista del escenario (canvas interactivo)
 * - Controles de simulación
 * - Menús y herramientas
 * - Guardado/Carga de configuraciones
 */
class VentanaPrincipal : public QMainWindow {
    Q_OBJECT

public:
    explicit VentanaPrincipal(QWidget *parent = nullptr);
    ~VentanaPrincipal();

private slots:
    // Slots de menú
    void nuevoEscenario();
    void guardarConfiguracion();
    void abrirConfiguracion();
    void salir();
    void mostrarAcercaDe();

    // Slots de control de simulación
    void iniciarSimulacion();
    void pausarSimulacion();
    void reiniciarSimulacion();
    void actualizarVista();
    void onSimulacionTerminada();

    // Slots de herramientas
    void cambiarHerramienta(int index);
    void cambiarVelocidadSimulacion(int valor);

    // Slots de estadísticas
    void actualizarEstadisticas();

private:
    // Métodos de inicialización
    void crearMenus();
    void crearBarraHerramientas();
    void crearPanelControl();
    void crearPanelEstadisticas();
    void configurarLayout();
    void conectarSeñales();

    // Métodos auxiliares
    void actualizarEstadoBotones(bool simulacionActiva);
    QString obtenerNombreHerramienta(int index) const;

    // Componentes principales
    Simulador* simulador;
    VistaEscenario* vistaEscenario;
    FactoriaAgentes* factoria;

    // Menús
    QMenu* menuArchivo;
    QMenu* menuSimulacion;
    QMenu* menuAyuda;

    // Barra de herramientas
    QToolBar* barraHerramientas;

    // Panel de control
    QGroupBox* panelControl;
    QPushButton* btnIniciar;
    QPushButton* btnPausar;
    QPushButton* btnReiniciar;
    QSlider* sliderVelocidad;
    QLabel* lblVelocidad;

    // Panel de herramientas de dibujo
    QGroupBox* panelHerramientas;
    QComboBox* comboHerramientas;
    QSpinBox* spinFilas;
    QSpinBox* spinColumnas;
    QPushButton* btnNuevoEscenario;

    // Panel de agentes
    QGroupBox* panelAgentes;
    QSpinBox* spinEdad;
    QComboBox* comboTipoAgente;

    // Panel de estadísticas
    QGroupBox* panelEstadisticas;
    QLabel* lblTotalAgentes;
    QLabel* lblEvacuados;
    QLabel* lblEnRuta;
    QLabel* lblBloqueados;
    QLabel* lblTiempoTranscurrido;

    // Estado de la aplicación
    bool simulacionEnEjecucion;
    int tiempoSimulacion;
    QString archivoActual;
};

#endif // VENTANAPRINCIPAL_H
