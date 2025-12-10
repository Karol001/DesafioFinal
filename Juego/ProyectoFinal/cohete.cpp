#include "Cohete.h"
#include <algorithm>

Cohete::Cohete()
    : velocidad(0.0), altura(0.0), posicionX(0.0), velocidadX(0.0), 
    combustible(8000.0), combustibleMaximo(8000.0),
    empuje(0.0), masa(3000.0 + 8000.0), masaSeca(3000.0),
    danado(false), tripulado(false),
    aceleracion(0.0), velocidadAnterior(0.0){}

void Cohete::actualizarEstado(double deltaTime) {
    if (deltaTime <= 0.0) return;

    aceleracion = (velocidad - velocidadAnterior) / deltaTime;
    velocidadAnterior = velocidad;

    altura += velocidad * deltaTime;
    
    // Actualizar posición horizontal
    posicionX += velocidadX * deltaTime;

    if (altura <= 0.0) {
        if (velocidad < -300.0) {
            danado = true;
        }

        altura = 0.0;
        velocidad = 0.0;
    }
}

void Cohete::ajustarEmpuje(double nuevoEmpuje) {
    if(!tieneCombustible()){
        empuje = 0.0;
        return;
    }
    const double EMPUJE_MAX = 500000.0;
    empuje = std::clamp(nuevoEmpuje, 0.0, EMPUJE_MAX);
}

double Cohete::obtenerVelocidad() const {
    return velocidad;
}

double Cohete::obtenerAltura() const {
    return altura;
}

double Cohete::obtenerPosicionX() const {
    return posicionX;
}

double Cohete::obtenerVelocidadX() const {
    return velocidadX;
}

double Cohete::obtenerCombustible() const {
    return combustible;
}

double Cohete::obtenerCombustibleMaximo() const {
    return combustibleMaximo;
}

double Cohete::obtenerPorcentajeCombustible() const {
    if (combustibleMaximo <= 0.0) return 0.0;
    return (combustible / combustibleMaximo) * 100.0;
}

double Cohete::obtenerEmpuje() const {
    return empuje;
}

double Cohete::obtenerMasa() const {
    return masa;
}

double Cohete::obtenerAceleracion() const {
    return aceleracion;
}

bool Cohete::estaDanado() const {
    return danado;
}

bool Cohete::esTripulado() const {
    return tripulado;
}

void Cohete::establecerVelocidad(double vel) {
    velocidad = vel;
}

void Cohete::establecerAltura(double alt) {
    altura = std::max(0.0, alt);
}

void Cohete::establecerPosicionX(double x) {
    posicionX = x;
}

void Cohete::establecerVelocidadX(double vx) {
    velocidadX = vx;
}

void Cohete::ajustarVelocidadX(double deltaVx) {
    velocidadX += deltaVx;
    // Limitar velocidad horizontal máxima
    const double VELOCIDAD_X_MAX = 50.0;
    if (velocidadX > VELOCIDAD_X_MAX) velocidadX = VELOCIDAD_X_MAX;
    if (velocidadX < -VELOCIDAD_X_MAX) velocidadX = -VELOCIDAD_X_MAX;
}


void Cohete::establecerMasa(double m) {
    masaSeca = std::max(100.0, m);
    calcularMasaActual();
}

void Cohete::establecerTripulado(bool trip) {
    tripulado = trip;
}

void Cohete::establecerVelocidadInicial(double vel) {
    establecerVelocidad(vel);
    velocidadAnterior = vel;
}

void Cohete::aplicarGravedad(double gravedad, double deltaTime) {
    velocidad -= gravedad * deltaTime;
}

void Cohete::aplicarResistenciaAire(double coeficiente, double deltaTime) {
    if (!estaEnAtmosfera()) return;

    double v = velocidad;
    if (v == 0.0 || masa <= 0.0) return;

    double aceleracionDrag = (coeficiente * v * v) / masa;

    if (v > 0) {
        velocidad -= aceleracionDrag * deltaTime;
    } else {
        velocidad += aceleracionDrag * deltaTime;
    }
}

void Cohete::consumirCombustible(double cantidad) {
    if (cantidad <= 0.0) return;

    combustible -= cantidad;
    if (combustible < 0.0) {
        combustible = 0.0;
    }

    calcularMasaActual();

    if (!tieneCombustible()) {
        empuje = 0.0;
    }
}

void Cohete::calcularMasaActual() {
    masa = masaSeca + combustible;
}

void Cohete::reiniciarEstado() {
    velocidad = 0.0;
    velocidadAnterior = 0.0;
    altura = 0.0;
    posicionX = 0.0;
    velocidadX = 0.0;
    empuje = 0.0;
    danado = false;
    aceleracion = 0.0;

    combustible = combustibleMaximo;
    calcularMasaActual();
}

void Cohete::configurarParaNivel(int nivel) {
    // Solo cambiamos características estructurales del cohete.
    // NO tocamos combustible ni combustibleMaximo para que
    // el tanque sea el mismo en todos los niveles.

    switch (nivel) {
    case 1:
        masaSeca = 3000.0;
        tripulado = false;
        break;

    case 2:
        masaSeca = 4000.0;
        tripulado = true;
        break;

    case 3:
        masaSeca = 15000.0;
        tripulado = true;
        break;

    default:
        masaSeca = 3000.0;
        tripulado = false;
        break;
    }

    // Recalculamos la masa total con el combustible actual.
    calcularMasaActual();
}


void Cohete::marcarDanado() {
    danado = true;
}

bool Cohete::tieneCombustible() const {
    return combustible > 0.0;
}

bool Cohete::estaEnAtmosfera() const {
    return altura < 100000.0;
}

void Cohete::establecerCombustible(double comb) {
    // Asegurar que el combustible no exceda el máximo del nivel actual
    combustible = std::clamp(comb, 0.0, combustibleMaximo);
    calcularMasaActual();
}
