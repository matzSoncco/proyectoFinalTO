#include "../include/VistaEscenario.h"
#include "../include/Persona.h"
#include "../include/Rescatista.h"
#include <QPainter>
#include <QMouseEvent>
#include <cmath>

VistaEscenario::VistaEscenario(QWidget *parent)
    : QWidget(parent),
    escenario(nullptr),
    agentes(nullptr),
    factoria(nullptr),
    tamañoCelda(TAMAÑO_CELDA_DEFAULT),
    margenX(MARGEN),
    margenY(MARGEN),
    escalaZoom(1.0f),
    offsetPan(0, 0),
    herramientaActual(HerramientaDibujo::PISO),
    edadAgenteNuevo(30),
    movilidadReducidaNuevo(false),
    dibujandoArrastre(false) {

    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
    setMinimumSize(400, 400);
}

VistaEscenario::~VistaEscenario() {
    // Los agentes creados se gestionan en el simulador
}

void VistaEscenario::setEscenario(Escenario* esc) {
    escenario = esc;
    calcularTamañoCelda();
    update();
}

void VistaEscenario::setAgentes(const std::vector<std::shared_ptr<AgenteBase>>* ags) {
    agentes = ags;
    update();
}

void VistaEscenario::setFactoria(FactoriaAgentes* fact) {
    factoria = fact;
}

void VistaEscenario::setHerramientaActual(HerramientaDibujo herr) {
    herramientaActual = herr;
}

void VistaEscenario::setEdadAgente(int edad) {
    edadAgenteNuevo = edad;
}

void VistaEscenario::setMovilidadReducida(bool reducida) {
    movilidadReducidaNuevo = reducida;
}

void VistaEscenario::limpiarEscenario() {
    if (!escenario) return;

    for (int i = 0; i < escenario->filas; ++i) {
        for (int j = 0; j < escenario->columnas; ++j) {
            escenario->setCelda(i, j, 0);
        }
    }
    update();
}

void VistaEscenario::limpiarAgentes() {
    agentesCreados.clear();
    update();
}

void VistaEscenario::redimensionarEscenario(int filas, int columnas) {
    if (!escenario) return;

    escenario->filas = filas;
    escenario->columnas = columnas;
    escenario->grid.clear();
    escenario->grid.resize(filas, std::vector<int>(columnas, 0));

    limpiarAgentes();
    calcularTamañoCelda();
    update();
}

void VistaEscenario::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Fondo
    painter.fillRect(rect(), QColor(240, 240, 240));

    if (!escenario) {
        painter.drawText(rect(), Qt::AlignCenter,
                         "No hay escenario cargado.\nCrea uno nuevo.");
        return;
    }

    // Aplicar transformaciones
    painter.translate(offsetPan);
    painter.scale(escalaZoom, escalaZoom);

    // Dibujar componentes
    dibujarGrid(painter);
    dibujarAgentes(painter);
    dibujarGuias(painter);
}

void VistaEscenario::dibujarGrid(QPainter& painter) {
    if (!escenario) return;

    for (int i = 0; i < escenario->filas; ++i) {
        for (int j = 0; j < escenario->columnas; ++j) {
            dibujarCelda(painter, i, j);
        }
    }
}

void VistaEscenario::dibujarCelda(QPainter& painter, int fila, int col) {
    QPoint pos = gridAPixel(fila, col);
    QRect rect(pos.x(), pos.y(), tamañoCelda, tamañoCelda);

    int tipoCelda = escenario->grid[fila][col];
    QColor color = obtenerColorCelda(tipoCelda);

    // Rellenar celda
    painter.fillRect(rect, color);

    // Borde
    painter.setPen(QPen(QColor(200, 200, 200), 1));
    painter.drawRect(rect);

    // Iconos especiales
    if (tipoCelda == 1) { // Pared
        painter.setPen(QPen(QColor(100, 100, 100), 2));
        painter.drawLine(pos.x() + 2, pos.y() + 2,
                         pos.x() + tamañoCelda - 2, pos.y() + tamañoCelda - 2);
        painter.drawLine(pos.x() + tamañoCelda - 2, pos.y() + 2,
                         pos.x() + 2, pos.y() + tamañoCelda - 2);
    } else if (tipoCelda == 2) { // Salida
        painter.setPen(QPen(QColor(0, 150, 0), 3));
        QFont font = painter.font();
        font.setPointSize(tamañoCelda / 3);
        font.setBold(true);
        painter.setFont(font);
        painter.drawText(rect, Qt::AlignCenter, "S");
    }
}

void VistaEscenario::dibujarAgentes(QPainter& painter) {
    if (!agentes) return;

    for (const auto& agente_ptr : *agentes) {
        dibujarAgente(painter, agente_ptr.get());
    }

    // Dibujar también los agentes creados que aún no están en el simulador
    for (const auto& agente_creado_ptr : agentesCreados) {
        bool yaEnSimulador = false;
        for (const auto& a_ptr : *agentes) {
            if (a_ptr.get() == agente_creado_ptr.get()) {
                yaEnSimulador = true;
                break;
            }
        }
        if (!yaEnSimulador) {
            dibujarAgente(painter, agente_creado_ptr.get());
        }
    }
}

