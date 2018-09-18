#include "utils.h"
#include <cmath>

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
