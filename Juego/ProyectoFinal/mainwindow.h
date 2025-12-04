#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <memory>
#include "Juego.h"
#include "visualizacionwidget.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // Slots para los botones de nivel
    void on_btnNivel1_clicked();
    void on_btnNivel2_clicked();
    void on_btnNivel3_clicked();

    // Slots para controles
    void on_btnIniciar_clicked();
    void on_btnPausar_clicked();
    void on_btnReiniciar_clicked();
    void on_sliderEmpuje_valueChanged(int value);
    void on_spinVelocidadInicial_valueChanged(double value);

    // Slot para actualización
    void actualizarJuego();
    void actualizarInterfaz();

private:
    Ui::MainWindow *ui;

    // Componentes del juego
    std::unique_ptr<Juego> juego;
    QTimer* timerJuego;

    // Widget de visualización
    VisualizacionWidget* widgetVisualizacion;

    // Métodos auxiliares
    void inicializarJuego();
    void inicializarWidgetVisualizacion();
    void actualizarTelemetria();
    void actualizarEstadoLabel();
    void agregarMensajeHAL(const QString& mensaje);
    void verificarEstadoJuego();
    void mostrarMensajeVictoria();
    void mostrarMensajeDerrota();
    void habilitarControles(bool habilitar);
};

#endif // MAINWINDOW_H
