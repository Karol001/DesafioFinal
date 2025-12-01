#ifndef AGENTE_HAL69_H
#define AGENTE_HAL69_H

#include <string>

class Cohete;
class Nivel;

class AgenteHAL69 {
private:
    double velSeguraMinNivel2;
    double velSeguraMaxNivel2;
    double velAdvertenciaPeligrosaNivel2;
    double combustibleBajo;
    double combustibleCritico;

    int fallosVelocidadAltaNivel2;

public:
    AgenteHAL69();

    void analizarEstado(int numeroNivel,
                        const Nivel& nivel,
                        const Cohete& cohete,
                        double tiempoSimulacion);

    void registrarFalloNivel2PorVelocidadAlta();
};

#endif // AGENTE_HAL69_H
