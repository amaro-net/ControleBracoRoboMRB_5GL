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
#include "cinematica.h"
#include "utils.h"

Cinematica::Cinematica()
{
    M_PI_ARRED = arredondaPara(M_PI, CASAS_DECIMAIS_ROTACOES_XYZ_RAD);
    M_PI_2_ARRED = arredondaPara(M_PI_2, CASAS_DECIMAIS_ROTACOES_XYZ_RAD);
    M_PI_4_ARRED = arredondaPara(M_PI_4, CASAS_DECIMAIS_ROTACOES_XYZ_RAD);
}

double *Cinematica::coordenadasAngFixosOuEulerZXY(QMatrix4x4 T)
{
    double r11 = arredondaPara(double(T(0,0)), CASAS_DECIMAIS_SENOS_COSSENOS);
    double r21 = arredondaPara(double(T(1,0)), CASAS_DECIMAIS_SENOS_COSSENOS);
    double r31 = arredondaPara(double(T(2,0)), CASAS_DECIMAIS_SENOS_COSSENOS);
    double r12 = arredondaPara(double(T(0,1)), CASAS_DECIMAIS_SENOS_COSSENOS);
    double r22 = arredondaPara(double(T(1,1)), CASAS_DECIMAIS_SENOS_COSSENOS);
    double r32 = arredondaPara(double(T(2,1)), CASAS_DECIMAIS_SENOS_COSSENOS);
    //double r13 = arredondaPara(double(T(0,2)), CASAS_DECIMAIS_SENOS_COSSENOS);
    //double r23 = arredondaPara(double(T(1,2)), CASAS_DECIMAIS_SENOS_COSSENOS);
    double r33 = arredondaPara(double(T(2,2)), CASAS_DECIMAIS_SENOS_COSSENOS);

    double x = arredondaPara(double(T(0, 3)), CASAS_DECIMAIS_POSICAO_XYZ);
    double y = arredondaPara(double(T(1, 3)), CASAS_DECIMAIS_POSICAO_XYZ);
    double z = arredondaPara(double(T(2, 3)), CASAS_DECIMAIS_POSICAO_XYZ);

    double beta = atan2(-r31, sqrt(pow(r11, 2) + pow(r21, 2))) * 180 / M_PI;

    double alfa, gama;

    if(EhIgual(beta, 90.0, CASAS_DECIMAIS_ROTACOES_XYZ))
    {
        alfa = 0;
        gama = atan2(r12, r22) * 180 / M_PI;
    }
    else if(EhIgual(beta, -90.0, CASAS_DECIMAIS_ROTACOES_XYZ))
    {
        alfa = 0;
        gama = -atan2(r12, r22) * 180 / M_PI;
    }
    else
    {
        double cbeta = cos(beta * M_PI / 180);
        alfa = atan2(r21/cbeta, r11/cbeta) * 180 / M_PI;
        gama = atan2(r32/cbeta, r33/cbeta) * 180 / M_PI;
    }

    gama = arredondaPara(gama, CASAS_DECIMAIS_ROTACOES_XYZ);
    beta = arredondaPara(beta, CASAS_DECIMAIS_ROTACOES_XYZ);
    alfa = arredondaPara(alfa, CASAS_DECIMAIS_ROTACOES_XYZ);

    return new double[6]{x, y, z, gama, beta, alfa};
}


double* Cinematica::coordenadasElevacaoETorcao(QMatrix4x4 T, double teta2graus, double teta3graus, double teta4graus, double teta5graus)
{
    double x = arredondaPara(double(T(0, 3)), CASAS_DECIMAIS_POSICAO_XYZ);
    double y = arredondaPara(double(T(1, 3)), CASAS_DECIMAIS_POSICAO_XYZ);
    double z = arredondaPara(double(T(2, 3)), CASAS_DECIMAIS_POSICAO_XYZ);

    double elevacao, torcao;

    // TODO: Cinemática direta/inversa: Implementar a conversão da matriz da posição da garra para coordenadas de elevação e torção.
    elevacao = teta2graus + teta3graus + teta4graus - 90.0;
    torcao = teta5graus;

    return new double[5]{x, y, z, elevacao, torcao};
}

void Cinematica::coordenadasNoPlanoDoBracoRobo(double x, double y, double z,
                                               double px, double py, double pz,
                                               double teta1,
                                               double *xgl, double *zgl,
                                               double *pxl, double *pzl)
{
    double sinalxgl, sinalpxl;

    if( (teta1 < 0.0 && (y > 0.0)) ||
        (teta1 > 0.0 && (y < 0.0)) ||
        (EhIgual(teta1, 0.0, CASAS_DECIMAIS_POSICAO_ANGULAR_RAD) && x < 0))
        sinalxgl = -1;
    else
        sinalxgl = 1;

    *xgl = sinalxgl * sqrt(pow(x, 2) + pow(y, 2));

    *zgl = z - d1;


    if( (teta1 < 0.0 && (py > 0.0)) ||
        (teta1 > 0.0 && (py < 0.0)) ||
        (EhIgual(teta1, 0.0, CASAS_DECIMAIS_POSICAO_ANGULAR_RAD) && px < 0))
        sinalpxl = -1;
    else
        sinalpxl = 1;

    *pxl = sinalpxl * sqrt(pow(px, 2) + pow(py, 2));

    *pzl = pz - d1;
}

bool Cinematica::garraColideComBaseFixa(double x, double y, double z,
                                        double px, double py, double pz,
                                        QVector3D Zt)
{
    if(!checarColisao)
        return false;

    QVector3D v = static_cast<float>(LgL3) * Zt;
    QVector3D P(static_cast<float>(px), static_cast<float>(py), static_cast<float>(pz));
    QVector3D G(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z));

    Plano3D face1(0.0, 2.5, 0.0, 0.0, 1.0, 0.0, -5.0, 3.0, 2.5, 2.5, 0, 13.2);
    Plano3D face2(0.0, -2.5, 0.0, 0.0, -1.0, 0.0, -5.0, 3.0, -2.5, -2.5, 0, 13.2);
    Plano3D face3(3.0, 0.0, 0.0, 1.0, 0.0, 0.0, 3.0, 3.0, -2.5, 2.5, 0.0, 13.2);
    Plano3D face4(-3.0, 0.0, 0.0, -1.0, 0.0, 0.0, -3.0, -3.0, -2.5, 2.5, 0.0, 13.2);
    Plano3D face5(0.0, 0.0, 13.2, 0.0, 0.0, 1.0, -5.0, 3.0, -2.5, 2.5, 13.2, 13.2);
    Plano3D face6(0.0, 0.0, 0.0, 0.0, 0.0, 1.0, -34.34, 3.36, -9.95, 9.95, static_cast<double>(-INFINITY), 0.0);

    QList<Plano3D> faces;

    faces.append(face1);
    faces.append(face2);
    faces.append(face3);
    faces.append(face4);
    faces.append(face5);
    faces.append(face6);

    for(Plano3D face : faces)
    {
        QVector3D N(face.N);
        QVector3D P0(face.P0);

        float NescalarV = QVector3D::dotProduct(N, v);

        if(NescalarV != 0.0f)
        {
            QVector3D v0 = P0 - P;
            float NescalarV0 = QVector3D::dotProduct(N, v0);

            float t = NescalarV0/NescalarV;

            if(EstaDentroDoIntervalo(0.0f, true, t, 1.0f, true, CASAS_DECIMAIS_POSICAO_XYZ))
            {
                QVector3D Pc = P + v * t;

                if(face.contemPonto(Pc))
                    return true;
            }
        }
        else
        {
            if(face.contemPonto(G) || face.contemPonto(P))
                return true;
        }
    }

    return false;
}

