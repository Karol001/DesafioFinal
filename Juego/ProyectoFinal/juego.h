#ifndef JUEGO_H
#define JUEGO_H

#include <memory>
#include <string>
#include "Cohete.h"
#include "Nivel.h"
#include "Nivel1_Sputnik.h"
#include "Nivel2_Vostok.h"
#include "Nivel3_Apolo11.h"
#include "AgenteHAL69.h"

class Juego {
private:
    std::unique_ptr<Cohete> cohete;
    std::unique_ptr<Nivel> nivelActual;
    std::unique_ptr<AgenteHAL69> agenteHAL;

    int nivelNumero;
    double tiempoSimulacion;
    double tiempoControl;
    double deltaTime;
    double tiempoMaximo;
    bool enEjecucion;
    bool pausado;
    bool victoria;
    bool derrota;

    const double INTERVALO_CONTROL = 10.0;
    const double INTERVALO_IMPRESION = 10.0;

    void inicializarNivel(int numeroNivel);
    void aplicarFisicaNivel();
    void verificarEstado();
    void consumirCombustiblePorEmpuje();
    void manejarColisionSuelo();

public:
    Juego();
    ~Juego();

    void iniciarNivel(int numeroNivel);
    void iniciarNivelDesdeVictoria(int numeroNivel); // Iniciar nivel manteniendo estado del cohete
    void iniciarNivel1();
    void iniciarNivel2();
    void iniciarNivel3();
    void actualizar();
    void pausar();
    void reanudar();
    void reiniciarNivel();
    void siguienteNivel();

    void ajustarEmpuje(double nuevoEmpuje);
    void establecerVelocidadInicial(double velocidad);
    void establecerAlturaInicial(double altura);
    void iniciarSimulacion();

    bool estaEnEjecucion() const;
    bool estaPausado() const;
    bool haGanado() const;
    bool haPerdido() const;
    int obtenerNivelActual() const;
    double obtenerTiempoSimulacion() const;

    const Cohete* obtenerCohete() const;
    const Nivel* obtenerNivel() const;
    AgenteHAL69* obtenerAgenteHAL();

    void imprimirEstadoConsola() const;
    void imprimirInformacionNivel() const;
    std::string obtenerResumenEstado() const;

    void simularNivel1Interactivo(double velocidadInicial);
    void simularNivel2Interactivo(double empujeInicial);
    void simularNivel3Interactivo();

    bool verificarVictoriaNivel() const;
    bool verificarDerrotaNivel() const;
    std::string obtenerRazonDerrota() const;

    double calcularVelocidadIdealActual() const;
    std::string obtenerFaseActual() const;
    std::string obtenerRecomendacionHAL() const;

private:
    void configurarCoheteParaNivel(int nivel);
    void limpiarEstadoAnterior();
    void actualizarTiempoControl();
    void manejarEventosPeriodicos();
};

#endif // JUEGO_H
