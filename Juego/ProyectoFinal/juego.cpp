#include "Juego.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cmath>

Juego::Juego()
    : nivelNumero(0),
    tiempoSimulacion(0.0),
    tiempoControl(0.0),
    deltaTime(0.1),
    tiempoMaximo(600.0),
    enEjecucion(false),
    pausado(false),
    victoria(false),
    derrota(false) {

    cohete = std::make_unique<Cohete>();
    agenteHAL = std::make_unique<AgenteHAL69>();
}

Juego::~Juego() {
}

void Juego::iniciarNivel(int numeroNivel) {
    limpiarEstadoAnterior();
    nivelNumero = numeroNivel;

    inicializarNivel(numeroNivel);
    configurarCoheteParaNivel(numeroNivel);

    enEjecucion = false;
    pausado = false;

    imprimirInformacionNivel();
}

void Juego::iniciarNivelDesdeVictoria(int numeroNivel) {
    // Guardar el estado actual del cohete antes de cambiar de nivel
    double velocidadActual = cohete->obtenerVelocidad();
    double combustibleActual = cohete->obtenerCombustible();

    limpiarEstadoAnterior();
    nivelNumero = numeroNivel;

    inicializarNivel(numeroNivel);

    // Configurar el cohete para el nuevo nivel (esto resetea masa, combustible máximo, etc.)
    cohete->configurarParaNivel(numeroNivel);

    // Aplicar la herencia de estados según el nivel
    if (numeroNivel == 2) {
        // Del nivel 1 al 2: heredar velocidad y combustible
        cohete->establecerVelocidad(velocidadActual);
        cohete->establecerCombustible(combustibleActual);
        cohete->establecerAltura(0.0); // Reiniciar altura

        std::cout << "\n=== Transición Nivel 1 -> Nivel 2 ===\n";
        std::cout << "Velocidad heredada: " << velocidadActual << " m/s\n";
        std::cout << "Combustible heredado: " << combustibleActual << " kg\n\n";

    } else if (numeroNivel == 3) {
        // Del nivel 2 al 3: heredar solo combustible, velocidad en 0
        cohete->establecerVelocidad(0.0); // Reiniciar velocidad para descenso
        cohete->establecerCombustible(combustibleActual);
        cohete->establecerAltura(15000.0); // Altura inicial del nivel 3

        std::cout << "\n=== Transición Nivel 2 -> Nivel 3 ===\n";
        std::cout << "Velocidad inicial: 0.0 m/s (descenso)\n";
        std::cout << "Combustible heredado: " << combustibleActual << " kg\n";
        std::cout << "Altura inicial: 15000 m\n\n";
    }

    // NO establecer enEjecucion aquí - solo se establece cuando se inicia la simulación
    enEjecucion = false;
    pausado = false;

    imprimirInformacionNivel();
}

void Juego::inicializarNivel(int numeroNivel) {
    switch (numeroNivel) {
    case 1:
        nivelActual = std::make_unique<Nivel1_Sputnik>();
        tiempoMaximo = 600.0;
        break;
    case 2:
        nivelActual = std::make_unique<Nivel2_Vostok>();
        tiempoMaximo = 2000.0;
        break;
    case 3:
        nivelActual = std::make_unique<Nivel3_Apolo11>();
        tiempoMaximo = 2000.0;
        dynamic_cast<Nivel3_Apolo11*>(nivelActual.get())->iniciarDescenso();
        break;
    default:
        nivelActual = std::make_unique<Nivel1_Sputnik>();
        break;
    }
}

void Juego::configurarCoheteParaNivel(int nivel) {
    // Ajusta masa, tripulación, etc., pero NO combustible.
    cohete->configurarParaNivel(nivel);

    // Aquí SÍ queremos reiniciar completamente el estado del cohete
    // porque esto se usa cuando el jugador selecciona el nivel desde la UI
    // (no en la transición automática entre niveles).
    cohete->reiniciarEstado();

    switch (nivel) {
    case 1:
        cohete->establecerAltura(0.0);
        // La velocidad inicial se establecerá desde la UI.
        cohete->ajustarEmpuje(0.0);
        break;

    case 2:
        cohete->establecerAltura(0.0);
        cohete->establecerVelocidadInicial(0.0);
        cohete->ajustarEmpuje(0.0);
        break;

    case 3:
        cohete->establecerAltura(15000.0);
        cohete->establecerVelocidadInicial(0.0);
        cohete->ajustarEmpuje(0.0);
        break;
    }
}