bool Cinematica::garraColideComBaseGiratoria(double xgl, double zgl,
                                             double pxl, double pzl,
                                             double teta234)
{
    if(!checarColisao)
        return false;

    double tetag = teta234 - M_PI_2;

    if( (EhMenorOuIgual(xgl, 2.15, CASAS_DECIMAIS_POSICAO_XYZ) &&
         EhMenorOuIgual(zgl, 1.2, CASAS_DECIMAIS_POSICAO_XYZ)   ) ||
        (EhMenorOuIgual(pxl, 2.15, CASAS_DECIMAIS_POSICAO_XYZ) &&
         EhMenorOuIgual(pzl, 1.2, CASAS_DECIMAIS_POSICAO_XYZ))       )
    {
        return true;
    }
    else
    {
        if(!(EhIgual(tetag, M_PI_2_ARRED, CASAS_DECIMAIS_ROTACOES_XYZ_RAD) ||
             EhIgual(tetag, -M_PI_2_ARRED, CASAS_DECIMAIS_ROTACOES_XYZ_RAD)) )
        {
            double a = tan(tetag);
            double b = pzl - a * pxl;


            if(EstaDentroDoIntervalo(fmin(xgl, pxl), true, 2.15, fmax(xgl, pxl), true, CASAS_DECIMAIS_POSICAO_XYZ) &&
               EhMenorOuIgual(a * 2.15 + b, 1.2, CASAS_DECIMAIS_POSICAO_XYZ))
            {
                return true;
            }

        }
        else
        {
            if(EhIgual(2.15, pxl, CASAS_DECIMAIS_ROTACOES_XYZ) &&
               EstaDentroDoIntervalo(fmin(pzl, zgl), true, 1.2, fmax(pzl, zgl), true, CASAS_DECIMAIS_POSICAO_XYZ))
            {
                return true;
            }
        }
    }

    return false;
}

bool Cinematica::garraColideComSegmentoL1(double xgl, double zgl,
                                          double pxl, double pzl,
                                          double teta2)
{
    if(!checarColisao)
        return false;

    if(!(EhIgual(teta2, M_PI_2_ARRED, CASAS_DECIMAIS_ROTACOES_XYZ_RAD) ||
         EhIgual(teta2, -M_PI_2_ARRED, CASAS_DECIMAIS_ROTACOES_XYZ_RAD)) )
    {
        double al1 = tan(teta2);

        double denominador = (zgl - pzl - al1 * (xgl - pxl));

        if(!EhIgual(denominador, 0.0, CASAS_DECIMAIS_POSICAO_XYZ))
        {
            double t = (al1 * pxl - pzl)/denominador;

            if(!EstaDentroDoIntervalo(0.0, true, t, 1.0, true, CASAS_DECIMAIS_POSICAO_XYZ))
                return false;

            double xl = pxl + (xgl - pxl) * t;
            double zl = pzl + (zgl - pzl) * t;
            double zlJ2 = L1 * sin(teta2);
            double xlJ2 = L1 * cos(teta2);

            if(EhIgual(zl, al1 * xl, CASAS_DECIMAIS_POSICAO_XYZ) &&
               EstaDentroDoIntervalo(fmin(0.0, xlJ2), true, xl, fmax(0.0, xlJ2), true, CASAS_DECIMAIS_POSICAO_XYZ) &&
               EstaDentroDoIntervalo(fmin(0.0, zlJ2), true, zl, fmax(0.0, zlJ2), true, CASAS_DECIMAIS_POSICAO_XYZ)    )
                return true;
        }
    }
    else
    {
        double denominador = (xgl - pxl);

        if(!EhIgual(denominador, 0.0, CASAS_DECIMAIS_POSICAO_XYZ))
        {
            double t = -pxl / denominador;

            if(!EstaDentroDoIntervalo(0.0, true, t, 1.0, true, CASAS_DECIMAIS_POSICAO_XYZ))
                return false;

            double xl = pxl + (xgl - pxl) * t;
            double zl = pzl + (zgl - pzl) * t;

            double xlJ2 = 0.0;
            double zlJ2 = L1;

            if(EhIgual(xl, xlJ2, CASAS_DECIMAIS_POSICAO_XYZ) &&
               EstaDentroDoIntervalo(0.0, true, zl, zlJ2, true, CASAS_DECIMAIS_POSICAO_XYZ) )
                return true;
        }
    }

    return false;
}



/**
 * @brief MainWindow::cinematicaDireta
 * Função que retorna uma matriz 4 x 4 que faz a transformação
 * de coordenadas do referencial do pulso da garra para um referencial
 * que coincide com um ponto na superfície da base do braço robô, que
 * cruza com uma linha vertical que passa rente ao eixo da junta 0.
 * As 3 primeiras linhas e 3 primeiras colunas contém uma matriz de
 * rotação do referencial do pulso da garra, enquanto as 3 primeiras
 * linhas da quarta coluna possui as translações em x, y e z do referencial
 * do pulso da garra.
 *
 * @param teta1graus ângulo em graus da junta 0
 * @param teta2graus ângulo em graus da junta 1
 * @param teta3graus ângulo em graus da junta 2
 * @param teta4graus ângulo em graus da junta 3
 * @param teta5graus ângulo em graus da junta 4 (pulso da garra)
 * @return a matriz da cinemática direta, composta pela matriz de rotação e pela translação do referencial do pulso da garra.
 */
QMatrix4x4 Cinematica::cinematicaDireta(double teta1graus, double teta2graus, double teta3graus, double teta4graus, double teta5graus)
{    
    double teta1 = teta1graus * M_PI / 180;
    double teta2 = teta2graus * M_PI / 180;
    double teta3 = teta3graus * M_PI / 180;
    double teta4 = teta4graus * M_PI / 180;
    double teta5 = teta5graus * M_PI / 180;

    double s1 = sin(teta1);
    double s2 = sin(teta2);
    double s3 = sin(teta3);
    double s4 = sin(teta4);
    double s5 = sin(teta5);

    double c1 = cos(teta1);
    double c2 = cos(teta2);
    double c3 = cos(teta3);
    double c4 = cos(teta4);
    double c5 = cos(teta5);

    double s23 = c2*s3 + c3*s2;
    double c23 = c2*c3 - s2*s3;
    double c234 = c23*c4 - s23*s4;
    double s234 = s23*c4 + c23*s4;

    float r11 = float(c1*c234*c5 + s1*s5);
    float r21 = float(s1*c234*c5 - c1*s5);
    float r31 = float(s234*c5);
    float r12 = float( s1*c5 - c1*c234*s5);
    float r22 = float(-c1*c5 - s1*c234*s5);
    float r32 = float(-s234*s5);
    float r13 = float(c1*s234);
    float r23 = float(s1*s234);
    float r33 = float(-c234);

    double f = a1 + a2*c2 + a3*c23 + a4*c234 + d5*s234;
    const double d234 = d2 + d3 + d4;

    float px = float(s1 * d234 + c1 * f);
    float py = float(s1 * f - c1 * d234);
    float pz = float(d1 + a2*s2 + a3*s23 + a4*s234 - d5*c234);

    return QMatrix4x4(r11, r12, r13, px,
                      r21, r22, r23, py,
                      r31, r32, r33, pz,
                        0,   0,   0,  1);
}

