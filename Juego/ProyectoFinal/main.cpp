#include <iostream>
#include <iomanip>

#include "Cohete.h"
#include "Nivel1_Sputnik.h"


void imprimirEstado(double t, const Cohete& c) {
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "t=" << t << " s"
              << " | h=" << c.obtenerAltura() << " m"
              << " | v=" << c.obtenerVelocidad() << " m/s"
              << " | fuel=" << c.obtenerPorcentajeCombustible() << " %"
              << std::endl;
}

void simularNivel(Nivel& nivel, Cohete& cohete,
                  double deltaTime, double tiempoMaximo)
{
    double tiempo = 0.0;

    std::cout << "\n=== Simulando nivel: " << nivel.obtenerNombre() << " ===\n";
    std::cout << nivel.obtenerDescripcion() << "\n";
    std::cout << "Objetivo: " << nivel.obtenerObjetivo() << "\n\n";

    while (tiempo < tiempoMaximo && !cohete.estaDanado()) {
        nivel.aplicarFisica(&cohete, deltaTime);
        tiempo += deltaTime;

        if (static_cast<int>(tiempo * 10) % 10 == 0) {
            imprimirEstado(tiempo, cohete);
        }

        if (nivel.verificarVictoria(&cohete)) {
            std::cout << "\n*** VICTORIA en " << nivel.obtenerNombre()
                      << " a t=" << tiempo << " s! ***\n";
            imprimirEstado(tiempo, cohete);
            return;
        }
    }

    if (cohete.estaDanado()) {
        std::cout << "\n>>> El cohete se ha DANADO en este nivel.\n";
    } else {
        std::cout << "\n>>> Tiempo maximo alcanzado sin cumplir el objetivo.\n";
    }
    imprimirEstado(tiempo, cohete);
}

int main() {
    // NIVEL 1 – Sputnik
    Cohete cohete;
    cohete.configurarParaNivel(1); // ajusta masa, combustible, etc.

    double velInicial;
    std::cout << "===== PRUEBA NIVEL 1: Sputnik =====\n";
    std::cout << "Ingresa una velocidad inicial de lanzamiento (m/s): ";
    velInicial = 1000;

    cohete.establecerVelocidadInicial(velInicial);
    // Para el nivel 1 usamos solo velocidad inicial (sin empuje continuo)
    cohete.ajustarEmpuje(0.0);

    Nivel1_Sputnik n1;
    simularNivel(n1, cohete, 0.1, 600.0); // dt = 0.1 s, máx 600 s

    return 0;
}