void Juego::actualizar() {
    if (!enEjecucion || pausado || victoria || derrota) {
        return;
    }

    if (tiempoSimulacion >= tiempoMaximo) {
        derrota = true;
        return;
    }

    aplicarFisicaNivel();

    tiempoSimulacion += deltaTime;
    tiempoControl += deltaTime;

    manejarEventosPeriodicos();

    verificarEstado();
}

void Juego::aplicarFisicaNivel() {
    if (!nivelActual || !cohete) return;

    nivelActual->aplicarFisica(cohete.get(), deltaTime);
    // El consumo de combustible se maneja dentro de aplicarFisica de cada nivel
    // consumirCombustiblePorEmpuje(); // Comentado porque cada nivel maneja su propio consumo
    manejarColisionSuelo();
}

void Juego::consumirCombustiblePorEmpuje() {
    if (cohete->tieneCombustible() && cohete->obtenerEmpuje() > 0) {
        double consumo = 0.0;

        switch (nivelNumero) {
        case 1:
        case 2:
            consumo = (cohete->obtenerEmpuje() / 10000.0) * deltaTime;
            break;
        case 3:
            consumo = (cohete->obtenerEmpuje() / 9000.0) * deltaTime;
            break;
        }

        cohete->consumirCombustible(consumo);
    }
}

void Juego::manejarColisionSuelo() {
    if (cohete->obtenerAltura() <= 0.0) {
        double velocidadImpacto = cohete->obtenerVelocidad();

        if (nivelNumero == 3) {
            // Usar el rango definido en Nivel3_Apolo11
            auto* apolo = dynamic_cast<Nivel3_Apolo11*>(nivelActual.get());
            double vImpactoAbs = std::abs(velocidadImpacto);

            if (apolo) {
                double vMin = apolo->obtenerVelocidadAterrizajeMin();
                double vMax = apolo->obtenerVelocidadAterrizajeMax();

                if (vImpactoAbs < vMin || vImpactoAbs > vMax) {
                    cohete->marcarDanado();
                }
            } else {
                if (vImpactoAbs > 5.0) {
                    cohete->marcarDanado();
                }
            }

            cohete->establecerAltura(0.0);
            cohete->establecerVelocidad(0.0);
        }
    }
}

void Juego::verificarEstado() {
    if (!nivelActual || !cohete) return;

    if (nivelActual->verificarVictoria(cohete.get())) {
        victoria = true;
        enEjecucion = false;
        return;
    }

    if (cohete->estaDanado()) {
        derrota = true;
        enEjecucion = false;
        return;
    }

    // Para nivel 2: verificar si se quedó sin combustible antes de llegar
    if (nivelNumero == 2) {
        if (!cohete->tieneCombustible() &&
            cohete->obtenerAltura() < nivelActual->obtenerAlturaObjetivo()) {
            derrota = true;
            enEjecucion = false;
            return;
        }
    } else {
        // Para otros niveles
        if (!cohete->tieneCombustible() &&
            cohete->obtenerAltura() < nivelActual->obtenerAlturaObjetivo()) {
            derrota = true;
            enEjecucion = false;
            return;
        }
    }
}

void Juego::manejarEventosPeriodicos() {
    if (agenteHAL && tiempoControl >= INTERVALO_CONTROL) {
        agenteHAL->analizarEstado(nivelNumero, *nivelActual, *cohete, tiempoSimulacion);
        tiempoControl = 0.0;
    }
}

void Juego::imprimirEstadoConsola() const {
    if (!cohete) return;

    std::cout << std::fixed << std::setprecision(2);
    std::cout << "t=" << tiempoSimulacion << " s"
              << " | h=" << cohete->obtenerAltura()/1000.0 << " km"
              << " | v=" << cohete->obtenerVelocidad() << " m/s"
              << " | fuel=" << cohete->obtenerPorcentajeCombustible() << " %"
              << " | masa=" << cohete->obtenerMasa() << " kg"
              << " | empuje=" << cohete->obtenerEmpuje() << " N"
              << std::endl;
}

