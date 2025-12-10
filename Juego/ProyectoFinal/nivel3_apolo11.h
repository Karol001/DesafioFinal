#ifndef NIVEL3_APOLO11_H
#define NIVEL3_APOLO11_H

#include "Nivel.h"

class Nivel3_Apolo11 : public Nivel {
private:
    double velocidadAterrizajeMin;
    double velocidadAterrizajeMax;
    double gravedadLunar;
    double alturaSuperficie;
    bool   enDescenso;
    double alturaInicial;
    double tiempoTranscurrido;

public:
    Nivel3_Apolo11();

    bool verificarVictoria(Cohete* cohete) override;
    void aplicarFisica(Cohete* cohete, double deltaTime) override;
    std::string obtenerDescripcion() const override;
    std::string obtenerObjetivo() const override;

    bool verificarAterrizaje(Cohete* cohete) const;
    void iniciarDescenso();

    double obtenerAlturaInicial() const { return alturaInicial; }
    double obtenerTiempoTranscurrido() const { return tiempoTranscurrido; }
    std::string obtenerFaseDescenso(Cohete* cohete) const;
    double obtenerVelocidadIdeal(double altura) const;

    double obtenerVelocidadAterrizajeMin() const { return velocidadAterrizajeMin; }
    double obtenerVelocidadAterrizajeMax() const { return velocidadAterrizajeMax; }
};

#endif
