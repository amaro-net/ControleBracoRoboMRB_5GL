#ifndef UTILS_H
#define UTILS_H

#include <qglobal.h>

float arredondaPara(float num, int casasDecimais);
double arredondaPara(double num, int casasDecimais);
bool EhIgual(double p1, double p2, int casasDecimais);
bool EhIgual(float p1, float p2, int casasDecimais);
bool EhMenorOuIgual(double p1, double p2, int casasDecimais);
bool EhMenorOuIgual(float p1, float p2, int casasDecimais);
bool EhMaiorOuIgual(double p1, double p2, int casasDecimais);
bool EhMaiorOuIgual(float p1, float p2, int casasDecimais);

#endif // UTILS_H