/**
 * @brief MainWindow::matrizPosGarra
 * Matriz de posicionamento da garra. Esta matriz faz a translação
 * do referencial da garra para o pulso, do pulso para o cruzamento
 * entre os eixos das juntas 1 e 0 (com a função de cinemática direta),
 * e do cruzamento de J1 e J0 para a base do braço robô.
 *
 * @param teta1graus ângulo em graus da junta 0
 * @param teta2graus ângulo em graus da junta 1
 * @param teta3graus ângulo em graus da junta 2
 * @param teta4graus ângulo em graus da junta 3
 * @param teta5graus ângulo em graus da junta 4 (pulso da garra)
 * @return Matriz de posicionamento da garra
 */
QMatrix4x4 Cinematica::matrizPosGarra(double teta1graus, double teta2graus, double teta3graus, double teta4graus, double teta5graus,
                                      bool* colideComBaseFixa,
                                      bool* colideComBaseGir,
                                      bool* colideComSegmentoL1)
{
    QMatrix4x4 matrizPulsoParaBaseFixa = cinematicaDireta(teta1graus, teta2graus, teta3graus, teta4graus, teta5graus);

    QMatrix4x4 matrizGarraParaBaseFixa = matrizPulsoParaBaseFixa; // posição da garra é preenchida adiante

    float r13 = matrizPulsoParaBaseFixa(0,2);
    float r23 = matrizPulsoParaBaseFixa(1,2);
    float r33 = matrizPulsoParaBaseFixa(2,2);

    float px = matrizPulsoParaBaseFixa(0,3);
    float py = matrizPulsoParaBaseFixa(1,3);
    float pz = matrizPulsoParaBaseFixa(2,3);

    float x = px + float(LgL3) * r13;
    float y = py + float(LgL3) * r23;
    float z = pz + float(LgL3) * r33;

    QVector4D BPT = matrizGarraParaBaseFixa.column(3);

    BPT.setX(x);
    BPT.setY(y);
    BPT.setZ(z);

    matrizGarraParaBaseFixa.setColumn(3, BPT);

    if(colideComBaseFixa != nullptr)
    {
        QVector3D Zt(r13, r23, r33);

        *colideComBaseFixa = garraColideComBaseFixa(double(x), double(y), double(z),
                                                    double(px), double(py), double(pz),
                                                    Zt);
    }

    if(colideComBaseGir != nullptr || colideComSegmentoL1 != nullptr)
    {
        double xgl, zgl, pxl, pzl;

        coordenadasNoPlanoDoBracoRobo(double(x), double(y), double(z),
                                      double(px), double(py), double(pz),
                                      teta1graus * M_PI / 180.0,
                                      &xgl, &zgl,
                                      &pxl, &pzl);

        if(colideComBaseGir != nullptr)
        {
            double teta234 = (teta2graus + teta3graus + teta4graus) * M_PI / 180.0;

            teta234 = arredondaPara(teta234, CASAS_DECIMAIS_POSICAO_ANGULAR);

            *colideComBaseGir = garraColideComBaseGiratoria(xgl, zgl, pxl, pzl, teta234);
        }

        if(colideComSegmentoL1 != nullptr)
        {
            double teta2 = teta2graus * M_PI / 180.0;
            teta2 = arredondaPara(teta2, CASAS_DECIMAIS_POSICAO_ANGULAR_RAD);
            *colideComSegmentoL1 = garraColideComSegmentoL1(xgl, zgl, pxl, pzl, teta2);
        }
    }

    return matrizGarraParaBaseFixa;
}




/**
 * @brief MainWindow::posicaoGarra
 * Função que retorna as coordenadas x, y, z, Rx, Ry e Rz da garra em relação à base do braço robô,
 * sendo Rx, Ry e Rz as rotações, em graus, em torno dos eixos X, Y e Z da base da garra.
 * Lembrar de desalocar o ponteiro que receber o resultado desta função quando não for mais usado.
 * @param teta1graus ângulo em graus da junta 0
 * @param teta2graus ângulo em graus da junta 1
 * @param teta3graus ângulo em graus da junta 2
 * @param teta4graus ângulo em graus da junta 3
 * @param teta5graus ângulo em graus da junta 4 (pulso da garra)
 * @return vetor que contém nos 3 primeiros elementos as coordenadas X, Y e Z da garra em cm, e no restante os ângulos Rx, Ry e Rz em graus.
 */
double *Cinematica::posicaoGarra(double teta1graus, double teta2graus, double teta3graus, double teta4graus, double teta5graus,
                                 bool* colideComBaseFixa,
                                 bool* colideComBaseGir,
                                 bool* colideComSegmentoL1)
{
    QMatrix4x4 MGarra = matrizPosGarra(teta1graus, teta2graus, teta3graus, teta4graus, teta5graus,
                                       colideComBaseFixa,
                                       colideComBaseGir,
                                       colideComSegmentoL1);

    return coordenadasAngFixosOuEulerZXY(MGarra);
}


void Cinematica::avaliaAnguloTeta(double* teta, double tetaMin, double tetaMax,
                                  SolucaoCinematicaInversa* solucao,
                                  double* tetaSolucao, bool* tetaPossivel)
{
    if(EhMaiorOuIgual(*teta, tetaMin, CASAS_DECIMAIS_POSICAO_ANGULAR_RAD) &&
       EhMenorOuIgual(*teta, tetaMax, CASAS_DECIMAIS_POSICAO_ANGULAR_RAD))
    {
        if(tetaSolucao != nullptr)
            *tetaSolucao = *teta;
        if(tetaPossivel != nullptr)
            *tetaPossivel = true;
    }
    else
    {
        if(*teta < tetaMin)
        {
            if(tetaPossivel != nullptr)
                *tetaPossivel = (std::abs(tetaMin - *teta) < 0.5);

            *teta = tetaMin;
        }
        else if(*teta > tetaMax)
        {
            if(tetaPossivel != nullptr)
                *tetaPossivel = (std::abs(*teta - tetaMax) < 0.5);

            *teta = tetaMax;
        }

        if(tetaSolucao != nullptr)
            *tetaSolucao = *teta;

        if(solucao != nullptr && tetaPossivel != nullptr && solucao->possivel)
            solucao->possivel = *tetaPossivel;
    }
}


