#ifndef CINEMATICA_H
#define CINEMATICA_H

#include <QMatrix4x4>
#include "constantes.h"
#include "solucaocinematicainversa.h"

/************ Tamanhos dos segmentos ************/
const double L1 = 11.65;
const double L2 = 5.825;
const double L3 = 8.633297;

const double Lg = 7.5; // tamanho do eixo que corta verticalmente a base da garra

const double LgL3 = Lg + L3;


/****** PARÂMETROS DE DENAVIT-HATENBERG (PARÂMETROS DH) ******/

// ângulos alfa(i-1) em graus
const double alfa0 = 0.0;
const double alfa1 = 90.0;
// TODO: Cinemática direta/inversa: testar uma cinemática provisória com o alfa2 = -180 e o alfa3 = 180, para o teta3 ser sempre positivo
const double alfa2 = 0.0;
const double alfa3 = 0.0;
const double alfa4 = 90.0;

// distâncias a(i-1) em centímetros
const double a0 = 0.0; // Desconsidera o deslocamento para frente da junta 1 (eixos Z0 e Z1 passam a se cruzar)
//const double a1 = 0.025;
const double a1 = 0.0;
const double a2 = L1;
const double a3 = L2;
//const double a4 = 0.45;
const double a4 = 0.0; // Desconsidera o deslocamento a4 entre Z4 e Z5 (eixos Z4 e Z5 passam a se cruzar)

// distâncias d(i) em centímetros
const double d1 = 17.547644;
/* WARNING: d2, d3 e d4 zerados significa que o ponto do pulso da garra (e o ponto da garra, consequentemente),
   estará alinhado com a origem do sistema de coordenadas XYZ localizado na base do
   braço robô. O efeito disto é que o ponto
   efetivamente calculado para a garra estará deslocado a uma distância de
   1,3191 cm do ponto que fica rente com o eixo de giro da garra. */
/*const double d2 = 4.293516;
const double d3 = -3.438032;
const double d4 = -2.174584;*/

const double d2 = 0;
const double d3 = 0;
const double d4 = 0;

/* WARNING: O d5 zerado significa que, para efeito de cálculo, a posição do pulso da garra
 * (junta 4) será a mesma posição da junta 3, isto levando-se em conta que a
 * soma das distâncias d2, d3 e d4 serão zero.
   */
const double d5 = 0;

class Cinematica
{
    public:
        Cinematica();

        /* Representações de coordenadas da garra */
        double *coordenadasAngFixosOuEulerZXY(QMatrix4x4 T);
        double *coordenadasElevacaoETorcao(QMatrix4x4 T, double teta2graus, double teta3graus, double teta4graus, double teta5graus);

        /* Métodos de cinemática direta */
        QMatrix4x4 cinematicaDireta(double teta1graus, double teta2graus, double teta3graus, double teta4graus, double teta5graus);
        QMatrix4x4 matrizPosGarra(double teta1graus, double teta2graus, double teta3graus, double teta4graus, double teta5graus);
        double *posicaoGarra(double teta1graus, double teta2graus, double teta3graus, double teta4graus, double teta5graus);

        /* Métodos de cinemática inversa */
        double *angJuntas(double *x, double *y, double *z,
                          double *gamaGraus, double *betaGraus, double *alfaGraus,
                          double *angulosCorrentesJuntas,
                          double *angulosMaxGraus, double *angulosMinGraus,
                          bool *posicaoProjetada = nullptr, bool *posicaoAtingivel = nullptr);

};

#endif // CINEMATICA_H
