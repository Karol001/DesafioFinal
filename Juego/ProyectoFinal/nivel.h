#ifndef NIVEL_H
#define NIVEL_H

#include <string>
#include "Cohete.h"

class Nivel {
protected:
    std::string nombre;
    double alturaObjetivo;
    double velocidadMaxima;
    double velocidadMinimaAterrizaje;
    double gravedadLocal;
    double resistenciaAire;

public:
    Nivel(const std::string& nom, double altObj, double velMax,
          double velMinAterrizaje, double grav, double resist);

    virtual ~Nivel();

    std::string obtenerNombre() const;
    double obtenerAlturaObjetivo() const;
    double obtenerGravedad() const;
    double obtenerResistenciaAire() const;
    double obtenerVelocidadMaxima() const;
    double obtenerVelocidadMinimaAterrizaje() const;

    virtual bool verificarVictoria(Cohete* cohete) = 0;
    virtual void aplicarFisica(Cohete* cohete, double deltaTime) = 0;
    virtual std::string obtenerDescripcion() const = 0;
    virtual std::string obtenerObjetivo() const = 0;
};

#endif // NIVEL_H
