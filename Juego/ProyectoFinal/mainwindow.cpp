#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QVBoxLayout>
#include <sstream>
#include <iomanip>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Inicializar el juego
    inicializarJuego();

    // Crear y configurar el widget de visualización
    inicializarWidgetVisualizacion();

    // Timer para actualización del juego
    timerJuego = new QTimer(this);
    connect(timerJuego, &QTimer::timeout, this, &MainWindow::actualizarJuego);

    // Configuración inicial de controles
    ui->sliderEmpuje->setValue(0);
    ui->btnPausar->setEnabled(false);
    ui->btnReiniciar->setEnabled(false);
    
    // Inicialmente deshabilitar campo de velocidad inicial hasta que se seleccione nivel 1
    ui->spinVelocidadInicial->setEnabled(false);
    ui->labelVelocidadInicial->setEnabled(false);

    // Mensaje inicial de HAL
    agregarMensajeHAL("HAL-69: Sistemas iniciados. Selecciona un nivel para comenzar.");
}

MainWindow::~MainWindow()
{
    if(timerJuego->isActive()) {
        timerJuego->stop();
    }
    delete ui;
}

void MainWindow::inicializarJuego()
{
    juego = std::make_unique<Juego>();
}

void MainWindow::inicializarWidgetVisualizacion()
{
    // Crear el widget de visualización
    widgetVisualizacion = new VisualizacionWidget(this);

    // Reemplazar el widget placeholder con nuestro widget personalizado
    QVBoxLayout* layout = new QVBoxLayout(ui->widgetVisualizacion);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(widgetVisualizacion);
}

// ============================================================================
// SLOTS DE BOTONES DE NIVEL
// ============================================================================

void MainWindow::on_btnNivel1_clicked()
{
    juego->iniciarNivel(1);

    // Establecer velocidad inicial desde el campo de entrada
    double velocidadInicial = ui->spinVelocidadInicial->value();
    juego->establecerVelocidadInicial(velocidadInicial);

    ui->labelNombreNivel->setText("SPUTNIK 1957");
    ui->labelDescripcionNivel->setText(
        QString::fromStdString(juego->obtenerNivel()->obtenerDescripcion())
        );

    widgetVisualizacion->actualizarNivel(juego->obtenerNivel(), 1);
    widgetVisualizacion->actualizarCohete(juego->obtenerCohete());

    // Habilitar campo de velocidad inicial solo para nivel 1
    ui->spinVelocidadInicial->setEnabled(true);
    ui->labelVelocidadInicial->setEnabled(true);

    agregarMensajeHAL(QString("HAL-69: Nivel 1 cargado. Velocidad inicial: %1 m/s. Objetivo: Alcanzar 100 km de altura.")
                      .arg(velocidadInicial, 0, 'f', 1));

    ui->btnIniciar->setEnabled(true);
    ui->btnReiniciar->setEnabled(true);
    actualizarTelemetria();
}

void MainWindow::on_btnNivel2_clicked()
{
    juego->iniciarNivel(2);

    ui->labelNombreNivel->setText("VOSTOK 1 - 1961");
    ui->labelDescripcionNivel->setText(
        QString::fromStdString(juego->obtenerNivel()->obtenerDescripcion())
        );

    widgetVisualizacion->actualizarNivel(juego->obtenerNivel(), 2);
    widgetVisualizacion->actualizarCohete(juego->obtenerCohete());

    // Deshabilitar campo de velocidad inicial para otros niveles
    ui->spinVelocidadInicial->setEnabled(false);
    ui->labelVelocidadInicial->setEnabled(false);

    agregarMensajeHAL("HAL-69: Nivel 2 cargado. Mantén velocidad entre 2000-9000 m/s.");

    ui->btnIniciar->setEnabled(true);
    ui->btnReiniciar->setEnabled(true);
    actualizarTelemetria();
}

