#include "nivel3_apolo11.h"
#include <cmath>

Nivel3_Apolo11::Nivel3_Apolo11()
    : Nivel("Apolo 11 - 1969",
            0.0,   // altura objetivo (tocando superficie)
            100.0, // velocidad máxima (no muy relevante aquí)
            0.0,   // velocidad mínima de aterrizaje (base)
            1.62,
            0.0),
    velocidadAterrizajeMin(0.0),
    velocidadAterrizajeMax(5.0),
    gravedadLunar(1.62),
    alturaSuperficie(0.0),
    enDescenso(false),
    alturaInicial(15000.0),
    tiempoTranscurrido(0.0)
{
}


bool Nivel3_Apolo11::verificarVictoria(Cohete* cohete) {
    double v = std::abs(cohete->obtenerVelocidad());

    if (cohete->obtenerAltura() <= alturaSuperficie + 1.0 &&
        v >= velocidadAterrizajeMin &&
        v <= velocidadAterrizajeMax &&
        !cohete->estaDanado() &&
        verificarAterrizajeEnZona(cohete))
    {
        return true;
    }

    if (cohete->obtenerAltura() <= alturaSuperficie) {
        // Verificar si no aterrizó en la zona correcta
        if (!verificarAterrizajeEnZona(cohete)) {
            // No está en la zona, marcar como dañado
            cohete->marcarDanado();
        } else if (v < velocidadAterrizajeMin || v > velocidadAterrizajeMax) {
            // Está en la zona pero velocidad incorrecta
            cohete->marcarDanado();
        }
    }

    return false;
}


void Nivel3_Apolo11::aplicarFisica(Cohete* cohete, double deltaTime) {
    if (deltaTime <= 0.0) return;

    tiempoTranscurrido += deltaTime;

    if (!enDescenso) {
        cohete->establecerAltura(alturaInicial);
        cohete->establecerVelocidad(0.0);
        cohete->establecerPosicionX(0.0);  // Iniciar en el centro
        cohete->establecerVelocidadX(0.0);
        enDescenso = true;
    }

    if (cohete->tieneCombustible() && cohete->obtenerEmpuje() > 0.0) {
        double aceleracion = cohete->obtenerEmpuje() / cohete->obtenerMasa();
        double nuevaVelocidad =
            cohete->obtenerVelocidad() + aceleracion * deltaTime;
        cohete->establecerVelocidad(nuevaVelocidad);

        const double FACTOR_CONSUMO_APOLO = 4000.0;
        double consumo = (cohete->obtenerEmpuje() / FACTOR_CONSUMO_APOLO) * deltaTime;
        cohete->consumirCombustible(consumo);
    }

    cohete->aplicarGravedad(gravedadLunar, deltaTime);
    
    // Aplicar fricción a la velocidad horizontal (simular inercia en el espacio)
    double velocidadXActual = cohete->obtenerVelocidadX();
    if (std::abs(velocidadXActual) > 0.01) {
        // Reducir gradualmente la velocidad horizontal (95% cada segundo)
        double factorFriccion = std::pow(0.95, deltaTime);
        cohete->establecerVelocidadX(velocidadXActual * factorFriccion);
    } else {
        cohete->establecerVelocidadX(0.0);
    }
    
    // Limitar posición X dentro del rango del nivel (aproximadamente -1000 a 1000 metros)
    double posX = cohete->obtenerPosicionX();
    if (posX > 1000.0) {
        cohete->establecerPosicionX(1000.0);
        cohete->establecerVelocidadX(0.0);
    } else if (posX < -1000.0) {
        cohete->establecerPosicionX(-1000.0);
        cohete->establecerVelocidadX(0.0);
    }

    cohete->actualizarEstado(deltaTime);

    if (cohete->obtenerAltura() <= alturaSuperficie) {
        double vImpacto = std::abs(cohete->obtenerVelocidad());

        if (vImpacto < velocidadAterrizajeMin || vImpacto > velocidadAterrizajeMax) {
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
    double v = std::abs(cohete->obtenerVelocidad());
    return (cohete->obtenerAltura() <= alturaSuperficie + 1.0 &&
            v >= velocidadAterrizajeMin &&
            v <= velocidadAterrizajeMax &&
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

bool Nivel3_Apolo11::verificarAterrizajeEnZona(const Cohete* cohete) const {
    // El área de aterrizaje es el 25% central de la pantalla
    // En coordenadas del cohete (metros), asumiendo que el ancho total es 2000m
    // El centro sería de -250m a +250m (25% del total de 2000m = 500m, mitad = 250m)
    double posicionX = cohete->obtenerPosicionX();
    double rangoAterrizaje = 250.0;  // 25% del ancho total de 2000m
    
    return (posicionX >= -rangoAterrizaje && posicionX <= rangoAterrizaje);
}