void Cinematica::abordagemGeometrica(double* teta2ptr, double teta2min, double teta2max,
                                     double* teta3ptr, double teta3min, double teta3max,
                                     double* teta4ptr, double teta4min, double teta4max,
                                     double* teta234ptr,
                                     double pxl, double pzl,
                                     SolucaoCinematicaInversa* solucao)
{
    double teta2, teta3, teta4, teta234 = *teta234ptr;

    double *solucaoTeta2 = nullptr,
           *solucaoTeta3 = nullptr,
           *solucaoTeta4 = nullptr;

    bool *solucaoTeta2Possivel = nullptr,
         *solucaoTeta3Possivel = nullptr,
         *solucaoTeta4Possivel = nullptr;

    if(solucao != nullptr)
    {
        solucaoTeta2 = &solucao->teta2;
        solucaoTeta3 = &solucao->teta3;
        solucaoTeta4 = &solucao->teta4;

        solucaoTeta2Possivel = &solucao->teta2possivel;
        solucaoTeta3Possivel = &solucao->teta3possivel;
        solucaoTeta4Possivel = &solucao->teta4possivel;
    }


    double pxl2pzl2 = pow(pxl, 2) + pow(pzl, 2);

    // Cálculo do teta3
    double c3 = (pxl2pzl2 - pow(a2, 2) - pow(a3, 2))/(2 * a2 * a3);

    // Se c3 for consideravelmente maior que 1.0 em módulo, a solução será dada como impossível
    bool c3EstourouLimite = false;
    if(!EhMenorOuIgual(std::abs(c3), 1.05, CASAS_DECIMAIS_SENOS_COSSENOS))
    {
        c3EstourouLimite = true;
    }

    // Se c3 for maior, em módulo, que 1.0, mesmo que por uma diferença menor
    // que 0.00000001 (por exemplo, o que não faria a solução ser necessariamente
    // impossível), força c3 a ter valor 1.0 em módulo
    if(c3 > 1.0)
        c3 = 1.0;
    else if(c3 < -1.0)
        c3 = -1.0;

    /* WARNING: Enquanto o ângulo máximo da junta 2 for zero, teta3 sempre será negativo
                Rever este trecho caso o ângulo máximo da junta 2 passe a ser positivo */
    double s3 = -sqrt(1 - pow(c3, 2));

    teta3 = atan2(s3, c3);

    avaliaAnguloTeta(&teta3, teta3min, teta3max, solucao, solucaoTeta3, solucaoTeta3Possivel);

    if(c3EstourouLimite)
    {
        *solucaoTeta3Possivel = false;
        solucao->possivel = false;
    }

    // Cálculo do teta2
    double beta2 = atan2(pzl, pxl);

    double cksi = (pxl2pzl2 + pow(a2, 2) - pow(a3, 2))/(2 * a2 * sqrt(pxl2pzl2));

    // Se cksi for consideravelmente maior que 1.05 em módulo, a solução será dada como impossível
    bool cksiEstourouLimite = false;
    if(!EhMenorOuIgual(std::abs(cksi), 1.05, CASAS_DECIMAIS_SENOS_COSSENOS))
    {
        cksiEstourouLimite = true;
    }

    // Se cksi for maior, em módulo, que 1.0, mesmo que por uma diferença menor
    // que 0.00000001 (por exemplo, o que não faria a solução ser necessariamente
    // impossível), força cksi a ter valor 1.0 em módulo
    if(cksi > 1.0)
        cksi = 1.0;
    else if(cksi < -1.0)
        cksi = -1.0;

    double sksi = sqrt(1 - pow(cksi, 2));

    double ksi = atan2(sksi, cksi);

    teta2 = beta2 + ksi;

    avaliaAnguloTeta(&teta2, teta2min, teta2max, solucao, solucaoTeta2, solucaoTeta2Possivel);

    if(cksiEstourouLimite)
    {
        *solucaoTeta2Possivel = false;
        solucao->possivel = false;
    }

    // Cálculo do teta4
    teta4 = teta234 - teta2 - teta3;

    bool solucaoPossivelTemp = solucao->possivel;
    avaliaAnguloTeta(&teta4, teta4min, teta4max, solucao, solucaoTeta4, solucaoTeta4Possivel);

    // Esta situação pode ocorrer quando teta234 dá negativo quando deveria dar maior
    // que 180 graus
    if(!(*solucaoTeta4Possivel) && *teta234ptr < 0)
    {
        solucao->possivel = solucaoPossivelTemp;

        teta234 = teta234 + 2 * M_PI;
        teta4 = teta234 - teta2 - teta3;

        avaliaAnguloTeta(&teta4, teta4min, teta4max, solucao, solucaoTeta4, solucaoTeta4Possivel);
    }

    *teta2ptr = teta2;
    *teta3ptr = teta3;
    *teta4ptr = teta4;
    *teta234ptr = teta234;
}

void Cinematica::calculaTeta2Teta3Teta4Singular(double* teta2, double* teta3, double* teta4,
                                                double* z,
                                                double *angulosMaxGraus, double *angulosMinGraus,
                                                bool *posicaoAtingivel)
{
    if(EhIgual(*z, d1+a2+a3+LgL3, CASAS_DECIMAIS_POSICAO_XYZ))
    {
        *teta2 = 90.0;
        *teta3 = 0.0;
        *teta4 = 90.0;
    }
    else if(*z < d1+a2+a3+LgL3)
    {
        SolucaoCinematicaInversa solucao;

        double teta2min = angulosMinGraus[1] * M_PI / 180.0;
        double teta2max = angulosMaxGraus[1] * M_PI / 180.0;

        double teta3min = angulosMinGraus[2] * M_PI / 180.0;
        double teta3max = angulosMaxGraus[2] * M_PI / 180.0;

        double teta4min = angulosMinGraus[3] * M_PI / 180.0;
        double teta4max = angulosMaxGraus[3] * M_PI / 180.0;

        /* Faz a abordagem geometrica do teta2, teta3 e teta4 */        
        double teta234 = M_PI; /* teta234 é a inclinação do segmento L3
                                   em relação à base da garra (90 graus)
                                   mais 90 graus. */        

        solucao.possivel = true;

        double pz = *z - LgL3;

        abordagemGeometrica(teta2, teta2min, teta2max,
                            teta3, teta3min, teta3max,
                            teta4, teta4min, teta4max,
                            &teta234,
                            0, pz - d1, &solucao);

        *teta2 = *teta2 * 180.0 / M_PI;
        *teta3 = *teta3 * 180.0 / M_PI;
        *teta4 = *teta4 * 180.0 / M_PI;

        if(posicaoAtingivel != nullptr)
            *posicaoAtingivel = solucao.possivel;
    }
    else // if(z > d1+a2+a3+LgL3)
    {
        if(posicaoAtingivel != nullptr)
            *posicaoAtingivel = false;

         *z = d1+a2+a3+LgL3;
         *teta2 = 90.0;
         *teta3 = 0.0;
         *teta4 = 90.0;
    }
}

double *Cinematica::vetorPosAngularSingular(double alfaGraus, int quadranteAlfa,
                                            double teta1,
                                            double *z,
                                            double *angulosMaxGraus, double *angulosMinGraus,
                                            bool *posicaoAtingivel)
{
    double teta2, teta3, teta4, teta5;
    double sinal180graus = 1;

    double teta5Min = angulosMinGraus[4];
    double teta5Max = angulosMaxGraus[4];

    while(quadranteAlfa <= 0)
    {
        quadranteAlfa += 4;
    }

    while(quadranteAlfa > 4)
    {
        quadranteAlfa -= 4;
    }

    if(quadranteAlfa == 1 || quadranteAlfa == 2)
        sinal180graus = -1;


    teta5 = alfaGraus + sinal180graus * 180.0 - teta1;

    if( ! EstaDentroDoIntervalo(teta5Min, true, teta5, teta5Max, true, CASAS_DECIMAIS_POSICAO_ANGULAR))
    {
        double difTeta5;

        if(teta5 < teta5Min)
        {
            difTeta5 = teta5 - teta5Min;
            teta5 = teta5Min;
        }
        else // if(teta5 > teta5Max)
        {
            difTeta5 = teta5 - teta5Max;
            teta5 = teta5Max;
        }

        teta1 = teta1 + difTeta5;
    }

    calculaTeta2Teta3Teta4Singular(&teta2, &teta3, &teta4,
                                   z,
                                   angulosMaxGraus,
                                   angulosMinGraus,
                                   posicaoAtingivel);

    return new double[5]{teta1, teta2, teta3, teta4, teta5};
}