void MainWindow::on_btnNivel3_clicked()
{
    juego->iniciarNivel(3);

    ui->labelNombreNivel->setText("APOLO 11 - 1969");
    ui->labelDescripcionNivel->setText(
        QString::fromStdString(juego->obtenerNivel()->obtenerDescripcion())
        );

    widgetVisualizacion->actualizarNivel(juego->obtenerNivel(), 3);
    widgetVisualizacion->actualizarCohete(juego->obtenerCohete());

    // Deshabilitar campo de velocidad inicial para otros niveles
    ui->spinVelocidadInicial->setEnabled(false);
    ui->labelVelocidadInicial->setEnabled(false);

    agregarMensajeHAL("HAL-69: Nivel 3 cargado. Alunizaje: velocidad máxima 5 m/s.");

    ui->btnIniciar->setEnabled(true);
    ui->btnReiniciar->setEnabled(true);
    actualizarTelemetria();
}

// ============================================================================
// SLOTS DE CONTROLES
// ============================================================================

void MainWindow::on_btnIniciar_clicked()
{
    if(!juego->estaEnEjecucion() && juego->obtenerNivelActual() > 0) {
        // Iniciar simulación en el juego
        juego->iniciarSimulacion();
        
        // Iniciar timer y animación
        timerJuego->start(100); // Actualizar cada 100ms
        widgetVisualizacion->iniciarAnimacion();

        ui->btnIniciar->setEnabled(false);
        ui->btnPausar->setEnabled(true);
        ui->btnPausar->setText("⏸ PAUSAR");

        agregarMensajeHAL("HAL-69: Simulación iniciada. ¡Buena suerte!");

        ui->labelEstado->setText("🟢 EN PROGRESO");
        ui->labelEstado->setStyleSheet(
            "font-size: 14px; font-weight: bold; color: #00ff00; "
            "padding: 10px; background-color: #16213e; border-radius: 5px;"
            );
    }
}

void MainWindow::on_btnPausar_clicked()
{
    if(juego->estaPausado()) {
        // Reanudar
        juego->reanudar();
        timerJuego->start(100);
        widgetVisualizacion->iniciarAnimacion();

        ui->btnPausar->setText("⏸ PAUSAR");
        agregarMensajeHAL("HAL-69: Simulación reanudada.");

        ui->labelEstado->setText("🟢 EN PROGRESO");
    } else {
        // Pausar
        juego->pausar();
        timerJuego->stop();
        widgetVisualizacion->detenerAnimacion();
        
        // Limpiar teclas presionadas al pausar
        teclasPresionadas.clear();

        ui->btnPausar->setText("▶ REANUDAR");
        agregarMensajeHAL("HAL-69: Simulación pausada.");

        ui->labelEstado->setText("🟡 PAUSADO");
        ui->labelEstado->setStyleSheet(
            "font-size: 14px; font-weight: bold; color: #ffaa00; "
            "padding: 10px; background-color: #16213e; border-radius: 5px;"
            );
    }
}

void MainWindow::on_btnReiniciar_clicked()
{
    timerJuego->stop();
    widgetVisualizacion->detenerAnimacion();
    
    // Limpiar teclas presionadas al reiniciar
    teclasPresionadas.clear();

    juego->reiniciarNivel();

    // Reestablecer velocidad inicial si es el nivel 1
    if(juego->obtenerNivelActual() == 1) {
        double velocidadInicial = ui->spinVelocidadInicial->value();
        juego->establecerVelocidadInicial(velocidadInicial);
    }

    widgetVisualizacion->reiniciar();
    widgetVisualizacion->actualizarCohete(juego->obtenerCohete());

    ui->sliderEmpuje->setValue(0);
    ui->btnIniciar->setEnabled(true);
    ui->btnPausar->setEnabled(false);
    ui->btnPausar->setText("⏸ PAUSAR");

    agregarMensajeHAL("HAL-69: Nivel reiniciado. Sistemas listos.");

    ui->labelEstado->setText("⚪ LISTO");
    ui->labelEstado->setStyleSheet(
        "font-size: 14px; font-weight: bold; color: #aaa; "
        "padding: 10px; background-color: #16213e; border-radius: 5px;"
        );

    actualizarTelemetria();
}

