#include "visualizacionwidget.h"
#include <QPainter>
#include <QLinearGradient>
#include <QPaintEvent>
#include <QRandomGenerator>
#include <QDir>
#include <QFileInfo>
#include <QCoreApplication>
#include <cmath>

VisualizacionWidget::VisualizacionWidget(QWidget *parent)
    : QWidget(parent),
    coheteActual(nullptr),
    nivelActual(nullptr),
    numeroNivel(0),
    frameAnimacion(0),
    animacionActiva(false),
    escalaAltura(1.0),
    alturaMaximaVista(150000.0),
    spritesCargados(false),
    numFramesX(3),
    numFramesY(3),
    mostrarExplosion(false)
{
    setMinimumSize(600, 600);

    timerAnimacion = new QTimer(this);
    connect(timerAnimacion, &QTimer::timeout, this, &VisualizacionWidget::actualizarAnimacion);

    for(int i = 0; i < 20; ++i) {
        particulasPropulsion.append(QPointF(0, 0));
    }

    cargarSprites();
    dividirSpriteSheet();
}

VisualizacionWidget::~VisualizacionWidget()
{
    detenerAnimacion();
}

void VisualizacionWidget::actualizarCohete(const Cohete* cohete)
{
    coheteActual = cohete;
    if(cohete) {
        calcularPosicionCohete();
        
        if(numeroNivel == 2 && cohete->obtenerVelocidad() > 11000.0) {
            mostrarExplosion = true;
        } else if(cohete->estaDanado() && cohete->obtenerVelocidad() > 10000.0) {
            mostrarExplosion = true;
        } else {
            mostrarExplosion = false;
        }
        
        update();
    }
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
    timerAnimacion->start(50);
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
        if(mostrarExplosion && !spriteExplosion.isNull()) {
            dibujarExplosion(painter);
        } else if(!mostrarExplosion) {
            dibujarCohete(painter);
            if(coheteActual->obtenerEmpuje() > 0) {
                dibujarPropulsion(painter, posicionCohete);
                dibujarParticulas(painter);
            }
        }
    }

    dibujarLineaObjetivo(painter);
}

