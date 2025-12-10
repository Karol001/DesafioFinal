#include "AgenteHAL69.h"
#include <iostream>
#include "Cohete.h"
#include "Nivel.h"

AgenteHAL69::AgenteHAL69()
    : velSeguraMinNivel2(2000.0),
    velSeguraMaxNivel2(9000.0),
    velAdvertenciaPeligrosaNivel2(9000.0),
    combustibleBajo(40.0),
    combustibleCritico(20.0),
    fallosVelocidadAltaNivel2(0)
{
}

void AgenteHAL69::analizarEstado(int numeroNivel,
                                 const Nivel& nivel,
                                 const Cohete& cohete,
                                 double tiempoSimulacion)
{
    using std::cout;
    using std::endl;

    double v    = cohete.obtenerVelocidad();
    double fuel = cohete.obtenerPorcentajeCombustible();
    double h    = cohete.obtenerAltura();

    bool emitioAlgo = false;

    cout << "HAL-69 [" << nivel.obtenerNombre()
         << " | t=" << tiempoSimulacion << " s]: ";

    if (numeroNivel == 2) {
        if (v < velSeguraMinNivel2) {
            cout << "\n  - Velocidad baja (" << v
                 << " m/s). Te recomiendo aumentar un poco el empuje.";
            emitioAlgo = true;
        } else if (v > velAdvertenciaPeligrosaNivel2) {
            cout << "\n  - Velocidad PELIGROSA (" << v
                 << " m/s). Reduce el empuje para evitar dañar la nave.";
            emitioAlgo = true;
        } else if (v > velSeguraMaxNivel2 * 0.9) {
            cout << "\n  - Estas cerca del limite seguro de velocidad. Vigila el empuje.";
            emitioAlgo = true;
        }
    } else if (numeroNivel == 1) {
        double velMaxNivel1 = nivel.obtenerVelocidadMaxima();
        if (v > velMaxNivel1 * 0.95 && h < 100000.0) {
            cout << "\n  - Velocidad muy alta en la atmosfera. "
                    "Podrias quemar el Sputnik.";
            emitioAlgo = true;
        }
    }

    if (fuel < combustibleCritico) {
        cout << "\n  - Combustible CRITICO (" << fuel
             << "%). Ajusta el empuje o no alcanzaras el objetivo.";
        emitioAlgo = true;
    } else if (fuel < combustibleBajo) {
        cout << "\n  - Combustible bajo (" << fuel
             << "%). Administra mejor la potencia.";
        emitioAlgo = true;
    }

    if (!emitioAlgo) {
        cout << "Todo en rangos seguros por ahora." << endl;
    } else {
        cout << endl;
    }
}

std::string AgenteHAL69::obtenerMensajeUI(int numeroNivel,
                                         const Nivel& nivel,
                                         const Cohete& cohete,
                                         double tiempoSimulacion)
{
    (void)tiempoSimulacion; // Parámetro reservado para uso futuro
    double v = cohete.obtenerVelocidad();
    double fuel = cohete.obtenerPorcentajeCombustible();
    double h = cohete.obtenerAltura();
    double alturaObjetivo = nivel.obtenerAlturaObjetivo();
    
    // Mensajes positivos
    if (numeroNivel == 2) {
        if (v >= 2000.0 && v <= 8500.0 && fuel > 30.0 && h < alturaObjetivo) {
            double progreso = (h / alturaObjetivo) * 100.0;
            if (progreso > 50.0) {
                return "Excelente progreso. Mantén esta velocidad y llegarás al objetivo.";
            } else if (progreso > 25.0) {
                return "Buen ritmo. Continúa así para alcanzar la órbita.";
            }
        }
        
        // Advertencias
        if (v > 10000.0) {
            return "¡PELIGRO! Velocidad crítica. Reduce el empuje inmediatamente o la nave se destruirá.";
        } else if (v > 9500.0) {
            return "Velocidad muy alta. Reduce el empuje para evitar daños.";
        } else if (v < 2000.0 && h < alturaObjetivo * 0.5) {
            return "Velocidad baja. Aumenta el empuje para mantener el ascenso.";
        }
        
        if (fuel < 10.0) {
            return "¡COMBUSTIBLE CRÍTICO! Optimiza el empuje o no alcanzarás el objetivo.";
        } else if (fuel < 25.0) {
            return "Combustible bajo. Ajusta el empuje para optimizar el consumo.";
        }
        
        if (h >= alturaObjetivo * 0.9 && v >= 2000.0 && v <= 8500.0) {
            return "¡Casi ahí! Mantén la velocidad actual para completar la misión.";
        }
    } else if (numeroNivel == 1) {
        if (v > 7500.0 && h < 100000.0) {
            return "Velocidad muy alta en la atmósfera. Podrías quemar el Sputnik.";
        }
        if (fuel < 20.0) {
            return "Combustible bajo. Ajusta el empuje.";
        }
    }
    
    return ""; // Sin mensaje si todo está bien
}

void AgenteHAL69::registrarFalloNivel2PorVelocidadAlta()
{
    ++fallosVelocidadAltaNivel2;

    if (fallosVelocidadAltaNivel2 == 1) {
        std::cout << "HAL-69: He registrado un fallo por exceso de velocidad. "
                     "A partir de ahora te avisare antes de superar "
                  << velAdvertenciaPeligrosaNivel2 << " m/s.\n";
    } else if (fallosVelocidadAltaNivel2 > 1) {
        velAdvertenciaPeligrosaNivel2 *= 0.9;
        if (velAdvertenciaPeligrosaNivel2 < 1500.0) {
            velAdvertenciaPeligrosaNivel2 = 1500.0;
        }

        std::cout << "HAL-69: He aprendido de tus fallos anteriores, "
                     "ahora te avisare antes de superar "
                  << velAdvertenciaPeligrosaNivel2 << " m/s.\n";
    }
}
