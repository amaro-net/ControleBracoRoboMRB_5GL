#ifndef SOLUCAOCINEMATICAINVERSA_H
#define SOLUCAOCINEMATICAINVERSA_H

#include "subsolucaocinematicainversa.h"

class SolucaoCinematicaInversa
{
    public:
        double teta1;
        double teta2;
        double teta3;
        double teta4;
        double teta5;

        double c1;
        double s1;
        double c2;
        double s2;
        double c3;
        double s3;
        double c4;
        double s4;
        double c5;
        double s5;

        bool teta1possivel = false;
        bool teta2possivel = false;
        bool teta3possivel = false;
        bool teta4possivel = false;
        bool teta5possivel = false;

        SubSolucaoCinematicaInversa *subSolucao = nullptr;

        bool possivel;

        bool colideComBaseGir;
        bool colideComSegmentoL1;

        /**
         * @brief peso
         * valor que corresponde ao quanto a solução está mais próxima da posicao corrente do braço robô.
         * Este valor é usado como critério para a escolha da solução da cinemática inversa, quando a mesma
         * encontra mais de uma solução válida.
         */
        double peso;        

        SolucaoCinematicaInversa();
        ~SolucaoCinematicaInversa();

        bool possuiAlgumAnguloImpossivel(int *idxAngulo);
};

static const double pesoTeta1 = 1.0;
static const double pesoTeta2 = 2.0;
static const double pesoTeta3 = 3.0;
static const double pesoTeta4 = 4.0;
static const double pesoTeta5 = 5.0;


#endif // SOLUCAOCINEMATICAINVERSA_H