void MainWindow::on_sliderEmpuje_valueChanged(int value)
{
    juego->ajustarEmpuje(static_cast<double>(value));

    ui->labelEmpujeValor->setText(QString("%1 N").arg(value));
}

void MainWindow::on_spinVelocidadInicial_valueChanged(double value)
{
    // Si el nivel 1 está cargado pero no iniciado, actualizar la velocidad inicial
    if(juego->obtenerNivelActual() == 1 && !juego->estaEnEjecucion()) {
        juego->establecerVelocidadInicial(value);
        widgetVisualizacion->actualizarCohete(juego->obtenerCohete());
    }
}

// ============================================================================
// ACTUALIZACIÓN DEL JUEGO
// ============================================================================

void MainWindow::actualizarJuego()
{
    if(!juego->estaEnEjecucion() || juego->estaPausado()) {
        return;
    }

    // Procesar movimiento con teclado para nivel 3
    if(juego->obtenerNivelActual() == 3) {
        // El timer se ejecuta cada 100ms, así que ajustamos la velocidad por frame
        // para un movimiento más suave y controlable
        double aceleracionHorizontal = 3.0; // m/s² por frame (ajustado para movimiento suave)
        
        if(teclasPresionadas.contains(Qt::Key_Left)) {
            juego->moverCoheteHorizontal(-aceleracionHorizontal);
        }
        if(teclasPresionadas.contains(Qt::Key_Right)) {
            juego->moverCoheteHorizontal(aceleracionHorizontal);
        }
        if(teclasPresionadas.contains(Qt::Key_Up)) {
            // Aumentar empuje gradualmente
            double empujeActual = juego->obtenerCohete()->obtenerEmpuje();
            double nuevoEmpuje = empujeActual + 5000.0;
            juego->ajustarEmpuje(nuevoEmpuje);
            ui->sliderEmpuje->setValue(static_cast<int>(nuevoEmpuje));
        }
        if(teclasPresionadas.contains(Qt::Key_Down)) {
            // Disminuir empuje gradualmente
            double empujeActual = juego->obtenerCohete()->obtenerEmpuje();
            double nuevoEmpuje = empujeActual - 5000.0;
            juego->ajustarEmpuje(nuevoEmpuje);
            ui->sliderEmpuje->setValue(static_cast<int>(nuevoEmpuje));
        }
    }

    // Actualizar la simulación
    juego->actualizar();

    // Actualizar la interfaz
    actualizarInterfaz();
    
    // Obtener mensaje de HAL69 y mostrarlo si hay algo importante
    std::string mensajeHAL = juego->obtenerAgenteHAL()->obtenerMensajeUI(
        juego->obtenerNivelActual(),
        *juego->obtenerNivel(),
        *juego->obtenerCohete(),
        juego->obtenerTiempoSimulacion()
    );
    
    // Solo mostrar mensajes importantes (no vacíos) y no repetirlos constantemente
    static std::string ultimoMensajeHAL = "";
    static int contadorMensajes = 0;
    
    if(!mensajeHAL.empty() && mensajeHAL != ultimoMensajeHAL) {
        // Mostrar mensaje cada 2 segundos aproximadamente (cada 20 actualizaciones)
        if(contadorMensajes % 20 == 0) {
            agregarMensajeHAL(QString::fromStdString(mensajeHAL));
            ultimoMensajeHAL = mensajeHAL;
        }
        contadorMensajes++;
    } else if(mensajeHAL.empty()) {
        ultimoMensajeHAL = "";
        contadorMensajes = 0;
    }

    // Verificar estado (victoria/derrota)
    verificarEstadoJuego();
}

