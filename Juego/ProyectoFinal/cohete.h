#ifndef COHETE_H
#define COHETE_H

class Cohete {
private:
    double velocidad;
    double altura;
    double posicionX;  // Posición horizontal para nivel 3
    double velocidadX; // Velocidad horizontal para nivel 3
    double combustible;

    double combustibleMaximo;
    double empuje;
    double masa;
    double masaSeca;

    bool danado;
    bool tripulado;

    double aceleracion;
    double velocidadAnterior;

public:
    Cohete();

    void actualizarEstado(double deltaTime);
    void ajustarEmpuje(double nuevoEmpuje);

    double obtenerVelocidad() const;
    double obtenerAltura() const;
    double obtenerPosicionX() const;
    double obtenerVelocidadX() const;
    double obtenerCombustible() const;
    double obtenerCombustibleMaximo() const;
    double obtenerPorcentajeCombustible() const;
    double obtenerEmpuje() const;
    double obtenerMasa() const;
    double obtenerAceleracion() const;
    bool estaDanado() const;
    bool esTripulado() const;

    void establecerVelocidad(double vel);
    void establecerAltura(double alt);
    void establecerPosicionX(double x);
    void establecerVelocidadX(double vx);
    void ajustarVelocidadX(double deltaVx);
    void establecerCombustible(double comb);
    void establecerMasa(double m);
    void establecerTripulado(bool trip);
    void establecerVelocidadInicial(double vel);

    void aplicarGravedad(double gravedad, double deltaTime);
    void aplicarResistenciaAire(double coeficiente, double deltaTime);
    void consumirCombustible(double cantidad);
    void calcularMasaActual();

    void reiniciarEstado();
    void configurarParaNivel(int nivel);
    void marcarDanado();

    bool tieneCombustible() const;
    bool estaEnAtmosfera() const;

};

#endif // COHETE_H