void VisualizacionWidget::dibujarFondo(QPainter& painter)
{
    if(numeroNivel == 1 && spritesCargados && !spriteFondo.isNull()) {
        QPixmap fondoEscalado = spriteFondo.scaled(width(), height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        painter.drawPixmap(0, 0, fondoEscalado);
    } else if(numeroNivel == 2 && spritesCargados && !spriteFondo2.isNull()) {
        QPixmap fondoEscalado = spriteFondo2.scaled(width(), height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        painter.drawPixmap(0, 0, fondoEscalado);
    } else if(numeroNivel == 3 && spritesCargados && !spriteFondo3.isNull()) {
        QPixmap fondoEscalado = spriteFondo3.scaled(width(), height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        painter.drawPixmap(0, 0, fondoEscalado);
    } else {
        QLinearGradient gradient(0, 0, 0, height());

        if(numeroNivel == 3) {
            gradient.setColorAt(0.0, QColor(10, 10, 20));
            gradient.setColorAt(1.0, QColor(5, 5, 15));
        } else if(coheteActual && coheteActual->obtenerAltura() < 50000) {
            gradient.setColorAt(0.0, QColor(10, 10, 30));
            gradient.setColorAt(0.3, QColor(20, 30, 60));
            gradient.setColorAt(1.0, QColor(100, 150, 200));
        } else if(coheteActual && coheteActual->obtenerAltura() < 100000) {
            gradient.setColorAt(0.0, QColor(5, 5, 15));
            gradient.setColorAt(0.5, QColor(10, 20, 40));
            gradient.setColorAt(1.0, QColor(50, 80, 120));
        } else {
            gradient.setColorAt(0.0, QColor(5, 5, 15));
            gradient.setColorAt(1.0, QColor(10, 10, 25));
        }

        painter.fillRect(rect(), gradient);
    }
}

void VisualizacionWidget::dibujarTierra(QPainter& painter)
{
    if(numeroNivel == 2) {
        return;
    }
    
    int alturaBase = height() - 50;

    QLinearGradient tierraGradient(0, alturaBase - 100, 0, alturaBase);
    tierraGradient.setColorAt(0.0, QColor(34, 139, 34));
    tierraGradient.setColorAt(0.5, QColor(101, 67, 33));
    tierraGradient.setColorAt(1.0, QColor(139, 90, 43));

    painter.fillRect(0, alturaBase, width(), 50, tierraGradient);

    painter.setPen(QPen(QColor(50, 50, 50), 2));
    painter.drawLine(0, alturaBase, width(), alturaBase);
}

void VisualizacionWidget::dibujarAtmosfera(QPainter& painter)
{
    if(!coheteActual) return;

    double altura = coheteActual->obtenerAltura();
    if(altura >= 100000) return;

    int alturaBase = height() - 50;
    int alturaAtmosfera = 150;

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

    QRandomGenerator generator(12345);

    int numEstrellas = 100;
    for(int i = 0; i < numEstrellas; ++i) {
        int x = generator.bounded(width());
        int y = generator.bounded(height() - 100);

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

    painter.save();

    // Usar sprites para todos los niveles (1, 2 y 3)
    if(spritesCargados && !framesCohete.isEmpty()) {
        int frameIndex = 0;

        if(numeroNivel == 1) {
            double altura = coheteActual->obtenerAltura();
            double velocidad = coheteActual->obtenerVelocidad();
            double empuje = coheteActual->obtenerEmpuje();

            if(empuje <= 0.0) {
                frameIndex = 0;
            } else {
                double alturaMaxima = 150000.0;
                double velocidadMaxima = 10000.0;

                double porcentajeAltura = std::min(1.0, altura / alturaMaxima);
                double porcentajeVelocidad = std::min(1.0, velocidad / velocidadMaxima);

                double progreso = (porcentajeAltura * 0.4 + porcentajeVelocidad * 0.6);

                int totalFrames = framesCohete.size();
                frameIndex = 1 + static_cast<int>(progreso * (totalFrames - 2));

                if(frameIndex < 1) frameIndex = 1;
                if(frameIndex >= totalFrames) frameIndex = totalFrames - 1;
            }
        } else if(numeroNivel == 2 || numeroNivel == 3) {
            // Usar empuje para determinar el frame tanto en nivel 2 como en nivel 3
            double empuje = coheteActual->obtenerEmpuje();
            double empujeMaximo = 500000.0;
            frameIndex = obtenerFrameSegunEmpuje(empuje, empujeMaximo);
        }

        if(frameIndex < 0) frameIndex = 0;
        if(frameIndex >= framesCohete.size()) frameIndex = framesCohete.size() - 1;

        QPixmap frameActual = framesCohete[frameIndex];

        int anchoCohete = 50;
        int altoCohete = 80;
        QPixmap coheteEscalado = frameActual.scaled(anchoCohete, altoCohete, Qt::KeepAspectRatio, Qt::SmoothTransformation);

        QRectF rectCohete(pos.x() - anchoCohete/2, pos.y() - altoCohete/2, anchoCohete, altoCohete);
        painter.drawPixmap(rectCohete.toRect(), coheteEscalado);

        if(coheteActual->estaDanado()) {
            painter.setCompositionMode(QPainter::CompositionMode_SourceAtop);
            painter.fillRect(rectCohete, QColor(255, 0, 0, 100));
            painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
        }
    } else {
        // Fallback: dibujo vectorial si no hay sprites
        QColor colorCohete = coheteActual->estaDanado() ?
                                 QColor(200, 50, 50) : QColor(220, 220, 220);

        painter.setPen(QPen(QColor(80, 80, 80), 2));
        painter.setBrush(colorCohete);

        QPolygonF cuerpoCohete;
        cuerpoCohete << QPointF(pos.x() - 8, pos.y() + 20)
                     << QPointF(pos.x() - 8, pos.y() - 10)
                     << QPointF(pos.x(), pos.y() - 25)
                     << QPointF(pos.x() + 8, pos.y() - 10)
                     << QPointF(pos.x() + 8, pos.y() + 20);

        painter.drawPolygon(cuerpoCohete);

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

        if(coheteActual->esTripulado()) {
            painter.setPen(Qt::NoPen);
            painter.setBrush(QColor(100, 200, 255));
            painter.drawEllipse(QPointF(pos.x(), pos.y() - 5), 3, 3);
        }
    }

    // Indicador de empuje (barra lateral)
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

    QLinearGradient llamaGradient(posicion.x(), posicion.y() + 20,
                                  posicion.x(), posicion.y() + 20 + longitudLlama);

    llamaGradient.setColorAt(0.0, QColor(255, 255, 200, 200));
    llamaGradient.setColorAt(0.3, QColor(255, 150, 0, 180));
    llamaGradient.setColorAt(0.7, QColor(255, 50, 0, 100));
    llamaGradient.setColorAt(1.0, QColor(200, 0, 0, 0));

    painter.setPen(Qt::NoPen);
    painter.setBrush(llamaGradient);

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

    double velocidad = coheteActual->obtenerVelocidad();
    QColor colorVel;

    if(numeroNivel == 1 && velocidad > 7000) {
        colorVel = QColor(255, 0, 0);
    } else if(numeroNivel == 2 && (velocidad < 2000 || velocidad > 9000)) {
        colorVel = QColor(255, 165, 0);
    } else {
        colorVel = QColor(0, 255, 0);
    }

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

    double intervalo = 50000.0;
    if(numeroNivel == 3) intervalo = 5000.0;

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
    int alturaSuperficie = height() - 50;

    QLinearGradient lunaGradient(0, alturaSuperficie - 100, 0, alturaSuperficie);
    lunaGradient.setColorAt(0.0, QColor(120, 120, 120));
    lunaGradient.setColorAt(1.0, QColor(80, 80, 80));

    painter.fillRect(0, alturaSuperficie, width(), 50, lunaGradient);

    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(60, 60, 60));

    QRandomGenerator generator(54321);
    for(int i = 0; i < 15; ++i) {
        int x = generator.bounded(width());
        int radio = generator.bounded(10) + 5;
        painter.drawEllipse(QPoint(x, alturaSuperficie + 10), radio, radio / 2);
    }
}

void VisualizacionWidget::dibujarExplosion(QPainter& painter)
{
    if(!coheteActual || spriteExplosion.isNull()) return;
    
    QPointF pos = posicionCohete;
    
    int anchoExplosion = 100;
    int altoExplosion = 100;
    QPixmap explosionEscalada = spriteExplosion.scaled(anchoExplosion, altoExplosion, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    
    QRectF rectExplosion(pos.x() - anchoExplosion/2, pos.y() - altoExplosion/2, anchoExplosion, altoExplosion);
    painter.drawPixmap(rectExplosion.toRect(), explosionEscalada);
}

void VisualizacionWidget::calcularPosicionCohete()
{
    if(!coheteActual) return;

    double altura = coheteActual->obtenerAltura();

    double y = alturaAPixel(altura);

    double x = width() / 2.0;

    posicionCohete = QPointF(x, y);
}

void VisualizacionWidget::calcularEscalaAltura()
{
    if(!nivelActual) return;

    switch(numeroNivel) {
    case 1:
        alturaMaximaVista = 150000.0;
        break;
    case 2:
        alturaMaximaVista = 250000.0;
        break;
    case 3:
        alturaMaximaVista = 20000.0;
        break;
    default:
        alturaMaximaVista = 150000.0;
    }

    escalaAltura = (height() - 100.0) / alturaMaximaVista;
}

double VisualizacionWidget::alturaAPixel(double altura) const
{
    double alturaBase = height() - 50;
    return alturaBase - (altura * escalaAltura);
}

void VisualizacionWidget::cargarSprites()
{
    QDir directorioActual = QDir::current();
    QDir dirEjecucion = QDir(QCoreApplication::applicationDirPath());
    
    QStringList rutasPosibles;
    
    rutasPosibles << "../Sprites"
                  << "../../Sprites"
                  << "../../../Sprites"
                  << "Sprites"
                  << directorioActual.absolutePath() + "/../Sprites"
                  << directorioActual.absolutePath() + "/../../Sprites"
                  << directorioActual.absolutePath() + "/../../../Sprites";
    
    rutasPosibles << dirEjecucion.absoluteFilePath("../Sprites")
                  << dirEjecucion.absoluteFilePath("../../Sprites")
                  << dirEjecucion.absoluteFilePath("../../../Sprites")
                  << dirEjecucion.absoluteFilePath("../../../../Sprites");
    
    QString rutaProyecto = dirEjecucion.absolutePath();
    if(rutaProyecto.contains("build") || rutaProyecto.contains("debug") || rutaProyecto.contains("release")) {
        QDir dirDesdeBuild = QDir(rutaProyecto);
        if(dirDesdeBuild.cdUp()) {
            if(dirDesdeBuild.cdUp()) {
                if(dirDesdeBuild.cdUp()) {
                    rutasPosibles << dirDesdeBuild.absoluteFilePath("Sprites");
                }
            }
        }
    }
    
    QString rutaWorkspace = QDir::homePath() + "/OneDrive/Documentos/DesafioFinal/Sprites";
    rutasPosibles << rutaWorkspace;
    
    QDir dirTemp = dirEjecucion;
    for(int i = 0; i < 5 && dirTemp.cdUp(); ++i) {
        QString rutaSpritesTemp = dirTemp.absoluteFilePath("Sprites");
        if(QDir(rutaSpritesTemp).exists()) {
            rutasPosibles << rutaSpritesTemp;
        }
    }
    
    QString rutaSprites;
    for(const QString& ruta : rutasPosibles) {
        QFileInfo infoCohete(ruta + "/cohete.png");
        if(infoCohete.exists() && infoCohete.isFile()) {
            rutaSprites = ruta;
            break;
        }
    }
    
    if(!rutaSprites.isEmpty()) {
        QString rutaCohete = QDir(rutaSprites).absoluteFilePath("cohete.png");
        QString rutaFondo = QDir(rutaSprites).absoluteFilePath("fondo.png");
        QString rutaFondo2 = QDir(rutaSprites).absoluteFilePath("fondo2.png");
        
        if(QFileInfo::exists(rutaCohete)) {
            spriteCohete = QPixmap(rutaCohete);
            if(spriteCohete.isNull()) {
                spriteCohete.load(rutaCohete);
            }
        }
        
        if(QFileInfo::exists(rutaFondo)) {
            spriteFondo = QPixmap(rutaFondo);
            if(spriteFondo.isNull()) {
                spriteFondo.load(rutaFondo);
            }
        }
        
        if(QFileInfo::exists(rutaFondo2)) {
            spriteFondo2 = QPixmap(rutaFondo2);
            if(spriteFondo2.isNull()) {
                spriteFondo2.load(rutaFondo2);
            }
        }
        
        QString rutaFondo3 = QDir(rutaSprites).absoluteFilePath("fondo3.png");
        if(QFileInfo::exists(rutaFondo3)) {
            spriteFondo3 = QPixmap(rutaFondo3);
            if(spriteFondo3.isNull()) {
                spriteFondo3.load(rutaFondo3);
            }
        }
        
        QString rutaExplosion = QDir(rutaSprites).absoluteFilePath("explosioncohete.png");
        if(QFileInfo::exists(rutaExplosion)) {
            spriteExplosion = QPixmap(rutaExplosion);
            if(spriteExplosion.isNull()) {
                spriteExplosion.load(rutaExplosion);
            }
        }
        
        spritesCargados = !spriteCohete.isNull() || !spriteFondo.isNull() || !spriteFondo2.isNull() || !spriteFondo3.isNull();
    }
}

void VisualizacionWidget::dividirSpriteSheet()
{
    framesCohete.clear();
    
    if(spriteCohete.isNull()) {
        return;
    }
    
    int anchoSheet = spriteCohete.width();
    int altoSheet = spriteCohete.height();
    
    int anchoFrame = anchoSheet / numFramesX;
    int altoFrame = altoSheet / numFramesY;
    
    for(int fila = 0; fila < numFramesY; ++fila) {
        for(int columna = 0; columna < numFramesX; ++columna) {
            int x = columna * anchoFrame;
            int y = fila * altoFrame;
            
            QPixmap frame = spriteCohete.copy(x, y, anchoFrame, altoFrame);
            framesCohete.append(frame);
        }
    }
}

int VisualizacionWidget::obtenerFrameSegunEmpuje(double empuje, double empujeMaximo) const
{
    if(empuje <= 0.0 || framesCohete.isEmpty()) {
        return 0;
    }
    
    double porcentajeEmpuje = std::min(1.0, empuje / empujeMaximo);
    
    int totalFrames = framesCohete.size();
    int frameIndex = static_cast<int>(porcentajeEmpuje * (totalFrames - 1));
    
    if(frameIndex < 0) frameIndex = 0;
    if(frameIndex >= totalFrames) frameIndex = totalFrames - 1;
    
    return frameIndex;
}
