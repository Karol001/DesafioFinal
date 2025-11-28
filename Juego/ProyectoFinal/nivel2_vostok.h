#ifndef NIVEL2_VOSTOK_H
#define NIVEL2_VOSTOK_H

#include "Nivel.h"

class Nivel2_Vostok : public Nivel {
private:
    double velocidadSeguraMin;
    double velocidadSeguraMax;

public:
    Nivel2_Vostok();

    bool verificarVictoria(Cohete* cohete) override;
    void aplicarFisica(Cohete* cohete, double deltaTime) override;
    std::string obtenerDescripcion() const override;
    std::string obtenerObjetivo() const override;

private:
    bool fueraRangoVelocidadSegura(Cohete* cohete) const;
};

#endif
