#include <iostream>
#include <iomanip>
#include <thread>
#include <chrono>

#include "Cohete.h"
#include "Nivel1_Sputnik.h"
#include "nivel2_vostok.h"
#include "AgenteHAL69.h"


void imprimirEstado(double t, const Cohete& c) {
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "t=" << t << " s"
              << " | h=" << c.obtenerAltura()/1000.0 << " km"
              << " | v=" << c.obtenerVelocidad() << " m/s"
              << " | fuel=" << c.obtenerPorcentajeCombustible() << " %"
              << " | masa=" << c.obtenerMasa() << " kg"
              << std::endl;
}

void simularNivel(int numeroNivel,
                  Nivel& nivel, Cohete& cohete,
                  double deltaTime, double tiempoMaximo,
                  AgenteHAL69* hal)
{
    double tiempo = 0.0;
    bool victoria = false;
    double tiempoControl = 0.0;
    const double INTERVALO_CONTROL = 10.0; // cada 10 s HAL analiza

    std::cout << "\n=== Simulando nivel: " << nivel.obtenerNombre() << " ===\n";
    std::cout << nivel.obtenerDescripcion() << "\n";
    std::cout << "Objetivo: " << nivel.obtenerObjetivo() << "\n\n";

    while (tiempo < tiempoMaximo && !cohete.estaDanado()) {
        nivel.aplicarFisica(&cohete, deltaTime);
        tiempo        += deltaTime;
        tiempoControl += deltaTime;

        if (static_cast<int>(tiempo * 10) % 10 == 0) {
            imprimirEstado(tiempo, cohete);
        }

        // Aquí se consulta al agente HAL-69 (si existe)
        if (hal && tiempoControl >= INTERVALO_CONTROL) {
            hal->analizarEstado(numeroNivel, nivel, cohete, tiempo);
            tiempoControl = 0.0;
        }

        if (nivel.verificarVictoria(&cohete)) {
            victoria = true;
            std::cout << "\n*** VICTORIA en " << nivel.obtenerNombre()
                      << " a t=" << tiempo << " s ***\n";
            imprimirEstado(tiempo, cohete);
            break;
        }
    }

    if (victoria) return;

    if (cohete.estaDanado()) {
        std::cout << "\n>>> El cohete se ha DANADO en este nivel.\n";
    } else if (!cohete.tieneCombustible() &&
               cohete.obtenerAltura() < nivel.obtenerAlturaObjetivo()) {
        std::cout << "\n>>> FALLO: Te quedaste sin combustible a "
                  << cohete.obtenerAltura()/1000.0 << " km\n";
        std::cout << "Necesitabas llegar a "
                  << nivel.obtenerAlturaObjetivo()/1000.0 << " km!\n";
    } else {
        std::cout << "\n>>> Tiempo maximo alcanzado sin cumplir el objetivo.\n";
    }

    imprimirEstado(tiempo, cohete);
}