void MainWindow::actualizarInterfaz()
{
    // Actualizar telemetría
    actualizarTelemetria();

    // Actualizar visualización
    widgetVisualizacion->actualizarCohete(juego->obtenerCohete());
}

void MainWindow::actualizarTelemetria()
{
    const Cohete* cohete = juego->obtenerCohete();
    if(!cohete) return;

    // Tiempo
    ui->labelTiempo->setText(QString("%1 s").arg(
        juego->obtenerTiempoSimulacion(), 0, 'f', 1
        ));

    // Altura
    double altura = cohete->obtenerAltura() / 1000.0; // Convertir a km
    ui->labelAltura->setText(QString("%1 km").arg(altura, 0, 'f', 2));

    // Velocidad
    double velocidad = cohete->obtenerVelocidad();
    ui->labelVelocidad->setText(QString("%1 m/s").arg(velocidad, 0, 'f', 1));

    // Cambiar color según velocidad peligrosa
    QString colorVel = "#e94560"; // Color normal
    if(juego->obtenerNivelActual() == 1 && velocidad > 7000) {
        colorVel = "#ff0000"; // Rojo peligro
    } else if(juego->obtenerNivelActual() == 2 &&
               (velocidad < 2000 || velocidad > 9000)) {
        colorVel = "#ff6600"; // Naranja advertencia
    }
    ui->labelVelocidad->setStyleSheet(
        QString("font-size: 16px; font-weight: bold; color: %1;").arg(colorVel)
        );

    // Combustible
    double porcentajeCombustible = cohete->obtenerPorcentajeCombustible();
    ui->progressCombustible->setValue(static_cast<int>(porcentajeCombustible));

    // Cambiar color de la barra según nivel
    QString colorBarra = "#e94560"; // Normal
    if(porcentajeCombustible < 20) {
        colorBarra = "#ff0000"; // Rojo crítico
    } else if(porcentajeCombustible < 40) {
        colorBarra = "#ff9900"; // Naranja bajo
    }

    ui->progressCombustible->setStyleSheet(
        QString("QProgressBar { border: 2px solid #16213e; border-radius: 5px; "
                "text-align: center; background-color: #0f3460; color: white; } "
                "QProgressBar::chunk { background-color: %1; border-radius: 3px; }")
            .arg(colorBarra)
        );

    // Masa
    ui->labelMasa->setText(QString("%1 kg").arg(
        cohete->obtenerMasa(), 0, 'f', 0
        ));
}

void MainWindow::verificarEstadoJuego()
{
    if(juego->haGanado()) {
        timerJuego->stop();
        widgetVisualizacion->detenerAnimacion();
        mostrarMensajeVictoria();
    } else if(juego->haPerdido()) {
        // Detener la simulación pero mantener la animación para mostrar la explosión
        timerJuego->stop();
        // NO detener la animación aquí para que la explosión se pueda animar
        // widgetVisualizacion->detenerAnimacion();
        mostrarMensajeDerrota();
    }
}

