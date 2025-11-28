#include "SistemaFisica.h"
#include <cmath>

double SistemaFisica::calcularGravedad(double altura, double masa, bool esTierra) {
    double masaCuerpo = esTierra ? MASA_TIERRA : MASA_LUNA;
    double radioCuerpo = esTierra ? RADIO_TIERRA : RADIO_LUNA;

    double distancia = radioCuerpo + altura;
    return (G * masaCuerpo * masa) / (distancia * distancia);
}

double SistemaFisica::calcularAceleracionGravedad(double altura, bool esTierra) {
    double masaCuerpo = esTierra ? MASA_TIERRA : MASA_LUNA;
    double radioCuerpo = esTierra ? RADIO_TIERRA : RADIO_LUNA;

    // g = G * M / r²
    double distancia = radioCuerpo + altura;
    return (G * masaCuerpo) / (distancia * distancia);
}

double SistemaFisica::calcularResistenciaAire(double velocidad, double densidad,
                                              double area, double coeficienteArrastre) {
    return 0.5 * densidad * velocidad * velocidad * coeficienteArrastre * area;
}

double SistemaFisica::calcularDensidadAtmosferica(double altura) {
    const double densidadNivelMar = 1.225;
    const double alturaEscala = 8500.0;

    if (altura < 0) altura = 0;
    if (altura > 100000) return 0.0;

    return densidadNivelMar * std::exp(-altura / alturaEscala);
}

double SistemaFisica::calcularEmpujeNeto(double empuje, double gravedad,
                                         double resistencia) {
    return empuje - gravedad - resistencia;
}

double SistemaFisica::calcularNuevaVelocidad(double velocidadActual,
                                             double aceleracion, double tiempo) {
    return velocidadActual + aceleracion * tiempo;
}

double SistemaFisica::calcularNuevaAltura(double alturaActual, double velocidad,
                                          double aceleracion, double tiempo) {
    return alturaActual + velocidad * tiempo + 0.5 * aceleracion * tiempo * tiempo;
}

double SistemaFisica::calcularVelocidadOrbital(double altura, bool esTierra) {
    double masaCuerpo = esTierra ? MASA_TIERRA : MASA_LUNA;
    double radioCuerpo = esTierra ? RADIO_TIERRA : RADIO_LUNA;

    double distancia = radioCuerpo + altura;
    return std::sqrt((G * masaCuerpo) / distancia);
}

double SistemaFisica::calcularVelocidadEscape(double altura, bool esTierra) {
    double masaCuerpo = esTierra ? MASA_TIERRA : MASA_LUNA;
    double radioCuerpo = esTierra ? RADIO_TIERRA : RADIO_LUNA;

    double distancia = radioCuerpo + altura;
    return std::sqrt((2.0 * G * masaCuerpo) / distancia);
}
