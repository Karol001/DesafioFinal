#include "nivel3_apolo11.h"
#include <cmath>

Nivel3_Apolo11::Nivel3_Apolo11()
    : Nivel("Apolo 11 - 1969",
            0.0,
            100.0,
            5.0,
            1.62,
            0.0),
    velocidadAterrizajeSegura(5.0),
    gravedadLunar(1.62),
    alturaSuperficie(0.0),
    enDescenso(false),
    alturaInicial(15000.0),
    tiempoTranscurrido(0.0)
{
}

bool Nivel3_Apolo11::verificarVictoria(Cohete* cohete) {
    if (cohete->obtenerAltura() <= alturaSuperficie + 1.0 &&
        std::abs(cohete->obtenerVelocidad()) <= velocidadAterrizajeSegura &&
        !cohete->estaDanado())
    {
        return true;
    }

    if (cohete->obtenerAltura() <= alturaSuperficie &&
        std::abs(cohete->obtenerVelocidad()) > velocidadAterrizajeSegura)
    {
        cohete->marcarDanado();
    }

    return false;
}

void Nivel3_Apolo11::aplicarFisica(Cohete* cohete, double deltaTime) {
    if (deltaTime <= 0.0) return;

    tiempoTranscurrido += deltaTime;

    if (!enDescenso) {
        cohete->establecerAltura(alturaInicial);
        cohete->establecerVelocidad(0.0);
        enDescenso = true;
    }

    if (cohete->tieneCombustible() && cohete->obtenerEmpuje() > 0.0) {
        double aceleracion = cohete->obtenerEmpuje() / cohete->obtenerMasa();
        double nuevaVelocidad =
            cohete->obtenerVelocidad() + aceleracion * deltaTime;
        cohete->establecerVelocidad(nuevaVelocidad);

        double consumo = (cohete->obtenerEmpuje() / 9000.0) * deltaTime;
        cohete->consumirCombustible(consumo);
    }

    cohete->aplicarGravedad(gravedadLunar, deltaTime);

    cohete->actualizarEstado(deltaTime);

    if (cohete->obtenerAltura() <= alturaSuperficie) {
        if (std::abs(cohete->obtenerVelocidad()) > velocidadAterrizajeSegura) {
            cohete->marcarDanado();
        }

        cohete->establecerAltura(alturaSuperficie);
        cohete->establecerVelocidad(0.0);
    }
}

std::string Nivel3_Apolo11::obtenerDescripcion() const {
    return "1969 - Alunizaje del Apolo 11. Controla el descenso sin atmósfera "
           "y usa el empuje para aterrizar suavemente.";
}

std::string Nivel3_Apolo11::obtenerObjetivo() const {
    return "Aterrizar en la Luna con velocidad menor o igual a 5 m/s.";
}

bool Nivel3_Apolo11::verificarAterrizaje(Cohete* cohete) const {
    return (cohete->obtenerAltura() <= alturaSuperficie + 1.0 &&
            std::abs(cohete->obtenerVelocidad()) <= velocidadAterrizajeSegura &&
            !cohete->estaDanado());
}

void Nivel3_Apolo11::iniciarDescenso() {
    enDescenso = false;
    tiempoTranscurrido = 0.0;
}

std::string Nivel3_Apolo11::obtenerFaseDescenso(Cohete* cohete) const {
    double h = cohete->obtenerAltura();

    if (h > 5000)  return "Órbita baja - Inicio de descenso";
    if (h > 1000)  return "Descenso medio";
    if (h > 100)   return "Descenso final";
    return "Aterrizando...";
}

double Nivel3_Apolo11::obtenerVelocidadIdeal(double altura) const {
    return -std::sqrt(2.0 * gravedadLunar * altura);
}