void MainWindow::mostrarMensajeVictoria()
{
    ui->labelEstado->setText("🏆 ¡VICTORIA!");
    ui->labelEstado->setStyleSheet(
        "font-size: 14px; font-weight: bold; color: #00ff00; "
        "padding: 10px; background-color: #16213e; border-radius: 5px;"
        );

    QString mensajeHAL = QString("HAL-69: ¡MISIÓN EXITOSA! Nivel %1 completado en %2 segundos.")
                             .arg(juego->obtenerNivelActual())
                             .arg(juego->obtenerTiempoSimulacion(), 0, 'f', 1);

    agregarMensajeHAL(mensajeHAL);

    ui->btnIniciar->setEnabled(false);
    ui->btnPausar->setEnabled(false);

    // Mensaje de victoria
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("¡VICTORIA!");
    msgBox.setText(QString("🎉 ¡Misión completada con éxito!\n\n"
                           "Nivel: %1\n"
                           "Tiempo: %2 segundos\n"
                           "Combustible restante: %3%\n"
                           "Velocidad final: %4 m/s")
                       .arg(juego->obtenerNivelActual())
                       .arg(juego->obtenerTiempoSimulacion(), 0, 'f', 1)
                       .arg(juego->obtenerCohete()->obtenerPorcentajeCombustible(), 0, 'f', 1)
                       .arg(juego->obtenerCohete()->obtenerVelocidad(), 0, 'f', 1));

    msgBox.setStyleSheet(
        "QMessageBox { background-color: #1a1a2e; } "
        "QLabel { color: #eee; font-size: 12px; } "
        "QPushButton { background-color: #16213e; color: #e94560; "
        "border: 2px solid #e94560; border-radius: 5px; padding: 8px; "
        "font-weight: bold; min-width: 80px; } "
        "QPushButton:hover { background-color: #e94560; color: white; }"
        );

    if(juego->obtenerNivelActual() < 3) {
        msgBox.setInformativeText("¿Quieres continuar al siguiente nivel?");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::Yes);

        int respuesta = msgBox.exec();

        if(respuesta == QMessageBox::Yes) {
            int siguienteNivel = juego->obtenerNivelActual() + 1;

            switch(siguienteNivel) {
            case 2:
                // Iniciar nivel 2 heredando velocidad y combustible del nivel 1
                juego->iniciarNivelDesdeVictoria(2);

                ui->labelNombreNivel->setText("VOSTOK 1 - 1961");
                ui->labelDescripcionNivel->setText(
                    QString::fromStdString(juego->obtenerNivel()->obtenerDescripcion())
                    );

                widgetVisualizacion->actualizarNivel(juego->obtenerNivel(), 2);
                widgetVisualizacion->actualizarCohete(juego->obtenerCohete());

                ui->spinVelocidadInicial->setEnabled(false);
                ui->labelVelocidadInicial->setEnabled(false);

                agregarMensajeHAL(QString("HAL-69: Nivel 2 iniciado.\n"
                                          "Velocidad heredada: %1 m/s\n"
                                          "Combustible heredado: %2 kg\n"
                                          "Objetivo: Mantén velocidad entre 2000-9000 m/s.")
                                      .arg(juego->obtenerCohete()->obtenerVelocidad(), 0, 'f', 1)
                                      .arg(juego->obtenerCohete()->obtenerCombustible(), 0, 'f', 1));

                ui->btnIniciar->setEnabled(true);
                ui->btnReiniciar->setEnabled(true);
                actualizarTelemetria();
                break;

            case 3:
                // Iniciar nivel 3 heredando solo combustible del nivel 2
                juego->iniciarNivelDesdeVictoria(3);

                ui->labelNombreNivel->setText("APOLO 11 - 1969");
                ui->labelDescripcionNivel->setText(
                    QString::fromStdString(juego->obtenerNivel()->obtenerDescripcion())
                    );

                widgetVisualizacion->actualizarNivel(juego->obtenerNivel(), 3);
                widgetVisualizacion->actualizarCohete(juego->obtenerCohete());

                ui->spinVelocidadInicial->setEnabled(false);
                ui->labelVelocidadInicial->setEnabled(false);

                agregarMensajeHAL(QString("HAL-69: Nivel 3 iniciado - Alunizaje.\n"
                                          "Combustible heredado: %1 kg\n"
                                          "Altura inicial: 15 km\n"
                                          "Velocidad inicial: 0 m/s (descenso)\n"
                                          "Objetivo: Aterrizar con velocidad ≤ 5 m/s.")
                                      .arg(juego->obtenerCohete()->obtenerCombustible(), 0, 'f', 1));

                ui->btnIniciar->setEnabled(true);
                ui->btnReiniciar->setEnabled(true);
                actualizarTelemetria();
                break;
            }
        }
    } else {
        msgBox.setInformativeText("¡Has completado todos los niveles!");
        msgBox.exec();
    }
}

