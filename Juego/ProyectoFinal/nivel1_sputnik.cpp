#include "Nivel1_Sputnik.h"
#include <cmath>

Nivel1_Sputnik::Nivel1_Sputnik()
    : Nivel("Sputnik 1957", 100000.0, 8000.0, 0.0, 9.81, 0.5),
    alturaOrbita(100000.0), velocidadCritica(8000.0) {
}

bool Nivel1_Sputnik::verificarVictoria(Cohete* cohete) {
    if (cohete->obtenerAltura() >= alturaOrbita && !cohete->estaDanado()) {
        if (cohete->obtenerVelocidad() >= 0) {
            return true;
        }
    }
    return false;
}

void Nivel1_Sputnik::aplicarFisica(Cohete* cohete, double deltaTime) {
    if (cohete->obtenerCombustible() > 0 && cohete->obtenerEmpuje() > 0) {
        double aceleracion = cohete->obtenerEmpuje() / cohete->obtenerMasa();
        double nuevaVelocidad = cohete->obtenerVelocidad() + aceleracion * deltaTime;
        cohete->establecerVelocidad(nuevaVelocidad);

        double consumo = (cohete->obtenerEmpuje() / 10000.0) * deltaTime;
        cohete->consumirCombustible(consumo);
    }

    double radioTierra = 6371000.0;
    double gravedadActual = gravedadLocal *
                            std::pow(radioTierra / (radioTierra + cohete->obtenerAltura()), 2);

    cohete->aplicarGravedad(gravedadActual, deltaTime);

    if (cohete->obtenerAltura() < 100000.0) {
        double factorDensidad = std::exp(-cohete->obtenerAltura() / 10000.0);
        double resistenciaActual = resistenciaAire * factorDensidad;
        cohete->aplicarResistenciaAire(resistenciaActual, deltaTime);
    }

    cohete->actualizarEstado(deltaTime);

    if (verificarQuemadura(cohete)) {
        cohete->marcarDanado();
    }
}

std::string Nivel1_Sputnik::obtenerDescripcion() const {
    return "1957 - Lanzamiento del primer satelite artificial. "
           "Debes alcanzar una altura de 100 km sin que el satelite "
           "se queme por exceso de velocidad en la atmosfera.";
}

std::string Nivel1_Sputnik::obtenerObjetivo() const {
    return "Alcanzar 100 km de altura sin superar los 8000 m/s";
}

bool Nivel1_Sputnik::verificarQuemadura(Cohete* cohete) const {
    if (cohete->obtenerAltura() < 100000.0 &&
        cohete->obtenerVelocidad() > velocidadCritica) {
        return true;
    }
    return false;
}
