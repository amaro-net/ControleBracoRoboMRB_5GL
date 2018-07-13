#ifndef SUBSOLUCAOCINEMATICAINVERSA_H
#define SUBSOLUCAOCINEMATICAINVERSA_H


class SubSolucaoCinematicaInversa
{
    public:
        double teta2;
        double teta3;
        double teta4;

        bool teta2possivel = false;
        bool teta3possivel = false;
        bool teta4possivel = false;

        double c2;
        double s2;
        double c3;
        double s3;
        double c4;
        double s4;
        double c5;
        double s5;

        bool possivel;

        /**
         * @brief peso
         * valor que corresponde ao quanto a solução está mais próxima da posicao corrente do braço robô.
         * Este valor é usado como critério para a escolha da solução da cinemática inversa, quando a mesma
         * encontra mais de uma solução válida. No caso de uma solução possuir mais de uma subsolução, este
         * peso indica se a subsolução está mais próxima ou não da posição atual.
         */
        double peso;

        SubSolucaoCinematicaInversa();
};

#endif // SUBSOLUCAOCINEMATICAINVERSA_H