void Juego::imprimirInformacionNivel() const {
    if (!nivelActual) return;

    std::cout << "\n=== Nivel " << nivelNumero << ": "
              << nivelActual->obtenerNombre() << " ===\n";
    std::cout << nivelActual->obtenerDescripcion() << "\n";
    std::cout << "Objetivo: " << nivelActual->obtenerObjetivo() << "\n\n";

    if (nivelNumero == 1) {
        std::cout << "Configuracion del cohete:\n";
        std::cout << "- Masa seca: 3000 kg\n";
        std::cout << "- Combustible: 8000 kg\n";
        std::cout << "- Objetivo: Alcanzar 100 km\n\n";
    } else if (nivelNumero == 2) {
        std::cout << "Configuracion del cohete:\n";
        std::cout << "- Masa seca: 4000 kg\n";
        std::cout << "- Combustible: 12000 kg\n";
        std::cout << "- Tripulado: Si\n";
        std::cout << "- Objetivo: Alcanzar 300 km\n";
        std::cout << "- Restriccion: velocidad 2000-9000 m/s\n\n";
    } else if (nivelNumero == 3) {
        std::cout << "Configuracion del cohete:\n";
        std::cout << "- Masa seca: 15000 kg\n";
        std::cout << "- Combustible: 8000 kg\n";
        std::cout << "- Tripulado: Si\n";
        std::cout << "- Inicias a 15 km de altura\n";
        std::cout << "- Objetivo: Aterrizar con |v| <= 5 m/s\n\n";
    }
}

std::string Juego::obtenerResumenEstado() const {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2);

    if (victoria) {
        ss << "VICTORIA! Nivel " << nivelNumero << " completado en "
           << tiempoSimulacion << " segundos.\n";
    } else if (derrota) {
        ss << "DERROTA: " << obtenerRazonDerrota() << "\n";
    } else {
        ss << "En progreso - Tiempo: " << tiempoSimulacion << "s";
    }

    return ss.str();
}

std::string Juego::obtenerRazonDerrota() const {
    if (!cohete) return "Error interno";

    if (cohete->estaDanado()) {
        if (nivelNumero == 1 && cohete->obtenerVelocidad() > 8000.0) {
            return "El cohete se quemo por exceso de velocidad en la atmosfera";
        } else if (nivelNumero == 2) {
            if (cohete->obtenerVelocidad() > 12000.0) {
                return "Velocidad excesiva (>12000 m/s) destruyo la nave";
            } else {
                return "El cohete ha sido danado";
            }
        } else if (nivelNumero == 3 && cohete->obtenerAltura() <= 0.0) {
            return "Impacto demasiado fuerte en la superficie lunar";
        } else {
            return "El cohete ha sido danado";
        }
    }

    if (!cohete->tieneCombustible()) {
        if (nivelNumero == 2) {
            return "Sin combustible antes de alcanzar el objetivo. Ajusta el empuje para optimizar el consumo.";
        }
        return "Sin combustible antes de alcanzar el objetivo";
    }

    if (tiempoSimulacion >= tiempoMaximo) {
        return "Tiempo maximo agotado";
    }

    return "Razon desconocida";
}

void Juego::ajustarEmpuje(double nuevoEmpuje) {
    if (cohete) {
        cohete->ajustarEmpuje(nuevoEmpuje);
    }
}

void Juego::iniciarSimulacion() {
    if (nivelNumero > 0 && !victoria && !derrota) {
        enEjecucion = true;
        pausado = false;
    }
}

void Juego::establecerVelocidadInicial(double velocidad) {
    if (cohete) {
        cohete->establecerVelocidadInicial(velocidad);
    }
}

void Juego::establecerAlturaInicial(double altura) {
    if (cohete) {
        cohete->establecerAltura(altura);
    }
}

bool Juego::verificarVictoriaNivel() const {
    return nivelActual && cohete && nivelActual->verificarVictoria(cohete.get());
}

bool Juego::verificarDerrotaNivel() const {
    return derrota ||
           (cohete && cohete->estaDanado()) ||
           tiempoSimulacion >= tiempoMaximo;
}

void Juego::pausar() {
    pausado = true;
}

