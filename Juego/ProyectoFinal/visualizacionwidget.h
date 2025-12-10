#ifndef VISUALIZACIONWIDGET_H
#define VISUALIZACIONWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QTimer>
#include <QPixmap>
#include <QSoundEffect>
#include <QMediaPlayer>
#include <QAudioOutput>
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

    // Sprites
    QPixmap spriteCohete;  // Sprite sheet completo
    QPixmap spriteFondo;
    QPixmap spriteFondo2;  // Fondo para nivel 2
    QPixmap spriteFondo3;  // Fondo para nivel 3
    QPixmap spriteExplosion;  // Sprite sheet de explosión
    QVector<QPixmap> framesCohete;  // Frames individuales del sprite sheet
    QVector<QPixmap> framesExplosion;  // Frames de explosión (9 frames 3x3)
    bool spritesCargados;
    int numFramesX;  
    int numFramesY;  
    bool mostrarExplosion;
    int frameExplosionActual;  // Frame actual de la explosión
    bool explosionCompletada;  // Si la explosión ya terminó de reproducirse
    
    // Sonidos
    QMediaPlayer* sonidoExplosion;
    QMediaPlayer* sonidoArranque;
    QMediaPlayer* sonidoBase;
    QAudioOutput* audioOutput;
    QAudioOutput* audioOutputExplosion;
    QAudioOutput* audioOutputArranque;
    bool sonidoArranqueReproducido;
    void cargarSonidos();
    void reproducirSonidoArranque();
    void reproducirSonidoExplosion();  

    // Métodos de dibujo
    void dibujarFondo(QPainter& painter);
    void dibujarTierra(QPainter& painter);
    void dibujarAtmosfera(QPainter& painter);
    void dibujarEstrellas(QPainter& painter);
    void dibujarCohete(QPainter& painter);
    void dibujarPropulsion(QPainter& painter, const QPointF& posicion);
    void dibujarIndicadores(QPainter& painter);
    void dibujarLineaObjetivo(QPainter& painter);
    void dibujarMarcadoresAltura(QPainter& painter);
    void dibujarLuna(QPainter& painter); 
    void dibujarExplosion(QPainter& painter);
    void dibujarAreaAterrizaje(QPainter& painter); 

    void calcularPosicionCohete();
    void calcularEscalaAltura();
    double alturaAPixel(double altura) const;
    void cargarSprites();
    void dividirSpriteSheet();
    void dividirSpriteSheetExplosion();
    int obtenerFrameSegunEmpuje(double empuje, double empujeMaximo) const;

    void dibujarParticulas(QPainter& painter);
    QVector<QPointF> particulasPropulsion;

private slots:
    void actualizarAnimacion();
};

#endif // VISUALIZACIONWIDGET_H
