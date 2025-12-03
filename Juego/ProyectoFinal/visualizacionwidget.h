#ifndef VISUALIZACIONWIDGET_H
#define VISUALIZACIONWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QTimer>
#include "Cohete.h"
#include "Nivel.h"

class VisualizacionWidget : public QWidget
{
    Q_OBJECT

public:
    explicit VisualizacionWidget(QWidget *parent = nullptr);
    ~VisualizacionWidget();

    // Métodos para actualizar el estado
    void actualizarCohete(const Cohete* cohete);
    void actualizarNivel(const Nivel* nivel, int numeroNivel);
    void iniciarAnimacion();
    void detenerAnimacion();
    void reiniciar();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    // Datos del cohete
    const Cohete* coheteActual;
    const Nivel* nivelActual;
    int numeroNivel;

    // Control de animación
    QTimer* timerAnimacion;
    int frameAnimacion;
    bool animacionActiva;

    // Parámetros de visualización
    double escalaAltura;      // Factor de escala para mostrar la altura
    double alturaMaximaVista; // Altura máxima visible en pantalla
    QPointF posicionCohete;   // Posición en píxeles del cohete

    // Métodos de dibujo
    void dibujarFondo(QPainter& painter);
    void dibujarTierra(QPainter& painter);
    void dibujarAtmosfera(QPainter& painter);
    void dibujarEspacio(QPainter& painter);
    void dibujarEstrellas(QPainter& painter);
    void dibujarCohete(QPainter& painter);
    void dibujarPropulsion(QPainter& painter, const QPointF& posicion);
    void dibujarIndicadores(QPainter& painter);
    void dibujarLineaObjetivo(QPainter& painter);
    void dibujarMarcadoresAltura(QPainter& painter);
    void dibujarLuna(QPainter& painter); // Para nivel 3

    // Métodos auxiliares
    void calcularPosicionCohete();
    void calcularEscalaAltura();
    QColor obtenerColorPorAltura(double altura) const;
    double alturaAPixel(double altura) const;

    // Efectos visuales
    void dibujarParticulas(QPainter& painter);
    QVector<QPointF> particulasPropulsion;

private slots:
    void actualizarAnimacion();
};

#endif // VISUALIZACIONWIDGET_H
