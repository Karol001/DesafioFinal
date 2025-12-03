#include <iostream>
#include "Juego.h"

int main() {
    Juego juego;

    std::cout << "===== SIMULADOR DE COHETES =====\n";

    std::cout << "\nPresiona Enter para iniciar Nivel 1...";
    std::cin.get();

    juego.simularNivel1Interactivo(4300.0);

    if (juego.haGanado()) {
        std::cout << "\n\nPresiona Enter para iniciar Nivel 2...";
        std::cin.get();

        juego.simularNivel2Interactivo(280000.0);

        if (juego.haGanado()) {
            std::cout << "\n\nPresiona Enter para iniciar Nivel 3...";
            std::cin.get();

            juego.simularNivel3Interactivo();
        }
    }

    std::cout << "\n\n===== FIN DEL JUEGO =====\n";
    std::cout << "Resumen: " << juego.obtenerResumenEstado() << "\n";

    return 0;
}
