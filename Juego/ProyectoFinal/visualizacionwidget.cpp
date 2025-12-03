#include "visualizacionwidget.h"
#include <QPainter>
#include <QLinearGradient>
#include <QRadialGradient>
#include <QPaintEvent>
#include <QRandomGenerator>
#include <cmath>

VisualizacionWidget::VisualizacionWidget(QWidget *parent)
    : QWidget(parent),
    coheteActual(nullptr),
    nivelActual(nullptr),
    numeroNivel(0),
    frameAnimacion(0),
    animacionActiva(false),
    escalaAltura(1.0),
    alturaMaximaVista(150000.0)
{
    // Configurar el widget
    setMinimumSize(600, 600);

    // Timer para animación
    timerAnimacion = new QTimer(this);
    connect(timerAnimacion, &QTimer::timeout, this, &VisualizacionWidget::actualizarAnimacion);

    // Generar partículas iniciales
    for(int i = 0; i < 20; ++i) {
        particulasPropulsion.append(QPointF(0, 0));
    }
}

VisualizacionWidget::~VisualizacionWidget()
{
    detenerAnimacion();
}

void VisualizacionWidget::actualizarCohete(const Cohete* cohete)
{
    coheteActual = cohete;
    calcularPosicionCohete();
    update(); // Forzar redibujado
}

void VisualizacionWidget::actualizarNivel(const Nivel* nivel, int numNivel)
{
    nivelActual = nivel;
    numeroNivel = numNivel;
    calcularEscalaAltura();
    update();
}

void VisualizacionWidget::iniciarAnimacion()
{
    animacionActiva = true;
    timerAnimacion->start(50); // 20 FPS
}

void VisualizacionWidget::detenerAnimacion()
{
    animacionActiva = false;
    timerAnimacion->stop();
}

void VisualizacionWidget::reiniciar()
{
    frameAnimacion = 0;
    particulasPropulsion.clear();
    for(int i = 0; i < 20; ++i) {
        particulasPropulsion.append(QPointF(0, 0));
    }
    update();
}

void VisualizacionWidget::actualizarAnimacion()
{
    frameAnimacion++;
    if(frameAnimacion > 1000) frameAnimacion = 0;

    // Actualizar partículas de propulsión
    if(coheteActual && coheteActual->obtenerEmpuje() > 0) {
        for(int i = particulasPropulsion.size() - 1; i > 0; --i) {
            particulasPropulsion[i] = particulasPropulsion[i-1];
        }

        double desvio = (QRandomGenerator::global()->bounded(20) - 10) / 10.0;
        particulasPropulsion[0] = QPointF(posicionCohete.x() + desvio,
                                          posicionCohete.y() + 30);
    }

    update();
}

void VisualizacionWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Dibujar en orden
    dibujarFondo(painter);

    if(numeroNivel == 3) {
        dibujarLuna(painter);
    } else {
        dibujarTierra(painter);
        dibujarAtmosfera(painter);
    }

    dibujarEstrellas(painter);
    dibujarIndicadores(painter);
    dibujarMarcadoresAltura(painter);

    if(coheteActual) {
        dibujarCohete(painter);
        if(coheteActual->obtenerEmpuje() > 0) {
            dibujarPropulsion(painter, posicionCohete);
            dibujarParticulas(painter);
        }
    }

    dibujarLineaObjetivo(painter);
}

void VisualizacionWidget::dibujarFondo(QPainter& painter)
{
    QLinearGradient gradient(0, 0, 0, height());

    if(numeroNivel == 3) {
        // Fondo espacial para la Luna
        gradient.setColorAt(0.0, QColor(10, 10, 20));
        gradient.setColorAt(1.0, QColor(5, 5, 15));
    } else if(coheteActual && coheteActual->obtenerAltura() < 50000) {
        // Atmósfera baja
        gradient.setColorAt(0.0, QColor(10, 10, 30));
        gradient.setColorAt(0.3, QColor(20, 30, 60));
        gradient.setColorAt(1.0, QColor(100, 150, 200));
    } else if(coheteActual && coheteActual->obtenerAltura() < 100000) {
        // Atmósfera alta
        gradient.setColorAt(0.0, QColor(5, 5, 15));
        gradient.setColorAt(0.5, QColor(10, 20, 40));
        gradient.setColorAt(1.0, QColor(50, 80, 120));
    } else {
        // Espacio
        gradient.setColorAt(0.0, QColor(5, 5, 15));
        gradient.setColorAt(1.0, QColor(10, 10, 25));
    }

    painter.fillRect(rect(), gradient);
}

