#include "nivel2_vostok.h"
#include <cmath>

Nivel2_Vostok::Nivel2_Vostok()
    : Nivel("Vostok 1 - 1961",
            200000.0,  // Reducido de 300km a 200km para hacerlo más jugable
            11000.0,   // Velocidad máxima reducida para más control
            0.0,
            9.81,
            0.3),
    velocidadSeguraMin(2000.0),
    velocidadSeguraMax(8500.0)  // Reducido ligeramente para más desafío
{
}

bool Nivel2_Vostok::verificarVictoria(Cohete* cohete) {
    return (!cohete->estaDanado() &&
            cohete->obtenerAltura() >= alturaObjetivo &&
            cohete->obtenerVelocidad() >= velocidadSeguraMin &&
            cohete->obtenerVelocidad() <= velocidadSeguraMax);
}

void Nivel2_Vostok::aplicarFisica(Cohete* cohete, double deltaTime) {

    if (cohete->tieneCombustible() && cohete->obtenerEmpuje() > 0) {
        double a = cohete->obtenerEmpuje() / cohete->obtenerMasa();
        cohete->establecerVelocidad(cohete->obtenerVelocidad() + a * deltaTime);

        double consumo = (cohete->obtenerEmpuje() / 10000.0) * deltaTime;
        cohete->consumirCombustible(consumo);
    }

    double R = 6371000.0;
    double g = gravedadLocal * std::pow(R / (R + cohete->obtenerAltura()), 2);
    cohete->aplicarGravedad(g, deltaTime);

    if (cohete->estaEnAtmosfera()) {
        double densidad = std::exp(-cohete->obtenerAltura() / 10000.0);
        double k_actual = resistenciaAire * densidad;
        cohete->aplicarResistenciaAire(k_actual, deltaTime);
    }

    cohete->actualizarEstado(deltaTime);

    if (cohete->obtenerVelocidad() > velocidadMaxima) {
        cohete->marcarDanado();
    }
}

std::string Nivel2_Vostok::obtenerDescripcion() const {
    return "1961 - Vostok 1: Manten una velocidad segura mientras asciendes "
           "hasta la orbita. Controla bien tu combustible.";
}

std::string Nivel2_Vostok::obtenerObjetivo() const {
    return "Alcanzar 200 km manteniendo velocidad entre 2000-8500 m/s.";
}

bool Nivel2_Vostok::fueraRangoVelocidadSegura(Cohete* cohete) const {
    double v = cohete->obtenerVelocidad();
    return (v < velocidadSeguraMin || v > velocidadSeguraMax);
}