double* Cinematica::calculaPosicaoSingular(double* z,
                                           double *gamaGraus, double *betaGraus, double *alfaGraus,
                                           double *angulosCorrentesJuntas,
                                           double *angulosMaxGraus,
                                           double *angulosMinGraus,
                                           bool *posicaoAtingivel)
{
    double teta1;
    double teta1Corrente = angulosCorrentesJuntas[0];    

    if(EhIgual(*gamaGraus, 0.0, CASAS_DECIMAIS_ROTACOES_XYZ) &&
       EhIgual(*betaGraus, 0.0, CASAS_DECIMAIS_ROTACOES_XYZ))
    {
        if(*alfaGraus > 180.0)
        {
            do
                *alfaGraus = *alfaGraus - 360.0;
            while (*alfaGraus > 180.0);
        }
        else if(*alfaGraus < -180.0)
        {
            do
                *alfaGraus = *alfaGraus + 360.0;
            while (*alfaGraus < -180.0);
        }

        // Primeiro quadrante de alfa
        if(EstaDentroDoIntervalo(0.0, true, *alfaGraus, 90.0, true, CASAS_DECIMAIS_ROTACOES_XYZ))
        {            
            if (   EstaDentroDoIntervalo(-90.0, true, teta1Corrente, 0.0, false, CASAS_DECIMAIS_POSICAO_ANGULAR)
                || EstaDentroDoIntervalo(90.0, false, teta1Corrente, 100.0, true, CASAS_DECIMAIS_POSICAO_ANGULAR))
                teta1 = teta1Corrente;
            else if(EstaDentroDoIntervalo(0.0, true, *alfaGraus, 10.0, true, CASAS_DECIMAIS_ROTACOES_XYZ))
                teta1 = 100.0;
            else
                teta1 = -90.0;

            return vetorPosAngularSingular(*alfaGraus, 1,
                                           teta1,
                                           z,
                                           angulosMaxGraus, angulosMinGraus,
                                           posicaoAtingivel);
        }
        // Segundo quadrante de alfa
        else if(EstaDentroDoIntervalo(90.0, false, *alfaGraus, 180.0, true, CASAS_DECIMAIS_ROTACOES_XYZ))
        {            
            if (EstaDentroDoIntervalo(0.0, true, teta1Corrente, 90.0, false, CASAS_DECIMAIS_POSICAO_ANGULAR))
                teta1 = teta1Corrente;
            else
                teta1 = -90.0;

            return vetorPosAngularSingular(*alfaGraus, 2,
                                           teta1,
                                           z,
                                           angulosMaxGraus, angulosMinGraus,
                                           posicaoAtingivel);
        }
        // Terceiro quadrante de alfa
        else if(EstaDentroDoIntervalo(-180.0, true, *alfaGraus, -90.0, true, CASAS_DECIMAIS_ROTACOES_XYZ))
        {            
            if (EstaDentroDoIntervalo(0.0, true, teta1Corrente, 100.0, true, CASAS_DECIMAIS_POSICAO_ANGULAR))
                teta1 = teta1Corrente;
            else
                teta1 = 0.0;

            return vetorPosAngularSingular(*alfaGraus, 3,
                                           teta1,
                                           z,
                                           angulosMaxGraus, angulosMinGraus,
                                           posicaoAtingivel);
        }
        // Quarto quadrante de alfa
        else if(EstaDentroDoIntervalo(-90.0, false, *alfaGraus, 0.0, false, CASAS_DECIMAIS_ROTACOES_XYZ))
        {            
            if (EstaDentroDoIntervalo(0.0, false, teta1Corrente, 100.0, true, CASAS_DECIMAIS_POSICAO_ANGULAR))
                teta1 = teta1Corrente;
            else
                teta1 = 90.0;

            return vetorPosAngularSingular(*alfaGraus, 4,
                                           teta1,
                                           z,
                                           angulosMaxGraus, angulosMinGraus,
                                           posicaoAtingivel);
        }
    }

    return nullptr;
}

void Cinematica::trazPosAlvoParaDentroDoEspacoDeTrabalho(double *x, double *y, double *z)
{
    double De = a2 + a3 + LgL3;
    double x2y2 = pow(*x, 2) + pow(*y, 2);
    double zd1 = *z - d1;

    double D = sqrt(x2y2 + pow(zd1, 2));

    if(D > De)
    {
        double Dxy = sqrt(x2y2);

        double xe = 0;
        double ye = 0;

        double stetaElev = zd1 / D;
        double ctetaElev = Dxy / D;

        if(Dxy > 0)
        {
            double Dexy = De * ctetaElev;
            double ctetaAzim = *x / Dxy;
            double stetaAzim = *y / Dxy;
            xe = Dexy * ctetaAzim;
            ye = Dexy * stetaAzim;
        }

        double ze = De * stetaElev + d1;

        *x = xe;
        *y = ye;
        *z = ze;
    }
}

void Cinematica::ajustaSenoOuCossenoIgualA1(float* steta, float* cteta)
{
    if (EhMaiorOuIgual(*steta, 1.0f, CASAS_DECIMAIS_SENOS_COSSENOS))
    {
        *steta = 1.0f;
        *cteta = 0.0f;
    }
    else if(EhMaiorOuIgual(*cteta, 1.0f, CASAS_DECIMAIS_SENOS_COSSENOS))
    {
        *cteta = 1.0f;
        *steta = 0.0f;
    }
    else if(EhMenorOuIgual(*steta, -1.0f, CASAS_DECIMAIS_SENOS_COSSENOS))
    {
        *steta = -1.0f;
        *cteta = 0.0f;
    }
    else if(EhMenorOuIgual(*cteta, -1.0f, CASAS_DECIMAIS_SENOS_COSSENOS))
    {
        *cteta = -1.0f;
        *steta = 0.0f;
    }
}

void Cinematica::ajustaSenoOuCossenoIgualA1(double* steta, double* cteta)
{
    if (EhMaiorOuIgual(*steta, 1.0, CASAS_DECIMAIS_SENOS_COSSENOS))
    {
        *steta = 1.0;
        *cteta = 0.0;
    }
    else if(EhMaiorOuIgual(*cteta, 1.0, CASAS_DECIMAIS_SENOS_COSSENOS))
    {
        *cteta = 1.0;
        *steta = 0.0;
    }
    else if(EhMenorOuIgual(*steta, -1.0, CASAS_DECIMAIS_SENOS_COSSENOS))
    {
        *steta = -1.0;
        *cteta = 0.0;
    }
    else if(EhMenorOuIgual(*cteta, -1.0, CASAS_DECIMAIS_SENOS_COSSENOS))
    {
        *cteta = -1.0;
        *steta = 0.0;
    }
}

// TODO: Cinemática direta: Criar um método para calcular o tamanho da abertura da garra com base no ângulo do eixo do servo da garra.
// TODO: Cinemática direta: Criar a cinemática direta para os dedos da garra, calculando o XYZ de cada dedo.
// TODO: Cinemática direta/inversa: incluir tratamento para evitar cálculo de posições em que os dedos da garra colidem com a base fixa, com a base giratória ou com o segmento L1

/**
 * @brief MainWindow::angJuntas
 *
 * Cinemática inversa
 *
 * @param x coordenada x da garra em relação à base da garra. Esta coordenada poderá ser recalculada, caso ela não seja atingível.
 * @param y coordenada y da garra em relação à base da garra. Esta coordenada poderá ser recalculada, caso ela não seja atingível.
 * @param z coordenada z da garra em relação à base da garra. Esta coordenada poderá ser recalculada, caso ela não seja atingível.
 * @param gamaGraus
 * @param betaGraus
 * @param alfaGraus
 * @param angulosMaxGraus vetor contendo os ângulos máximo das juntas.
 * @param angulosMinGraus vetor contendo os ângulos mínimo das juntas.
 * @param posicaoProjetada true se a posição desejada foi projetada no plano vertical do braço robô, e qualquer das coordenadas (XYZ ou rotações) foi alterada. false caso contrário.
 * @param posicaoAtingivel true se a posição (projetada ou não) for atingível. false se qualquer das juntas assumir um ângulo impossível.
 * @return ângulos das juntas em graus, se a posição de alguma forma for atingível.
 */
