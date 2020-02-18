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

#include "utils.h"

float arredondaPara(float num, int casasDecimais)
{
    return float(arredondaPara(double(num), casasDecimais));
}


double arredondaPara(double num, int casasDecimais)
{
    //char charArrayNumArred[50];

    double divisor = pow(10, casasDecimais);

    double parteInteira = trunc(num);
    double parteDecimal = num - parteInteira;
    parteDecimal = round(parteDecimal * divisor) / divisor;

    double numArred = parteInteira + parteDecimal;

    //double numArred = lround(num * divisor) / divisor;

    /*double numArred = num * divisor;
    numArred = lround(numArred);
    numArred = numArred / divisor;*/

    //sprintf(charArrayNumArred, "%.*f\n", casasDecimais, numArredAux);
    //QString strNumArred(charArrayNumArred);
    //double numArred = strNumArred.toDouble();

    return numArred;    
}

bool EhIgual(double p1, double p2, int casasDecimais)
{
    double incremento = 0.0;

    double epsilon;

    if((qAbs(p1) < 1.0) && (qAbs(p2) < 1.0))
        epsilon = pow(10.0, -(casasDecimais+1));
    else
        epsilon = pow(10.0, -(casasDecimais+2));

    if((qAbs(p1) < 1.0) || (qAbs(p2) < 1.0))
    {
        if(p1 >= 0.0 && p2 >= 0.0)
            incremento = 1.0;
        else if(p1 <= 0.0 && p2 <= 0.0)
            incremento = -1.0;
        else if((p1 >= 0.0 && p2 <= 0.0) || (p1 <= 0.0 && p2 >= 0.0))
            incremento = 2.0;

        p1 += incremento;
        p2 += incremento;
    }

    double diff = qAbs(p1 - p2);

    if(diff <= epsilon)
        return true;

    double max = qMax(qAbs(p1), qAbs(p2));

    return (diff <= epsilon * max);
}

bool EhIgual(float p1, float p2, int casasDecimais)
{
    float incremento = 0.0f;

    float epsilon = powf(10.0f, -(casasDecimais+2));

    if((qAbs(p1) < 1.0f) && (qAbs(p2) < 1.0f))
        epsilon = powf(10.0, -(casasDecimais+1));
    else
        epsilon = powf(10.0, -(casasDecimais+2));

    if((qAbs(p1) < 1.0f) || (qAbs(p2) < 1.0f))
    {
        if(p1 >= 0.0f && p2 >= 0.0f)
            incremento = 1.0f;
        else if(p1 <= 0.0f && p2 <= 0.0f)
            incremento = -1.0f;
        else if((p1 >= 0.0f && p2 <= 0.0f) || (p1 <= 0.0f && p2 >= 0.0f))
            incremento = 2.0f;

        p1 += incremento;
        p2 += incremento;
    }

    float diff = qAbs(p1 - p2);

    if(diff <= epsilon)
        return true;

    float max = qMax(qAbs(p1), qAbs(p2));

    return (diff <= epsilon * max);
}

bool EhMenorOuIgual(double p1, double p2, int casasDecimais)
{
    return EhIgual(p1, p2, casasDecimais) || p1 < p2;
}

bool EhMenorOuIgual(float p1, float p2, int casasDecimais)
{
    return EhIgual(p1, p2, casasDecimais) || p1 < p2;
}

bool EhMaiorOuIgual(double p1, double p2, int casasDecimais)
{
    return EhIgual(p1, p2, casasDecimais) || p1 > p2;
}

bool EhMaiorOuIgual(float p1, float p2, int casasDecimais)
{
    return EhIgual(p1, p2, casasDecimais) || p1 > p2;
}

bool EstaDentroDoIntervalo(double valorMin, bool inclusoMin, double x, double valorMax, bool inclusoMax, int casasDecimais)
{
    bool resultado;

    if(inclusoMin)
        resultado = EhMenorOuIgual(valorMin, x, casasDecimais);
    else
        resultado = valorMin < x;

    if(inclusoMax)
        resultado = resultado && EhMenorOuIgual(x, valorMax, casasDecimais);
    else
        resultado = resultado && x < valorMax;


    return resultado;
}

bool EstaDentroDoIntervalo(float valorMin, bool inclusoMin, float x, float valorMax, bool inclusoMax, int casasDecimais)
{
    bool resultado;

    if(inclusoMin)
        resultado = EhMenorOuIgual(valorMin, x, casasDecimais);
    else
        resultado = valorMin < x;

    if(inclusoMax)
        resultado = resultado && EhMenorOuIgual(x, valorMax, casasDecimais);
    else
        resultado = resultado && x < valorMax;


    return resultado;
}