void VisualizacionWidget::dibujarTierra(QPainter& painter)
{
    int alturaBase = height() - 50;

    // Superficie de la Tierra
    QLinearGradient tierraGradient(0, alturaBase - 100, 0, alturaBase);
    tierraGradient.setColorAt(0.0, QColor(34, 139, 34));  // Verde
    tierraGradient.setColorAt(0.5, QColor(101, 67, 33)); // Marrón
    tierraGradient.setColorAt(1.0, QColor(139, 90, 43)); // Marrón oscuro

    painter.fillRect(0, alturaBase, width(), 50, tierraGradient);

    // Línea de horizonte
    painter.setPen(QPen(QColor(50, 50, 50), 2));
    painter.drawLine(0, alturaBase, width(), alturaBase);
}

void VisualizacionWidget::dibujarAtmosfera(QPainter& painter)
{
    if(!coheteActual) return;

    double altura = coheteActual->obtenerAltura();
    if(altura >= 100000) return; // Sin atmósfera visible

    int alturaBase = height() - 50;
    int alturaAtmosfera = 150;

    // Degradado de atmósfera
    QLinearGradient atmosferaGradient(0, alturaBase - alturaAtmosfera, 0, alturaBase);

    double opacidad = std::max(0.0, 1.0 - (altura / 100000.0));
    QColor colorAtmosfera(135, 206, 235, static_cast<int>(80 * opacidad));

    atmosferaGradient.setColorAt(0.0, QColor(135, 206, 235, 0));
    atmosferaGradient.setColorAt(0.5, colorAtmosfera);
    atmosferaGradient.setColorAt(1.0, QColor(100, 150, 200, static_cast<int>(120 * opacidad)));

    painter.fillRect(0, alturaBase - alturaAtmosfera, width(), alturaAtmosfera, atmosferaGradient);
}

void VisualizacionWidget::dibujarEstrellas(QPainter& painter)
{
    if(!coheteActual || coheteActual->obtenerAltura() < 30000) return;

    painter.setPen(Qt::white);
    painter.setBrush(Qt::white);

    // Dibujar estrellas en posiciones fijas (basadas en el frame para parpadeo)
    QRandomGenerator generator(12345); // Seed fija para posiciones constantes

    int numEstrellas = 100;
    for(int i = 0; i < numEstrellas; ++i) {
        int x = generator.bounded(width());
        int y = generator.bounded(height() - 100);

        // Parpadeo basado en el frame
        bool visible = ((frameAnimacion + i) % 30) > 5;
        if(visible) {
            int tamaño = generator.bounded(2) + 1;
            painter.drawEllipse(QPointF(x, y), tamaño, tamaño);
        }
    }
}

