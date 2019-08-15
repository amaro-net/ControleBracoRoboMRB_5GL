#include "utils.h"

float arredondaPara(float num, int casasDecimais)
{
    return float(arredondaPara(double(num), casasDecimais));
}


double arredondaPara(double num, int casasDecimais)
{
    //char charArrayNumArred[50];

    // TODO: Arredondamentos: verificar, se possível, o porquê de alguns arredondamentos não funcionarem como esperado.
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

    return (qAbs(p1 - p2) * pow(10.0, casasDecimais) <= qMin(qAbs(p1), qAbs(p2)));
}

bool EhIgual(float p1, float p2, int casasDecimais)
{
    float incremento = 0.0f;

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

    return (qAbs(p1 - p2) * powf(10.0f, casasDecimais) <= qMin(qAbs(p1), qAbs(p2)));
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