void Juego::reanudar() {
    pausado = false;
}

void Juego::reiniciarNivel() {
    int nivel = nivelNumero; // Guardar el número de nivel antes de limpiar
    iniciarNivel(nivel);
    // Asegurar que no esté en ejecución después de reiniciar
    enEjecucion = false;
    pausado = false;
}

void Juego::siguienteNivel() {
    if (victoria && nivelNumero < 3) {
        iniciarNivel(nivelNumero + 1);
    }
}

void Juego::limpiarEstadoAnterior() {
    tiempoSimulacion = 0.0;
    tiempoControl = 0.0;
    victoria = false;
    derrota = false;
}

const Cohete* Juego::obtenerCohete() const {
    return cohete.get();
}

const Nivel* Juego::obtenerNivel() const {
    return nivelActual.get();
}

AgenteHAL69* Juego::obtenerAgenteHAL() {
    return agenteHAL.get();
}

void Juego::simularNivel1Interactivo(double velocidadInicial) {
    iniciarNivel(1);
    velocidadInicial=1000;
    establecerVelocidadInicial(velocidadInicial);
    ajustarEmpuje(0.0);

    std::cout << "\n=== Simulacion Nivel 1 (Sputnik) ===\n";
    while (estaEnEjecucion() && !haGanado() && !haPerdido()) {
        if (static_cast<int>(tiempoSimulacion * 10) % 10 == 0) {
            imprimirEstadoConsola();
        }
        actualizar();
    }

    if (haGanado()) {
        std::cout << "\n*** VICTORIA en Sputnik 1957 ***\n";
    } else {
        std::cout << "\n" << obtenerRazonDerrota() << "\n";
    }
}

void Juego::simularNivel2Interactivo(double empujeInicial) {
    iniciarNivel(2);
    ajustarEmpuje(empujeInicial);

    std::cout << "\n=== Simulacion Nivel 2 (Vostok) ===\n";
    while (estaEnEjecucion() && !haGanado() && !haPerdido()) {
        if (static_cast<int>(tiempoSimulacion) % 10 == 0) {
            imprimirEstadoConsola();
        }
        actualizar();
    }

    if (haGanado()) {
        std::cout << "\n*** VICTORIA en Vostok 1 ***\n";
        std::cout << "Combustible restante: "
                  << cohete->obtenerPorcentajeCombustible() << "%\n";
    } else {
        std::cout << "\n" << obtenerRazonDerrota() << "\n";
    }
}

void Juego::simularNivel3Interactivo() {
    iniciarNivel(3);

    std::cout << "\n=== Simulacion Nivel 3 (Apolo 11) ===\n";
    std::cout << "Nota: En la version completa, esto sera controlado por interfaz grafica\n";

    ajustarEmpuje(20000.0);

    while (estaEnEjecucion() && !haGanado() && !haPerdido()) {
        imprimirEstadoConsola();
        actualizar();
    }

    if (haGanado()) {
        std::cout << "\n*** ALUNIZAJE EXITOSO ***\n";
    } else {
        std::cout << "\n" << obtenerRazonDerrota() << "\n";
    }
}

double Juego::calcularVelocidadIdealActual() const {
    if (nivelNumero == 3 && nivelActual) {
        auto* apolo = dynamic_cast<Nivel3_Apolo11*>(nivelActual.get());
        if (apolo && cohete) {
            return apolo->obtenerVelocidadIdeal(cohete->obtenerAltura());
        }
    }
    return 0.0;
}

std::string Juego::obtenerFaseActual() const {
    if (nivelNumero == 3 && nivelActual && cohete) {
        auto* apolo = dynamic_cast<Nivel3_Apolo11*>(nivelActual.get());
        if (apolo) {
            return apolo->obtenerFaseDescenso(cohete.get());
        }
    }
    return "En ascenso";
}

bool Juego::estaEnEjecucion() const { return enEjecucion; }
bool Juego::estaPausado() const { return pausado; }
bool Juego::haGanado() const { return victoria; }
bool Juego::haPerdido() const { return derrota; }
int Juego::obtenerNivelActual() const { return nivelNumero; }
double Juego::obtenerTiempoSimulacion() const { return tiempoSimulacion; }