void VisualizacionWidget::dibujarCohete(QPainter& painter)
{
    if(!coheteActual) return;

    QPointF pos = posicionCohete;

    // Guardar estado del painter
    painter.save();

    // Color según estado
    QColor colorCohete = coheteActual->estaDanado() ?
                             QColor(200, 50, 50) : QColor(220, 220, 220);

    // Cuerpo del cohete
    painter.setPen(QPen(QColor(80, 80, 80), 2));
    painter.setBrush(colorCohete);

    QPolygonF cuerpoCohete;
    cuerpoCohete << QPointF(pos.x() - 8, pos.y() + 20)   // Base izquierda
                 << QPointF(pos.x() - 8, pos.y() - 10)   // Medio izquierdo
                 << QPointF(pos.x(), pos.y() - 25)       // Punta
                 << QPointF(pos.x() + 8, pos.y() - 10)   // Medio derecho
                 << QPointF(pos.x() + 8, pos.y() + 20);  // Base derecha

    painter.drawPolygon(cuerpoCohete);

    // Aletas
    QPolygonF aletaIzq;
    aletaIzq << QPointF(pos.x() - 8, pos.y() + 10)
             << QPointF(pos.x() - 15, pos.y() + 20)
             << QPointF(pos.x() - 8, pos.y() + 20);

    QPolygonF aletaDer;
    aletaDer << QPointF(pos.x() + 8, pos.y() + 10)
             << QPointF(pos.x() + 15, pos.y() + 20)
             << QPointF(pos.x() + 8, pos.y() + 20);

    painter.setBrush(QColor(180, 180, 180));
    painter.drawPolygon(aletaIzq);
    painter.drawPolygon(aletaDer);

    // Ventanas (si es tripulado)
    if(coheteActual->esTripulado()) {
        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(100, 200, 255));
        painter.drawEllipse(QPointF(pos.x(), pos.y() - 5), 3, 3);
    }

    // Indicador de empuje (barra al lado)
    if(coheteActual->obtenerEmpuje() > 0) {
        double porcentajeEmpuje = coheteActual->obtenerEmpuje() / 500000.0;
        int alturaBarra = static_cast<int>(40 * porcentajeEmpuje);

        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(233, 69, 96, 150));
        painter.drawRect(static_cast<int>(pos.x() + 20),
                         static_cast<int>(pos.y() + 20 - alturaBarra),
                         5, alturaBarra);
    }

    painter.restore();
}

void VisualizacionWidget::dibujarPropulsion(QPainter& painter, const QPointF& posicion)
{
    if(!coheteActual || coheteActual->obtenerEmpuje() <= 0) return;

    double intensidad = coheteActual->obtenerEmpuje() / 500000.0;
    int longitudLlama = static_cast<int>(20 + 30 * intensidad);

    // Gradiente de la llama
    QLinearGradient llamaGradient(posicion.x(), posicion.y() + 20,
                                  posicion.x(), posicion.y() + 20 + longitudLlama);

    llamaGradient.setColorAt(0.0, QColor(255, 255, 200, 200));
    llamaGradient.setColorAt(0.3, QColor(255, 150, 0, 180));
    llamaGradient.setColorAt(0.7, QColor(255, 50, 0, 100));
    llamaGradient.setColorAt(1.0, QColor(200, 0, 0, 0));

    painter.setPen(Qt::NoPen);
    painter.setBrush(llamaGradient);

    // Dibujar llama con animación
    int anchoBase = 12;
    int variacion = (frameAnimacion % 4) - 2;

    QPolygonF llama;
    llama << QPointF(posicion.x() - anchoBase, posicion.y() + 20)
          << QPointF(posicion.x() + anchoBase, posicion.y() + 20)
          << QPointF(posicion.x() + variacion, posicion.y() + 20 + longitudLlama);

    painter.drawPolygon(llama);
}

void VisualizacionWidget::dibujarParticulas(QPainter& painter)
{
    if(!coheteActual || coheteActual->obtenerEmpuje() <= 0) return;

    painter.setPen(Qt::NoPen);

    for(int i = 0; i < particulasPropulsion.size(); ++i) {
        QPointF particula = particulasPropulsion[i];
        if(particula.y() == 0) continue;

        double opacidad = 1.0 - (static_cast<double>(i) / particulasPropulsion.size());
        QColor colorParticula(255, 100 + i * 5, 0, static_cast<int>(150 * opacidad));

        painter.setBrush(colorParticula);
        painter.drawEllipse(particula, 3, 3);
    }
}

void VisualizacionWidget::dibujarIndicadores(QPainter& painter)
{
    if(!coheteActual) return;

    // Indicador de velocidad con color
    double velocidad = coheteActual->obtenerVelocidad();
    QColor colorVel;

    if(numeroNivel == 1 && velocidad > 7000) {
        colorVel = QColor(255, 0, 0); // Peligro
    } else if(numeroNivel == 2 && (velocidad < 2000 || velocidad > 9000)) {
        colorVel = QColor(255, 165, 0); // Advertencia
    } else {
        colorVel = QColor(0, 255, 0); // OK
    }

    // Pequeño indicador en la esquina
    painter.setPen(QPen(colorVel, 3));
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(10, 10, 20, 20);
}