void simularNivel2Interactivo(Nivel2_Vostok& nivel, Cohete& cohete,
                              double deltaTime, double tiempoMaximo,
                              AgenteHAL69* hal)
{
    double tiempo = 0.0;
    double tiempoActualizacion = 0.0;
    const double INTERVALO_ACTUALIZACION = 10.0;

    std::cout << "\n=== Simulando nivel: " << nivel.obtenerNombre() << " ===\n";
    std::cout << nivel.obtenerDescripcion() << "\n";
    std::cout << "Objetivo: " << nivel.obtenerObjetivo() << "\n\n";

    std::cout << "PARÁMETROS:\n";
    std::cout << "- Velocidad segura: 2000 - 9000 m/s\n";
    std::cout << "- Empuje maximo: 500000 N\n";
    std::cout << "- Meta: 300 km\n\n";

    double empujeActual = cohete.obtenerEmpuje();
    std::cout << "Empuje establecido: " << empujeActual << " N\n";
    std::cout << "La simulacion avanzara automáticamente...\n\n";

    while (tiempo < tiempoMaximo && !cohete.estaDanado()) {
        nivel.aplicarFisica(&cohete, deltaTime);
        tiempo              += deltaTime;
        tiempoActualizacion += deltaTime;

        if (tiempoActualizacion >= INTERVALO_ACTUALIZACION) {
            imprimirEstado(tiempo, cohete);
            tiempoActualizacion = 0.0;

            // Ahora las advertencias las hace HAL-69
            if (hal) {
                hal->analizarEstado(2, nivel, cohete, tiempo);
            }
        }

        // Verificar victoria
        if (nivel.verificarVictoria(&cohete)) {
            std::cout << "\n *** VICTORIA en " << nivel.obtenerNombre()
                << " a t=" << tiempo << "  ***\n";
            imprimirEstado(tiempo, cohete);
            std::cout << "\nCombustible restante: "
                      << cohete.obtenerPorcentajeCombustible() << "%\n";
            return;
        }

        // Verificar fallo prematuro (sin combustible)
        if (!cohete.tieneCombustible() && cohete.obtenerAltura() < 300000.0) {
            std::cout << "\n FALLO: Sin combustible a "
                      << cohete.obtenerAltura()/1000.0 << " km\n";
            std::cout << "Necesitabas llegar a 300 km!\n";
            imprimirEstado(tiempo, cohete);
            return;
        }
    }

    if (cohete.estaDanado()) {
        std::cout << "\n💥 El cohete se DANO (velocidad excesiva)\n";
        if (hal && cohete.obtenerVelocidad() > nivel.obtenerVelocidadMaxima()) {
            hal->registrarFalloNivel2PorVelocidadAlta();
        }
    } else {
        std::cout << "\n Tiempo maximo alcanzado sin objetivo\n";
    }

    imprimirEstado(tiempo, cohete);
}



int main() {

    AgenteHAL69 hal;
    //NIVEL 1: SPUTNIK
    Cohete cohete;
    cohete.configurarParaNivel(1);

    std::cout << "===== NIVEL 1: Sputnik 1957 =====\n";
    std::cout << "Configuracion del cohete:\n";
    std::cout << "- Masa seca: 3000 kg\n";
    std::cout << "- Combustible: 8000 kg\n";
    std::cout << "- Objetivo: Alcanzar 100 km\n\n";

    double velInicial = 4300;
    std::cout << "Velocidad inicial recomendada: 4300 m/s\n";
    std::cout << "Ingresa velocidad inicial (m/s): ";

    cohete.establecerVelocidadInicial(velInicial);
    cohete.ajustarEmpuje(0.0);

    Nivel1_Sputnik n1;
    simularNivel(1, n1, cohete, 0.1, 600.0, &hal);

    if (n1.verificarVictoria(&cohete)) {

        //NIVEL 2: VOSTOK
        std::cout << "\n\n===== NIVEL 2: Vostok 1961 =====\n";
        std::cout << "Configuracion del cohete:\n";
        std::cout << "- Masa seca: 4000 kg\n";
        std::cout << "- Combustible: 12000 kg\n";
        std::cout << "- Tripulado: Si\n";
        std::cout << "- Objetivo: Alcanzar 300 km\n";
        std::cout << "- Restriccion: velocidad 2000-9000 m/s\n\n";

        Cohete cohete2;
        cohete2.configurarParaNivel(2);


        cohete2.establecerAltura(0.0);
        cohete2.establecerVelocidadInicial(0.0);

        Nivel2_Vostok n2;

        double empujeInicial = 100000;
        std::cout << "Empuje recomendado: 280000 N\n";
        std::cout << "Ingresa empuje constante (0-500000 N): ";
        cohete2.ajustarEmpuje(empujeInicial);

        simularNivel2Interactivo(n2, cohete2, 0.1, 2000.0, &hal);

        if (n2.verificarVictoria(&cohete2)) {
        }
    } else {
        std::cout << "No pasaste el Nivel 1. Intenta de nuevo.\n";
    }

    return 0;
}