void MainWindow::mostrarMensajeDerrota()
{
    ui->labelEstado->setText("❌ MISIÓN FALLIDA");
    ui->labelEstado->setStyleSheet(
        "font-size: 14px; font-weight: bold; color: #ff0000; "
        "padding: 10px; background-color: #16213e; border-radius: 5px;"
        );

    QString razon = QString::fromStdString(juego->obtenerRazonDerrota());
    agregarMensajeHAL("HAL-69: Misión fallida. " + razon);

    ui->btnIniciar->setEnabled(false);
    ui->btnPausar->setEnabled(false);

    // Mensaje de derrota
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("Misión Fallida");
    msgBox.setText(QString("💥 La misión ha fallado\n\n"
                           "Razón: %1\n"
                           "Tiempo: %2 segundos")
                       .arg(razon)
                       .arg(juego->obtenerTiempoSimulacion(), 0, 'f', 1));

    msgBox.setInformativeText("¿Quieres intentarlo de nuevo?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::Yes);

    msgBox.setStyleSheet(
        "QMessageBox { background-color: #1a1a2e; } "
        "QLabel { color: #eee; font-size: 12px; } "
        "QPushButton { background-color: #16213e; color: #e94560; "
        "border: 2px solid #e94560; border-radius: 5px; padding: 8px; "
        "font-weight: bold; min-width: 80px; } "
        "QPushButton:hover { background-color: #e94560; color: white; }"
        );

    int respuesta = msgBox.exec();

    if(respuesta == QMessageBox::Yes) {
        on_btnReiniciar_clicked();
    }
}

void MainWindow::agregarMensajeHAL(const QString& mensaje)
{
    // Agregar mensaje al QTextEdit con formato HTML
    QString mensajeHTML = QString("<p style='margin:2px;'>"
                                  "<span style='color:#e94560;'>HAL-69:</span> "
                                  "<span style='color:#aaa;'>%1</span>"
                                  "</p>")
                              .arg(mensaje.mid(mensaje.indexOf(':') + 2)); // Quitar el "HAL-69: " del inicio

    ui->textHAL->append(mensajeHTML);

    // Scroll automático al final
    QTextCursor cursor = ui->textHAL->textCursor();
    cursor.movePosition(QTextCursor::End);
    ui->textHAL->setTextCursor(cursor);
}

void MainWindow::habilitarControles(bool habilitar)
{
    ui->btnNivel1->setEnabled(habilitar);
    ui->btnNivel2->setEnabled(habilitar);
    ui->btnNivel3->setEnabled(habilitar);
    ui->sliderEmpuje->setEnabled(habilitar);
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    // Solo procesar teclas cuando el nivel 3 está activo y en ejecución
    if(juego->obtenerNivelActual() == 3 && juego->estaEnEjecucion() && !juego->estaPausado()) {
        // Agregar la tecla al conjunto de teclas presionadas
        teclasPresionadas.insert(event->key());
        
        switch(event->key()) {
        case Qt::Key_Left:
        case Qt::Key_Right:
        case Qt::Key_Up:
        case Qt::Key_Down:
            event->accept();
            return;
        }
    }
    
    QMainWindow::keyPressEvent(event);
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    // Remover la tecla del conjunto cuando se suelta
    if(juego->obtenerNivelActual() == 3) {
        teclasPresionadas.remove(event->key());
        
        switch(event->key()) {
        case Qt::Key_Left:
        case Qt::Key_Right:
        case Qt::Key_Up:
        case Qt::Key_Down:
            // La velocidad horizontal se reducirá gradualmente en la física
            event->accept();
            return;
        }
    }
    
    QMainWindow::keyReleaseEvent(event);
}