void VisualizacionWidget::dibujarLineaObjetivo(QPainter& painter)
{
    if(!nivelActual) return;

    double alturaObjetivo = nivelActual->obtenerAlturaObjetivo();
    double yObjetivo = alturaAPixel(alturaObjetivo);

    if(yObjetivo >= 0 && yObjetivo <= height()) {
        painter.setPen(QPen(QColor(0, 255, 0, 150), 2, Qt::DashLine));
        painter.drawLine(0, static_cast<int>(yObjetivo),
                         width(), static_cast<int>(yObjetivo));

        // Etiqueta
        painter.setPen(QColor(0, 255, 0));
        painter.setFont(QFont("Arial", 10, QFont::Bold));
        QString texto = QString("OBJETIVO: %1 km").arg(alturaObjetivo / 1000.0, 0, 'f', 0);
        painter.drawText(10, static_cast<int>(yObjetivo) - 5, texto);
    }
}

void VisualizacionWidget::dibujarMarcadoresAltura(QPainter& painter)
{
    painter.setPen(QColor(150, 150, 150, 100));
    painter.setFont(QFont("Arial", 9));

    double intervalo = 50000.0; // Cada 50 km
    if(numeroNivel == 3) intervalo = 5000.0; // Cada 5 km para la Luna

    for(double alt = 0; alt <= alturaMaximaVista; alt += intervalo) {
        double y = alturaAPixel(alt);
        if(y >= 0 && y <= height()) {
            painter.drawLine(width() - 50, static_cast<int>(y),
                             width() - 10, static_cast<int>(y));

            QString etiqueta = QString("%1").arg(alt / 1000.0, 0, 'f', 0);
            painter.drawText(width() - 45, static_cast<int>(y) - 5, etiqueta + " km");
        }
    }
}

void VisualizacionWidget::dibujarLuna(QPainter& painter)
{
    // Superficie lunar
    int alturaSuperficie = height() - 50;

    QLinearGradient lunaGradient(0, alturaSuperficie - 100, 0, alturaSuperficie);
    lunaGradient.setColorAt(0.0, QColor(120, 120, 120));
    lunaGradient.setColorAt(1.0, QColor(80, 80, 80));

    painter.fillRect(0, alturaSuperficie, width(), 50, lunaGradient);

    // Cráteres
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(60, 60, 60));

    QRandomGenerator generator(54321);
    for(int i = 0; i < 15; ++i) {
        int x = generator.bounded(width());
        int radio = generator.bounded(10) + 5;
        painter.drawEllipse(QPoint(x, alturaSuperficie + 10), radio, radio / 2);
    }
}

void VisualizacionWidget::calcularPosicionCohete()
{
    if(!coheteActual) return;

    double altura = coheteActual->obtenerAltura();

    // Calcular posición Y (invertida, 0 arriba)
    double y = alturaAPixel(altura);

    // Posición X (centrada)
    double x = width() / 2.0;

    posicionCohete = QPointF(x, y);
}

void VisualizacionWidget::calcularEscalaAltura()
{
    if(!nivelActual) return;

    switch(numeroNivel) {
    case 1:
        alturaMaximaVista = 150000.0;  // 150 km
        break;
    case 2:
        alturaMaximaVista = 350000.0;  // 350 km
        break;
    case 3:
        alturaMaximaVista = 20000.0;   // 20 km (Luna)
        break;
    default:
        alturaMaximaVista = 150000.0;
    }

    escalaAltura = (height() - 100.0) / alturaMaximaVista;
}

double VisualizacionWidget::alturaAPixel(double altura) const
{
    // Convertir altura real a píxeles en pantalla
    // 0 metros = parte inferior, altura máxima = parte superior
    double alturaBase = height() - 50;
    return alturaBase - (altura * escalaAltura);
}

QColor VisualizacionWidget::obtenerColorPorAltura(double altura) const
{
    if(altura < 10000) return QColor(100, 150, 200);      // Atmósfera baja
    if(altura < 50000) return QColor(50, 80, 150);        // Atmósfera media
    if(altura < 100000) return QColor(20, 30, 80);        // Atmósfera alta
    return QColor(10, 10, 30);                             // Espacio
}