double *Cinematica::angJuntas(double *x, double *y, double *z,
                              double *gamaGraus, double *betaGraus, double *alfaGraus,
                              double *angulosCorrentesJuntas,
                              double *angulosMaxGraus, double *angulosMinGraus,
                              bool *posicaoProjetada, bool *posicaoAtingivel,
                              bool *colideComBaseFixa, bool *colideComBaseGir, bool *colideComSegmentoL1)
{    
    double teta1, teta2, teta3, teta4, teta5;

    if(posicaoAtingivel != nullptr)
        *posicaoAtingivel = true;

    if(colideComBaseFixa != nullptr)
        *colideComBaseFixa = false;

    if(colideComBaseGir != nullptr)
        *colideComBaseGir = false;

    if(colideComSegmentoL1 != nullptr)
        *colideComSegmentoL1 = false;

    if(EhIgual(*x, 0.0, CASAS_DECIMAIS_POSICAO_XYZ) &&
       EhIgual(*y, 0.0, CASAS_DECIMAIS_POSICAO_XYZ))
    {
        double* angulosJuntas = calculaPosicaoSingular(z,
                                                       gamaGraus, betaGraus, alfaGraus,
                                                       angulosCorrentesJuntas,
                                                       angulosMaxGraus,
                                                       angulosMinGraus,
                                                       posicaoAtingivel);

        if(angulosJuntas != nullptr)
        {
            if(posicaoProjetada != nullptr)
                *posicaoProjetada = false;
            return angulosJuntas;
        }
    }

    double gama = *gamaGraus * M_PI /180.0;
    double beta = *betaGraus * M_PI /180.0;
    double alfa = *alfaGraus * M_PI /180.0;

    double sgama = arredondaPara(sin(gama), CASAS_DECIMAIS_SENOS_COSSENOS);
    double sbeta = arredondaPara(sin(beta), CASAS_DECIMAIS_SENOS_COSSENOS);
    double salfa = arredondaPara(sin(alfa), CASAS_DECIMAIS_SENOS_COSSENOS);

    double cgama = arredondaPara(cos(gama), CASAS_DECIMAIS_SENOS_COSSENOS);
    double cbeta = arredondaPara(cos(beta), CASAS_DECIMAIS_SENOS_COSSENOS);
    double calfa = arredondaPara(cos(alfa), CASAS_DECIMAIS_SENOS_COSSENOS);

    ajustaSenoOuCossenoIgualA1(&sgama, &cgama);
    ajustaSenoOuCossenoIgualA1(&sbeta, &cbeta);
    ajustaSenoOuCossenoIgualA1(&salfa, &calfa);

    double r11 = calfa * cbeta;
    double r21 = salfa * cbeta;
    double r31 = -sbeta;
    double r12 = calfa * sbeta * sgama - salfa * cgama;
    double r22 = salfa * sbeta * sgama + calfa * cgama;
    double r32 = cbeta * sgama;
    double r13 = calfa * sbeta * cgama + salfa * sgama;
    double r23 = salfa * sbeta * cgama - calfa * sgama;
    double r33 = cbeta * cgama;

    double x2y2 = pow(*x, 2.0) + pow(*y, 2.0);
    double sqrtx2y2 = sqrt(x2y2);

    trazPosAlvoParaDentroDoEspacoDeTrabalho(x, y, z);

    // Projetando a orientação desejada no plano do braço. Esta projeção dirá a posição que a
    // garra poderá verdadeiramente assumir (ver cap. 4 do livro do Craig - The Yasukawa
    // Motoman L-3 página 121) ou o documento dos cálculos (calculos.pdf)

    // WARNING: Ponto da garra e origem do referencial da base: Notar que o ponto da garra (e o ponto do pulso) não está rente com a origem do referencial da base fixa.
    // WARNING: Vetor M e plano do braço robô: O plano cuja normal é M passa pela origem do referencial da base (não pega o ponto da garra nem o pulso J4).
    //          Isso se deve ao uso dos parâmetros DH d2, d3 e d4 estarem zerados. Com isso, o ponto da garra na cinemática direta (e na cinemática inversa,
    //          consequentemente) está deslocado em relação ponto que de fato seria o da garra (ver calculos.pdf). Se fossem usados parâmetros exatos, o vetor
    //          M iria definir um plano oblíquo, que passaria pelo ponto da garra e pela origem da base fixa, que faria o vetor Ztl apontar sempre para uma
    //          direção diferente da posição alvo, mesmo que a posição alvo, na prática, não precisasse ser projetada.
    // WARNING: Ver warnings do arquivo constantes.h referentes aos parâmetros d2, d3, d4 e d5        
    QVector3D M;

    if(sqrtx2y2 > 0)
    {
        M.setX(float(-(*y/sqrtx2y2)));
        M.setY(float(*x/sqrtx2y2));
        M.setZ(0.0f);
    }
    else
    {
        M.setX(float(-salfa));
        M.setY(float(calfa));
        M.setZ(0.0);
    }

    QVector3D Zt(static_cast<float>(r13), static_cast<float>(r23), static_cast<float>(r33));
    QVector3D Yt(static_cast<float>(r12), static_cast<float>(r22), static_cast<float>(r32));

    Zt.normalize();
    Yt.normalize();

    QVector3D K = QVector3D::crossProduct(M, Zt);

    K.normalize();

    QVector3D Ztl = QVector3D::crossProduct(K, M);

    Ztl.normalize();

    /* WARNING: Vetor Ztl:  Se forem respeitados os parâmetros exatos de Denavit-Hatenberg obtidos para o braço robô (mais
     * especificamente d2, d3 e d4), Ztl sempre apontará para uma direção diferente do Zt, mesmo que a posição
     * da garra seja uma posição possível, por conta do vetor M calculado (ver warning referente ao vetor M e ao plano ao
     * qual ele é perpendicular).
     */
    float cteta = QVector3D::dotProduct(Zt, Ztl);
    float steta = QVector3D::dotProduct(QVector3D::crossProduct(Zt, Ztl), K);

    steta = arredondaPara(steta, CASAS_DECIMAIS_SENOS_COSSENOS);
    cteta = arredondaPara(cteta, CASAS_DECIMAIS_SENOS_COSSENOS);

    ajustaSenoOuCossenoIgualA1(&steta, &cteta);

    double teta = atan2(static_cast<double>(steta), static_cast<double>(cteta)) * 180.0 / M_PI;

    teta = arredondaPara(teta, CASAS_DECIMAIS_ROTACOES_XYZ);
    teta = abs(teta);

    if(teta > 0.0)
    {
        QVector3D Ytl = cteta * Yt
                      + steta * QVector3D::crossProduct(K, Yt)
                      + ((1 - cteta) * QVector3D::dotProduct(K, Yt)) * K;

        Ytl.normalize();

        QVector3D Xtl = QVector3D::crossProduct(Ytl, Ztl);

        Xtl.normalize();

        for(int i = 0; i < 3; i++)
        {
            Xtl[i] = arredondaPara(Xtl[i], CASAS_DECIMAIS_SENOS_COSSENOS);
            Ytl[i] = arredondaPara(Ytl[i], CASAS_DECIMAIS_SENOS_COSSENOS);
            Ztl[i] = arredondaPara(Ztl[i], CASAS_DECIMAIS_SENOS_COSSENOS);
        }

        r11 = double(Xtl[0]);
        r21 = double(Xtl[1]);
        r31 = double(Xtl[2]);

        r12 = double(Ytl[0]);
        r22 = double(Ytl[1]);
        r32 = double(Ytl[2]);

        r13 = double(Ztl[0]);
        r23 = double(Ztl[1]);
        r33 = double(Ztl[2]);        

        //recalculando gama, beta e alfa
        double betaProj = atan2(-r31, sqrt(pow(r11,2)+pow(r21,2)));
        double alfaProj, gamaProj;

        if (EhIgual(betaProj, M_PI_2_ARRED, CASAS_DECIMAIS_ROTACOES_XYZ_RAD))
        {
            alfaProj = 0;
            gamaProj = atan2(r12, r22);
        }
        else if (EhIgual(betaProj, -M_PI_2_ARRED, CASAS_DECIMAIS_ROTACOES_XYZ_RAD))
        {
            alfaProj = 0;
            gamaProj = -atan2(r12, r22);
        }
        else
        {
            double cbeta = cos(betaProj);
            cbeta = arredondaPara(cbeta, CASAS_DECIMAIS_SENOS_COSSENOS);
            alfaProj = atan2(r21/cbeta, r11/cbeta);
            gamaProj = atan2(r32/cbeta, r33/cbeta);
        }       

        double alfaGrausProj = alfaProj * 180.0 / M_PI;
        double betaGrausProj = betaProj * 180.0 / M_PI;
        double gamaGrausProj = gamaProj * 180.0 / M_PI;

        alfaGrausProj = arredondaPara(alfaGrausProj, CASAS_DECIMAIS_ROTACOES_XYZ);
        betaGrausProj = arredondaPara(betaGrausProj, CASAS_DECIMAIS_ROTACOES_XYZ);
        gamaGrausProj = arredondaPara(gamaGrausProj, CASAS_DECIMAIS_ROTACOES_XYZ);

        if(posicaoProjetada != nullptr)
            *posicaoProjetada = true;

        *gamaGraus = gamaGrausProj;
        *betaGraus = betaGrausProj;
        *alfaGraus = alfaGrausProj;

        if(EhIgual(*x, 0.0, CASAS_DECIMAIS_POSICAO_XYZ) &&
           EhIgual(*y, 0.0, CASAS_DECIMAIS_POSICAO_XYZ))
        {
            double* angulosJuntas = calculaPosicaoSingular(z,
                                                           gamaGraus, betaGraus, alfaGraus,
                                                           angulosCorrentesJuntas,
                                                           angulosMaxGraus,
                                                           angulosMinGraus,
                                                           posicaoAtingivel);
            if(angulosJuntas != nullptr)
                return angulosJuntas;
        }
    }

    double px = *x - LgL3 * r13;
    double py = *y - LgL3 * r23;
    double pz = *z - LgL3 * r33;

    double px2py2 = pow(px, 2) + pow(py, 2);

    bool bColideComBaseFixa = false;

    if(garraColideComBaseFixa(*x, *y, *z, px, py, pz, Ztl))
    {
        bColideComBaseFixa = true;
        // TODO: Cinemática inversa: Implementar um mecanismo de reposicionamento para a posição mais próxima em caso de colisão com a base fixa
    }

    // ***** Início da cinemática inversa propriamente dita ****

    double teta1min = angulosMinGraus[0] * M_PI / 180;
    double teta1max = angulosMaxGraus[0] * M_PI / 180;

    double teta2min = angulosMinGraus[1] * M_PI / 180;
    double teta2max = angulosMaxGraus[1] * M_PI / 180;

    double teta3min = angulosMinGraus[2] * M_PI / 180;
    double teta3max = angulosMaxGraus[2] * M_PI / 180;

    double teta4min = angulosMinGraus[3] * M_PI / 180;
    double teta4max = angulosMaxGraus[3] * M_PI / 180;

    double teta5min = angulosMinGraus[4] * M_PI / 180;
    double teta5max = angulosMaxGraus[4] * M_PI / 180;

    SolucaoCinematicaInversa *solucao;
    QList<SolucaoCinematicaInversa *> solucoes;
    QList<double> solucaoTeta1;

    const double d234 = d2 + d3 + d4;
    const double d234quad = d234 * d234;

    // Cálculo do teta1 (todas as soluções possíveis)
    double atan2pypx = atan2(-px, py);
    double atan2sqrt = atan2(sqrt(px2py2 - d234quad),-d234);

    double teta1_1 = atan2pypx + atan2sqrt;
    double teta1_2 = atan2pypx - atan2sqrt;
    double teta1_3 = atan2(r23, r13);
    double teta1_4 = atan2(-r23, -r13);

    if(EhMaiorOuIgual(teta1_1, teta1min, CASAS_DECIMAIS_POSICAO_ANGULAR_RAD) &&
       EhMenorOuIgual(teta1_1, teta1max, CASAS_DECIMAIS_POSICAO_ANGULAR_RAD))
    {
        solucaoTeta1.append(teta1_1);
    }

    if(EhMaiorOuIgual(teta1_2, teta1min, CASAS_DECIMAIS_POSICAO_ANGULAR_RAD) &&
       EhMenorOuIgual(teta1_2, teta1max, CASAS_DECIMAIS_POSICAO_ANGULAR_RAD))
    {
        if(!solucaoTeta1.contains(teta1_2))
            solucaoTeta1.append(teta1_2);
    }

    if(EhMaiorOuIgual(teta1_3, teta1min, CASAS_DECIMAIS_POSICAO_ANGULAR_RAD) &&
       EhMenorOuIgual(teta1_3, teta1max, CASAS_DECIMAIS_POSICAO_ANGULAR_RAD))
    {
        if(!solucaoTeta1.contains(teta1_3))
            solucaoTeta1.append(teta1_3);
    }

    if(EhMaiorOuIgual(teta1_4, teta1min, CASAS_DECIMAIS_POSICAO_ANGULAR_RAD) &&
       EhMenorOuIgual(teta1_4, teta1max, CASAS_DECIMAIS_POSICAO_ANGULAR_RAD))
    {
        if(!solucaoTeta1.contains(teta1_4))
            solucaoTeta1.append(teta1_4);
    }

    if(solucaoTeta1.isEmpty())
    {
        if (teta1_1 < teta1min || teta1_2 < teta1min || teta1_3 < teta1min || teta1_4 < teta1min)
        {
            teta1 = teta1min;

        }
        else /*if (teta1_1 > teta1max || teta1_2 > teta1max || teta1_3 > teta1max || teta1_4 > teta1max)*/
        {
            teta1 = teta1max;
        }

        if(posicaoAtingivel != nullptr)
            *posicaoAtingivel = false;

        solucao = new SolucaoCinematicaInversa;

        solucao->teta1 = teta1;

        solucao->s1 = arredondaPara(sin(teta1), CASAS_DECIMAIS_SENOS_COSSENOS);
        solucao->c1 = arredondaPara(cos(teta1), CASAS_DECIMAIS_SENOS_COSSENOS);
        ajustaSenoOuCossenoIgualA1(&solucao->s1, &solucao->c1);
        solucao->possivel = false;
        solucao->teta1possivel = false;

        solucoes.append(solucao);
    }
    else
    {
        for(int i = 0; i < solucaoTeta1.count(); i++)
        {
            solucao = new SolucaoCinematicaInversa;

            teta1 = solucaoTeta1.at(i);

            solucao->teta1 = teta1;
            solucao->s1 = arredondaPara(sin(teta1), CASAS_DECIMAIS_SENOS_COSSENOS);
            solucao->c1 = arredondaPara(cos(teta1), CASAS_DECIMAIS_SENOS_COSSENOS);
            ajustaSenoOuCossenoIgualA1(&solucao->s1, &solucao->c1);
            solucao->possivel = true;
            solucao->teta1possivel = true;

            solucoes.append(solucao);
        }
    }

    // Cálculo do teta2, teta3, teta4 e teta5
    for(int i = 0; i < solucoes.count(); i++)
    {
        solucao = solucoes.at(i);

        teta1 = solucoes.at(i)->teta1;
        double s1 = solucoes.at(i)->s1;
        double c1 = solucoes.at(i)->c1;

        double s5 = s1 * r11 - c1 * r21;
        double c5 = s1 * r12 - c1 * r22;

        teta5 = atan2(s5, c5);

        avaliaAnguloTeta(&teta5, teta5min, teta5max, solucao, &solucao->teta5, &solucao->teta5possivel);

        double c234, s234, teta234;

        s234 = c1 * r13 + s1 * r23;
        c234 = -r33;

        teta234 = atan2(s234, c234);

        double xgl, zgl, pxl, pzl;

        coordenadasNoPlanoDoBracoRobo(*x, *y, *z,
                                      px, py, pz,
                                      teta1,
                                      &xgl, &zgl,
                                      &pxl, &pzl);        

        abordagemGeometrica(&teta2, teta2min, teta2max,
                            &teta3, teta3min, teta3max,
                            &teta4, teta4min, teta4max,
                            &teta234,
                            pxl, pzl,
                            solucao);

        solucao->colideComBaseGir = garraColideComBaseGiratoria(xgl, zgl, pxl, pzl, teta234);
        solucao->colideComSegmentoL1 = garraColideComSegmentoL1(xgl, zgl, pxl, pzl, teta2);
    }



    // Avaliação final das soluções
    if(solucoes.count() == 1)
    {
        solucao = solucoes[0];

        if(posicaoAtingivel != nullptr)
        {
            *posicaoAtingivel = solucao->possivel;
        }
    }
    else
    {
        // Ver no livro do Craig páginas 104 e 105 sobre pesos das juntas e solução de menor peso.
        QList<SolucaoCinematicaInversa *> solucoesValidas;

        for(int i = 0; i < solucoes.count(); i++)
        {
            solucao = solucoes.at(i);

            // Inclui a solução na lista de soluções válidas
            if(solucao->possivel)
            {
                solucoesValidas.append(solucao);
            }
        }

        // Se não existir solução válida
        if(solucoesValidas.isEmpty())
        {
            int maiorQtdPosicoesPossiveis = 0;
            int idxSolMaiorQtdAngPossivel = 0;

            // Pega a solução inválida com maior quantidade de ângulos válidos
            for(int i = 0; i < solucoes.count(); i++)
            {
                int qtd = 0;

                if(solucoes.at(i)->teta1possivel)
                    qtd++;

                if(solucoes.at(i)->teta2possivel)
                    qtd++;

                if(solucoes.at(i)->teta3possivel)
                    qtd++;

                if(solucoes.at(i)->teta4possivel)
                    qtd++;

                if(solucoes.at(i)->teta5possivel)
                    qtd++;

                if(qtd > maiorQtdPosicoesPossiveis)
                {
                    maiorQtdPosicoesPossiveis = qtd;
                    idxSolMaiorQtdAngPossivel = i;
                }
            }

            solucao = solucoes.at(idxSolMaiorQtdAngPossivel);
        }
        else if(solucoesValidas.count() == 1)
        {
            solucao = solucoesValidas.at(0);

            if(posicaoAtingivel != nullptr)
            {
                *posicaoAtingivel = solucao->possivel;
            }
        }
        else
        {
            // Obtém a solução válida de menor peso, ou a primeira, em caso de pesos iguais
            double difTeta[QTD_SERVOS - 1];
            double angTeta[QTD_SERVOS - 1];
            double pesosTeta[QTD_SERVOS - 1];

            pesosTeta[0] = pesoTeta1;
            pesosTeta[1] = pesoTeta2;
            pesosTeta[2] = pesoTeta3;
            pesosTeta[3] = pesoTeta4;
            pesosTeta[4] = pesoTeta5;

            for(int i = 0; i < solucoesValidas.count(); i++)
            {
                solucao = solucoesValidas.at(i);

                angTeta[0] = solucao->teta1 * 180.0 / M_PI;
                angTeta[1] = solucao->teta2 * 180.0 / M_PI;
                angTeta[2] = solucao->teta3 * 180.0 / M_PI;
                angTeta[3] = solucao->teta4 * 180.0 / M_PI;
                angTeta[4] = solucao->teta5 * 180.0 / M_PI;

                for(int j = 0; j < QTD_SERVOS - 1; j++)
                    difTeta[j] = std::abs(angulosCorrentesJuntas[j] - angTeta[j]);

                double somatorio = 0, somaPesos = 0;

                for(int j = 0; j < QTD_SERVOS - 1; j++)
                {
                    somatorio += pesosTeta[j] * difTeta[j];
                    somaPesos += pesosTeta[j];
                }

                solucao->peso = somatorio/somaPesos;
            }

            int idxSolucaoPesoMenor = 0;
            double menorPeso = solucoesValidas.at(0)->peso;

            for(int i = 1; i < solucoesValidas.count(); i++)
            {
                if(solucoesValidas.at(i)->peso < menorPeso)
                {
                    menorPeso = solucoesValidas.at(i)->peso;
                    idxSolucaoPesoMenor = i;
                }
            }

            solucao = solucoesValidas.at(idxSolucaoPesoMenor);
        }
    }    

    if(colideComBaseFixa != nullptr)
        *colideComBaseFixa = bColideComBaseFixa;

    if(posicaoAtingivel != nullptr)
    {
        *posicaoAtingivel = solucao->possivel &&
                            !(bColideComBaseFixa ||
                              solucao->colideComBaseGir ||
                              solucao->colideComSegmentoL1);
    }

    teta1 = solucao->teta1;
    teta2 = solucao->teta2;
    teta3 = solucao->teta3;
    teta4 = solucao->teta4;
    teta5 = solucao->teta5;

    if(colideComBaseGir != nullptr)
        *colideComBaseGir = solucao->colideComBaseGir;
    if(colideComSegmentoL1 != nullptr)
        *colideComSegmentoL1 = solucao->colideComSegmentoL1;

    while(!solucoes.isEmpty())
    {
        delete solucoes.takeFirst();
    }

    teta1 = teta1 * 180.0 / M_PI;
    teta2 = teta2 * 180.0 / M_PI;
    teta3 = teta3 * 180.0 / M_PI;
    teta4 = teta4 * 180.0 / M_PI;
    teta5 = teta5 * 180.0 / M_PI;

    teta1 = arredondaPara(teta1, CASAS_DECIMAIS_POSICAO_ANGULAR);
    teta2 = arredondaPara(teta2, CASAS_DECIMAIS_POSICAO_ANGULAR);
    teta3 = arredondaPara(teta3, CASAS_DECIMAIS_POSICAO_ANGULAR);
    teta4 = arredondaPara(teta4, CASAS_DECIMAIS_POSICAO_ANGULAR);
    teta5 = arredondaPara(teta5, CASAS_DECIMAIS_POSICAO_ANGULAR);

    return new double[5]{teta1, teta2, teta3, teta4, teta5};
}
