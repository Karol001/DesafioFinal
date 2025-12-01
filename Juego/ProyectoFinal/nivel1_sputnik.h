#ifndef NIVEL1_SPUTNIK_H
#define NIVEL1_SPUTNIK_H

#include "Nivel.h"

class Nivel1_Sputnik : public Nivel {
private:
    double alturaOrbita;
    double velocidadCritica;

public:
    Nivel1_Sputnik();

    bool verificarVictoria(Cohete* cohete) override;
    void aplicarFisica(Cohete* cohete, double deltaTime) override;
    std::string obtenerDescripcion() const override;
    std::string obtenerObjetivo() const override;

    bool verificarQuemadura(Cohete* cohete) const;
};

#endif // NIVEL1_SPUTNIK_H