void VistaEscenario::dibujarAgente(QPainter& painter, AgenteBase* agente) {
    if (!agente) return;

    QPoint posGrid = agente->getPosicion();
    QPoint posPantalla = gridAPixel(posGrid.x(), posGrid.y());

    // Centro de la celda
    int centroX = posPantalla.x() + tamañoCelda / 2;
    int centroY = posPantalla.y() + tamañoCelda / 2;
    int radio = tamañoCelda / 3;

    QColor color = obtenerColorAgente(agente);

    // Dibujar círculo para el agente
    painter.setBrush(QBrush(color));
    painter.setPen(QPen(Qt::black, 2));
    painter.drawEllipse(QPoint(centroX, centroY), radio, radio);

    // Letra identificadora
    QFont font = painter.font();
    font.setPointSize(tamañoCelda / 4);
    font.setBold(true);
    painter.setFont(font);
    painter.setPen(Qt::white);

    QString letra;
    if (dynamic_cast<Rescatista*>(agente)) {
        letra = "R";
    } else if (auto persona = dynamic_cast<Persona*>(agente)) {
        letra = persona->tieneMovilidadReducida() ? "M" : "P";
    }

    painter.drawText(QRect(centroX - radio, centroY - radio,
                           radio * 2, radio * 2),
                     Qt::AlignCenter, letra);
}

void VistaEscenario::dibujarGuias(QPainter& painter) {
    // Dibujar información adicional si es necesario
}

void VistaEscenario::mousePressEvent(QMouseEvent *event) {
    if (!escenario || event->button() != Qt::LeftButton) return;

    QPoint posClick = event->pos();
    dibujandoArrastre = true;
    ultimaPosArrastre = posClick;

    procesarClick(posClick);
}

void VistaEscenario::mouseMoveEvent(QMouseEvent *event) {
    if (!escenario) return;

    if (dibujandoArrastre && (herramientaActual == HerramientaDibujo::PISO ||
                              herramientaActual == HerramientaDibujo::PARED ||
                              herramientaActual == HerramientaDibujo::BORRAR)) {
        procesarArrastre(event->pos());
    }

    // Mostrar coordenadas en la barra de estado
    QPoint gridPos = pixelAGrid(event->pos());
    if (gridPos.x() >= 0 && gridPos.x() < escenario->filas &&
        gridPos.y() >= 0 && gridPos.y() < escenario->columnas) {
        emit infoActualizada(QString("Posición: (%1, %2)").arg(gridPos.x()).arg(gridPos.y()));
    }
}

void VistaEscenario::mouseReleaseEvent(QMouseEvent *event) {
    Q_UNUSED(event);
    dibujandoArrastre = false;
}

void VistaEscenario::wheelEvent(QWheelEvent *event) {
    // Zoom con la rueda del mouse
    float factorZoom = event->angleDelta().y() > 0 ? ESCALA_ZOOM_FACTOR : 1.0f / ESCALA_ZOOM_FACTOR;

    escalaZoom *= factorZoom;
    escalaZoom = std::max(0.5f, std::min(3.0f, escalaZoom));

    update();
}

void VistaEscenario::resizeEvent(QResizeEvent *event) {
    Q_UNUSED(event);
    calcularTamañoCelda();
}

void VistaEscenario::procesarClick(QPoint pos) {
    QPoint gridPos = pixelAGrid(pos);

    if (gridPos.x() < 0 || gridPos.x() >= escenario->filas ||
        gridPos.y() < 0 || gridPos.y() >= escenario->columnas) {
        return;
    }

    switch (herramientaActual) {
    case HerramientaDibujo::PISO:
        aplicarHerramientaPiso(gridPos.x(), gridPos.y());
        break;
    case HerramientaDibujo::PARED:
        aplicarHerramientaPared(gridPos.x(), gridPos.y());
        break;
    case HerramientaDibujo::SALIDA:
        aplicarHerramientaSalida(gridPos.x(), gridPos.y());
        break;
    case HerramientaDibujo::PERSONA:
        aplicarHerramientaPersona(gridPos.x(), gridPos.y());
        break;
    case HerramientaDibujo::RESCATISTA:
        aplicarHerramientaRescatista(gridPos.x(), gridPos.y());
        break;
    case HerramientaDibujo::BORRAR:
        aplicarHerramientaBorrar(gridPos.x(), gridPos.y());
        break;
    }
}

void VistaEscenario::procesarArrastre(QPoint pos) {
    if (pos == ultimaPosArrastre) return;

    ultimaPosArrastre = pos;
    procesarClick(pos);
}

QPoint VistaEscenario::pixelAGrid(QPoint pixel) const {
    if (!escenario) return QPoint(-1, -1);

    // Ajustar por transformaciones
    pixel = pixel - offsetPan;
    pixel = QPoint(pixel.x() / escalaZoom, pixel.y() / escalaZoom);

    int fila = (pixel.y() - margenY) / tamañoCelda;
    int col = (pixel.x() - margenX) / tamañoCelda;

    return QPoint(fila, col);
}

