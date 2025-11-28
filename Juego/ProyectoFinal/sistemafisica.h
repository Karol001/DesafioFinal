#ifndef SISTEMAFISICA_H
#define SISTEMAFISICA_H

class SistemaFisica {
public:
    static constexpr double G = 6.67430e-11;
    static constexpr double MASA_TIERRA = 5.972e24;
    static constexpr double RADIO_TIERRA = 6371000.0;
    static constexpr double MASA_LUNA = 7.342e22;
    static constexpr double RADIO_LUNA = 1737000.0;

    static double calcularGravedad(double altura, double masa, bool esTierra = true);

    static double calcularAceleracionGravedad(double altura, bool esTierra = true);

    static double calcularResistenciaAire(double velocidad, double densidad,
                                          double area = 10.0,
                                          double coeficienteArrastre = 0.5);

    static double calcularDensidadAtmosferica(double altura);

    static double calcularEmpujeNeto(double empuje, double gravedad,
                                     double resistencia);


    static double calcularNuevaVelocidad(double velocidadActual,
                                         double aceleracion, double tiempo);

    static double calcularNuevaAltura(double alturaActual, double velocidad,
                                      double aceleracion, double tiempo);

    static double calcularVelocidadOrbital(double altura, bool esTierra = true);

    static double calcularVelocidadEscape(double altura, bool esTierra = true);
};

#endif // SISTEMAFISICA_H
