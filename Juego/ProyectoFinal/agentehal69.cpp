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