QPoint VistaEscenario::gridAPixel(int fila, int col) const {
    int x = margenX + col * tamañoCelda;
    int y = margenY + fila * tamañoCelda;
    return QPoint(x, y);
}

void VistaEscenario::aplicarHerramientaPiso(int fila, int col) {
    escenario->setCelda(fila, col, 0);
    emit escenarioModificado();
    update();
}

void VistaEscenario::aplicarHerramientaPared(int fila, int col) {
    escenario->setCelda(fila, col, 1);
    emit escenarioModificado();
    update();
}

void VistaEscenario::aplicarHerramientaSalida(int fila, int col) {
    escenario->setCelda(fila, col, 2);
    emit escenarioModificado();
    update();
}

void VistaEscenario::aplicarHerramientaPersona(int fila, int col) {
    if (!factoria) return;

    QPoint pos(fila, col);

    // Verificar que la celda sea transitable
    if (!escenario->esTransitable(fila, col)) {
        return;
    }

    // Verificar que no haya otro agente en esa posición
    if (posicionOcupada(pos)) {
        return;
    }

    auto persona = factoria->crearPersona(pos, edadAgenteNuevo, movilidadReducidaNuevo);
    agentesCreados.push_back(persona);

    emit agenteAgregado(persona);
    update();
}

void VistaEscenario::aplicarHerramientaRescatista(int fila, int col) {
    if (!factoria) return;

    QPoint pos(fila, col);

    // Verificar que la celda sea transitable
    if (!escenario->esTransitable(fila, col)) {
        return;
    }

    // Verificar que no haya otro agente en esa posición
    if (posicionOcupada(pos)) {
        return;
    }

    auto rescatista = factoria->crearRescatista(pos);
    agentesCreados.push_back(rescatista);

    emit agenteAgregado(rescatista);
    update();
}

void VistaEscenario::aplicarHerramientaBorrar(int fila, int col) {
    QPoint pos(fila, col);

    escenario->setCelda(fila, col, 0);

    agentesCreados.erase(
        std::remove_if(agentesCreados.begin(), agentesCreados.end(),
                       [pos](const std::shared_ptr<AgenteBase>& agente_ptr) {
                           return agente_ptr->getPosicion() == pos;
                       }),
        agentesCreados.end()
        );
    emit escenarioModificado();
    update();
}

void VistaEscenario::calcularTamañoCelda() {
    if (!escenario) return;

    int anchoDisponible = width() - 2 * MARGEN;
    int altoDisponible = height() - 2 * MARGEN;

    int tamañoPorColumnas = anchoDisponible / escenario->columnas;
    int tamañoPorFilas = altoDisponible / escenario->filas;

    tamañoCelda = std::min(tamañoPorColumnas, tamañoPorFilas);
    tamañoCelda = std::max(TAMAÑO_CELDA_MIN, std::min(TAMAÑO_CELDA_MAX, tamañoCelda));

    // Centrar el grid
    int anchoTotal = escenario->columnas * tamañoCelda;
    int altoTotal = escenario->filas * tamañoCelda;

    margenX = (width() - anchoTotal) / 2;
    margenY = (height() - altoTotal) / 2;
}

QColor VistaEscenario::obtenerColorCelda(int tipoCelda) const {
    switch (tipoCelda) {
    case 0: return QColor(255, 255, 255);      // Piso - Blanco
    case 1: return QColor(100, 100, 100);      // Pared - Gris oscuro
    case 2: return QColor(100, 255, 100);      // Salida - Verde claro
    default: return QColor(255, 255, 255);
    }
}

QColor VistaEscenario::obtenerColorAgente(AgenteBase* agente) const {
    if (!agente) return Qt::black;

    // Color según el estado
    switch (agente->getEstado()) {
    case EstadoAgente::EVACUADO:
        return QColor(0, 200, 0);       // Verde - Evacuado
    case EstadoAgente::EVACUANDO:
        return QColor(0, 100, 255);     // Azul - En ruta
    case EstadoAgente::PANICO:
        return QColor(255, 150, 0);     // Naranja - Pánico
    case EstadoAgente::BLOQUEADO:
        return QColor(255, 0, 0);       // Rojo - Bloqueado
    default:
        break;
    }

    // Color según el tipo
    if (dynamic_cast<Rescatista*>(agente)) {
        return QColor(0, 150, 255);         // Azul rescatista
    } else if (auto persona = dynamic_cast<Persona*>(agente)) {
        if (persona->tieneMovilidadReducida()) {
            return QColor(200, 100, 200);   // Púrpura - Movilidad reducida
        }
        return QColor(100, 100, 255);       // Azul persona normal
    }

    return Qt::gray;
}

bool VistaEscenario::posicionOcupada(QPoint pos) const {
    // Verificar en agentes creados
    for (const auto& agente : agentesCreados) {
        if (agente->getPosicion() == pos) {
            return true;
        }
    }

    // Verificar en agentes del simulador
    if (agentes) {
        for (const auto& agente : *agentes) {
            if (agente->getPosicion() == pos) {
                return true;
            }
        }
    }

    return false;
}
