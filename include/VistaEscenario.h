#ifndef VISTAESCENARIO_H
#define VISTAESCENARIO_H

#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include <QPoint>
#include "Escenario.h"
#include "AgenteBase.h"
#include "FactoriaAgentes.h"
#include <memory>

/**
 * @brief Enumeración para las herramientas de dibujo
 */
enum class HerramientaDibujo {
    PISO,
    PARED,
    SALIDA,
    PERSONA,
    RESCATISTA,
    BORRAR
};

/**
 * @brief Widget personalizado para visualizar y editar el escenario
 *
 * Este widget maneja:
 * - Renderizado del grid (pisos, paredes, salidas)
 * - Visualización de agentes
 * - Interacción con el mouse para dibujar/editar
 * - Zoom y desplazamiento
 */
class VistaEscenario : public QWidget {
    Q_OBJECT

public:
    explicit VistaEscenario(QWidget *parent = nullptr);
    ~VistaEscenario();

    // Configuración del escenario
    void setEscenario(Escenario* esc);
    void setAgentes(const std::vector<std::shared_ptr<AgenteBase>>* ags);
    void setFactoria(FactoriaAgentes* fact);

    // Configuración de herramientas
    void setHerramientaActual(HerramientaDibujo herr);
    void setEdadAgente(int edad);
    void setMovilidadReducida(bool reducida);

    // Limpieza
    void limpiarEscenario();
    void limpiarAgentes();

    // Tamaño del escenario
    void redimensionarEscenario(int filas, int columnas);

    // Acceso a agentes creados
    //const std::vector<AgenteBase*>& getAgentesCreados() const { return agentesCreados; }
    const std::vector<std::shared_ptr<AgenteBase>>& getAgentesCreados() const { return agentesCreados; }

signals:
    void escenarioModificado();
    void agenteAgregado(std::shared_ptr<AgenteBase> agente);
    void infoActualizada(const QString& info);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    // Métodos de renderizado
    void dibujarGrid(QPainter& painter);
    void dibujarCelda(QPainter& painter, int fila, int col);
    void dibujarAgentes(QPainter& painter);
    void dibujarAgente(QPainter& painter, AgenteBase* agente);
    void dibujarGuias(QPainter& painter);

    // Métodos de interacción
    void procesarClick(QPoint pos);
    void procesarArrastre(QPoint pos);
    QPoint pixelAGrid(QPoint pixel) const;
    QPoint gridAPixel(int fila, int col) const;

    // Métodos de herramientas
    void aplicarHerramientaPiso(int fila, int col);
    void aplicarHerramientaPared(int fila, int col);
    void aplicarHerramientaSalida(int fila, int col);
    void aplicarHerramientaPersona(int fila, int col);
    void aplicarHerramientaRescatista(int fila, int col);
    void aplicarHerramientaBorrar(int fila, int col);

    // Métodos auxiliares
    void calcularTamañoCelda();
    QColor obtenerColorCelda(int tipoCelda) const;
    QColor obtenerColorAgente(AgenteBase* agente) const;
    bool posicionOcupada(QPoint pos) const;

    // Datos
    Escenario* escenario;
    const std::vector<std::shared_ptr<AgenteBase>>* agentes;
    FactoriaAgentes* factoria;
    std::vector<std::shared_ptr<AgenteBase>> agentesCreados;

    // Configuración de visualización
    int tamañoCelda;
    int margenX;
    int margenY;
    float escalaZoom;
    QPoint offsetPan;

    // Estado de la herramienta
    HerramientaDibujo herramientaActual;
    int edadAgenteNuevo;
    bool movilidadReducidaNuevo;
    bool dibujandoArrastre;
    QPoint ultimaPosArrastre;

    // Constantes de diseño
    static constexpr int TAMAÑO_CELDA_MIN = 15;
    static constexpr int TAMAÑO_CELDA_MAX = 60;
    static constexpr int TAMAÑO_CELDA_DEFAULT = 30;
    static constexpr int MARGEN = 20;
    static constexpr float ESCALA_ZOOM_FACTOR = 1.1f;
};

#endif // VISTAESCENARIO_H
