#include "Nivel.h"

Nivel::Nivel(const std::string& nom, double altObj, double velMax,
             double velMinAterrizaje, double grav, double resist)
    : nombre(nom), alturaObjetivo(altObj), velocidadMaxima(velMax),
    velocidadMinimaAterrizaje(velMinAterrizaje),
    gravedadLocal(grav), resistenciaAire(resist) {
}

Nivel::~Nivel() {
}

std::string Nivel::obtenerNombre() const {
    return nombre;
}

double Nivel::obtenerAlturaObjetivo() const {
    return alturaObjetivo;
}

double Nivel::obtenerGravedad() const {
    return gravedadLocal;
}

double Nivel::obtenerResistenciaAire() const {
    return resistenciaAire;
}

double Nivel::obtenerVelocidadMaxima() const {
    return velocidadMaxima;
}

double Nivel::obtenerVelocidadMinimaAterrizaje() const {
    return velocidadMinimaAterrizaje;
}
