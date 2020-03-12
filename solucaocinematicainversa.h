/*
    Controle do Braço Robô MRB-5GL - controls and configures MRB-5GL, a 5 DOF Robot Arm prototype

    Copyright (C) 2019  Amaro Duarte de Paula Neto

    This file is part of Controle do Braço Robô MRB-5GL.

    Controle do Braço Robô MRB-5GL is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Controle do Braço Robô MRB-5GL is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Controle do Braço Robô MRB-5GL.  If not, see <https://www.gnu.org/licenses/>.

    contact e-mail: amaro.net80@gmail.com


    Este arquivo é parte do programa Controle do Braço Robô MRB-5GL

    Controle do Braço Robô MRB-5GL é um software livre; você pode redistribuí-lo e/ou
    modificá-lo dentro dos termos da Licença Pública Geral GNU como
    publicada pela Free Software Foundation (FSF); na versão 3 da
    Licença, ou (a seu critério) qualquer versão posterior.

    Controle do Braço Robô MRB-5GL é distribuído na esperança de que possa ser útil,
    mas SEM NENHUMA GARANTIA; sem uma garantia implícita de ADEQUAÇÃO
    a qualquer MERCADO ou APLICAÇÃO EM PARTICULAR. Veja a
    Licença Pública Geral GNU para maiores detalhes.

    Você deve ter recebido uma cópia da Licença Pública Geral GNU junto
    com este programa, Se não, veja <http://www.gnu.org/licenses/>.
*/
#ifndef SOLUCAOCINEMATICAINVERSA_H
#define SOLUCAOCINEMATICAINVERSA_H

#include <math.h>

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
