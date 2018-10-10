#include "cinematica.h"
#include "utils.h"

Cinematica::Cinematica()
{

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

    // TODO: Implementar a conversão da matriz da posição da garra para coordenadas de elevação e torção.
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


bool Cinematica::pontoPertenceASegmentoDeReta(double xc, double yc, double zc,
                                              double x1, double y1, double z1,
                                              double x2, double y2, double z2)
{
    double xMaior = fmax(x1, x2);
    double xMenor = fmin(x1, x2);
    double yMaior = fmax(y1, y2);
    double yMenor = fmin(y1, y2);
    double zMaior = fmax(z1, z2);
    double zMenor = fmin(z1, z2);

    if(EstaDentroDoIntervalo(xMenor, true, xc, xMaior, true, CASAS_DECIMAIS_POSICAO_XYZ) &&
       EstaDentroDoIntervalo(yMenor, true, yc, yMaior, true, CASAS_DECIMAIS_POSICAO_XYZ) &&
       EstaDentroDoIntervalo(zMenor, true, zc, zMaior, true, CASAS_DECIMAIS_POSICAO_XYZ))
        return true;

    return false;
}

bool Cinematica::pontoColideComBaseFixa(double x, double y, double z)
{

    return ( (EstaDentroDoIntervalo(-5, true, x, 3, true, CASAS_DECIMAIS_POSICAO_XYZ) &&
              EstaDentroDoIntervalo(-2.5, true, y, 2.5, true, CASAS_DECIMAIS_POSICAO_XYZ) &&
              EstaDentroDoIntervalo(0.0, false, z, 13.2, true, CASAS_DECIMAIS_POSICAO_XYZ)   ) ||
             (EstaDentroDoIntervalo(-34.34, true, x, 3.36, true, CASAS_DECIMAIS_POSICAO_XYZ) &&
              EstaDentroDoIntervalo(-9.95, true, y, 9.95, true, CASAS_DECIMAIS_POSICAO_XYZ) &&
              EhMenorOuIgual(z, 0, CASAS_DECIMAIS_POSICAO_XYZ)) );

}

bool Cinematica::garraColideComBaseFixa(double x, double y, double z,
                                        double px, double py, double pz,
                                        QVector3D Zt)
{
    if(pontoColideComBaseFixa(x, y, z) || pontoColideComBaseFixa(px, py, pz))
        return true;

    double r13 = double(Zt[0]);
    double r23 = double(Zt[1]);
    double r33 = double(Zt[2]);

    Plano3D plano5(0.0, 0.0, 13.2, 0.0, 0.0, 1.0, -5.0, 3.0, -2.5, 2.5, 13.2, 13.2);
    Plano3D plano6(0.0, 0.0, 0.0, 0.0, 0.0, 1.0, -34.34, 3.36, -9.95, 9.95, -INFINITY, 0.0);

    QList<Plano3D> planos;

    planos.append(plano5);
    planos.append(plano6);

    for(Plano3D plano : planos)
    {
        double nx = plano.Nx();
        double ny = plano.Ny();
        double nz = plano.Nz();

        double x0 = plano.X0();
        double y0 = plano.Y0();
        double z0 = plano.Z0();

        double denominador = nx * r13 + ny * r23 + nz * r33;

        if(denominador != 0.0)
        {            
            double t = (nx * (x0 - px) + ny * (y0 - py) + nz * (z0 - pz))/(LgL3 * denominador);

            double xc = px + LgL3 * r13 * t;
            double yc = py + LgL3 * r23 * t;
            double zc = pz + LgL3 * r33 * t;

            if(pontoPertenceASegmentoDeReta(xc, yc, zc, px, py, pz, x, y, z) &&
               plano.contemPonto(xc, yc, zc))
            {
                return true;
            }
        }
    }

    return false;
}

bool Cinematica::garraColideComBaseGiratoria(double xgl, double zgl,
                                             double pxl, double pzl,
                                             double teta234)
{
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
        if(!(EhIgual(tetag, M_PI_2, CASAS_DECIMAIS_ROTACOES_XYZ_RAD) ||
             EhIgual(tetag, -M_PI_2, CASAS_DECIMAIS_ROTACOES_XYZ_RAD)) )
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
    if(!(EhIgual(teta2, M_PI_2, CASAS_DECIMAIS_ROTACOES_XYZ_RAD) ||
         EhIgual(teta2, -M_PI_2, CASAS_DECIMAIS_ROTACOES_XYZ_RAD)) )
    {
        double al1 = tan(teta2);

        double denominador = (zgl - pzl - al1 * (xgl - pxl));

        if(!EhIgual(denominador, 0.0, CASAS_DECIMAIS_POSICAO_XYZ))
        {
            double t = (al1 * pxl - pzl)/denominador;
            double xl = pxl + (xgl - pxl) * t;
            double zl = pzl + (zgl - pzl) * t;
            double zlJ2 = L1 * sin(teta2);
            double xlJ2 = L1 * cos(teta2);

            if(EhIgual(zl, al1 * xl, CASAS_DECIMAIS_POSICAO_XYZ) &&
               EstaDentroDoIntervalo(fmin(0.0, xlJ2), true, xl, fmax(0.0, xlJ2), true, CASAS_DECIMAIS_POSICAO_XYZ) &&
               EstaDentroDoIntervalo(fmin(0.0, zlJ2), true, zl, fmax(0.0, zlJ2), true, CASAS_DECIMAIS_POSICAO_XYZ) &&               
               EstaDentroDoIntervalo(fmin(xgl, pxl), true, xl, fmax(xgl, pxl), true, CASAS_DECIMAIS_POSICAO_XYZ) &&
               EstaDentroDoIntervalo(fmin(zgl, pzl), true, zl, fmax(zgl, pzl), true, CASAS_DECIMAIS_POSICAO_XYZ))
                return true;
        }
    }
    else
    {
        //double denominador = L1 - xgl + pxl - zgl + pzl;
        double denominador = (xgl - pxl);

        if(!EhIgual(denominador, 0.0, CASAS_DECIMAIS_POSICAO_XYZ))
        {
            //double t = (pxl + pzl)/denominador;
            double t = -pxl / denominador;

            double xl = pxl + (xgl - pxl) * t;
            double zl = pzl + (zgl - pzl) * t;

            double xlJ2 = 0.0;
            double zlJ2 = L1;

            if(EhIgual(xl, xlJ2, CASAS_DECIMAIS_POSICAO_XYZ) &&
               EstaDentroDoIntervalo(0.0, true, zl, zlJ2, true, CASAS_DECIMAIS_POSICAO_XYZ) &&
               EstaDentroDoIntervalo(fmin(xgl, pxl), true, xl, fmax(xgl, pxl), true, CASAS_DECIMAIS_POSICAO_XYZ) &&
               EstaDentroDoIntervalo(fmin(zgl, pzl), true, zl, fmax(zgl, pzl), true, CASAS_DECIMAIS_POSICAO_XYZ))
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
    QMatrix4x4 matrizGarraParaPulso(1, 0, 0, 0,
                                    0, 1, 0, 0,
                                    0, 0, 1, float(LgL3),
                                    0, 0, 0, 1);

    QMatrix4x4 matrizPulsoParaBaseFixa = cinematicaDireta(teta1graus, teta2graus, teta3graus, teta4graus, teta5graus);

    QMatrix4x4 matrizGarraParaBaseFixa = matrizPulsoParaBaseFixa * matrizGarraParaPulso;

    if(colideComBaseFixa != nullptr)
    {
        double x = double(matrizGarraParaBaseFixa(0,3));
        double y = double(matrizGarraParaBaseFixa(1,3));
        double z = double(matrizGarraParaBaseFixa(2,3));
        double px = double(matrizPulsoParaBaseFixa(0,3));
        double py = double(matrizPulsoParaBaseFixa(1,3));
        double pz = double(matrizPulsoParaBaseFixa(2,3));
        float r13 = matrizPulsoParaBaseFixa(0,2);
        float r23 = matrizPulsoParaBaseFixa(1,2);
        float r33 = matrizPulsoParaBaseFixa(2,2);

        QVector3D Zt(r13, r23, r33);

        *colideComBaseFixa = garraColideComBaseFixa(x, y, z, px, py, pz, Zt);
    }

    if(colideComBaseGir != nullptr || colideComSegmentoL1 != nullptr)
    {
        double xgl, zgl, pxl, pzl;

        double x = double(matrizGarraParaBaseFixa(0,3));
        double y = double(matrizGarraParaBaseFixa(1,3));
        double z = double(matrizGarraParaBaseFixa(2,3));
        double px = double(matrizPulsoParaBaseFixa(0,3));
        double py = double(matrizPulsoParaBaseFixa(1,3));
        double pz = double(matrizPulsoParaBaseFixa(2,3));

        coordenadasNoPlanoDoBracoRobo(x, y, z,
                                      px, py, pz,
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

    /*
    cinDir = ! r11 r12 r13   px !
             ! r21 r22 r23   py !
             ! r31 r32 r33   pz !
             !   0   0   0   1  !

    cinDir * mGP = ! r11 r12 r13   r13 * LgL3 + px !
                   ! r21 r22 r23   r23 * LgL3 + py !
                   ! r31 r32 r33   r33 * LgL3 + pz !
                   !   0   0   0          1        !

                   x = px + LgL3 * r13
                   y = py + LgL3 * r23
                   z = pz + LgL3 * r33

                   px = x - LgL3 * r13
                   py = y - LgL3 * r23
                   pz = z - LgL3 * r33
    */
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
                                     double teta234,
                                     double pxl, double pzl,
                                     SolucaoCinematicaInversa* solucao)
{
    double teta2, teta3, teta4;

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

    if(std::abs(c3) > 1.0 && std::abs(c3) < 1.5)
        c3 = trunc(c3);

    /* WARNING: Enquanto o ângulo máximo da junta 2 for zero, teta3 sempre será negativo
                Rever este trecho caso o ângulo máximo da junta 2 passe a ser positivo */
    double s3 = -sqrt(1 - pow(c3, 2));

    teta3 = atan2(s3, c3);

    avaliaAnguloTeta(&teta3, teta3min, teta3max, solucao, solucaoTeta3, solucaoTeta3Possivel);

    // Cálculo do teta2
    double beta2 = atan2(pzl, pxl);

    double cksi = (pxl2pzl2 + pow(a2, 2) - pow(a3, 2))/(2 * a2 * sqrt(pxl2pzl2));

    double sksi = sqrt(1 - pow(cksi, 2));

    if(std::abs(cksi) > 1.0 && std::abs(cksi) < 1.5)
        cksi = trunc(cksi);

    double ksi = atan2(sksi, cksi);

    teta2 = beta2 + ksi;

    avaliaAnguloTeta(&teta2, teta2min, teta2max, solucao, solucaoTeta2, solucaoTeta2Possivel);

    // Cálculo do teta4
    teta4 = teta234 - teta2 - teta3;

    avaliaAnguloTeta(&teta4, teta4min, teta4max, solucao, solucaoTeta4, solucaoTeta4Possivel);

    *teta2ptr = teta2;
    *teta3ptr = teta3;
    *teta4ptr = teta4;
}

void Cinematica::calculaTeta2Teta3Teta4Singular(double* teta2, double* teta3, double* teta4,
                                                double* pz,
                                                double *angulosMaxGraus, double *angulosMinGraus,
                                                bool *posicaoAtingivel)
{
    if(EhIgual(*pz, d1+a2+a3, CASAS_DECIMAIS_POSICAO_XYZ))
    {
        *teta2 = 90.0;
        *teta3 = 0.0;
        *teta4 = 90.0;
    }
    else if(*pz < d1+a2+a3)
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

        abordagemGeometrica(teta2, teta2min, teta2max,
                            teta3, teta3min, teta3max,
                            teta4, teta4min, teta4max,
                            teta234,
                            0, *pz - d1, &solucao);

        *teta2 = *teta2 * 180.0 / M_PI;
        *teta3 = *teta3 * 180.0 / M_PI;
        *teta4 = *teta4 * 180.0 / M_PI;

        if(posicaoAtingivel != nullptr)
            *posicaoAtingivel = solucao.possivel;
    }
    else // if(pz > d1+a2+a3)
    {
        if(posicaoAtingivel != nullptr)
            *posicaoAtingivel = false;

         *pz = d1+a2+a3;
         *teta2 = 90.0;
         *teta3 = 0.0;
         *teta4 = 90.0;
    }
}

double* Cinematica::calculaPosicaoSingular(double* pz,
                                           double gama, double beta, double alfa,
                                           double *angulosCorrentesJuntas,
                                           double *angulosMaxGraus,
                                           double *angulosMinGraus,
                                           bool *posicaoAtingivel)
{
    double teta1, teta2, teta3, teta4, teta5;

    double teta1Corrente = angulosCorrentesJuntas[0];
    double teta5Corrente = angulosCorrentesJuntas[4];

    if(EhIgual(gama, 0.0, CASAS_DECIMAIS_ROTACOES_XYZ_RAD) &&
       EhIgual(beta, 0.0, CASAS_DECIMAIS_ROTACOES_XYZ_RAD))
    {        
        if(EhIgual(alfa, 0.0, CASAS_DECIMAIS_ROTACOES_XYZ_RAD))
        {
            /*
             * teta1 = teta5 = 90.0
             *         ou
             * teta1 = teta5 = -90.0
            */
            if(teta1Corrente < 0.0)
            {
                teta1 = -90.0;
                teta5 = -90.0;
            }
            else
            {
                teta1 = 90.0;
                teta5 = 90.0;
            }

            calculaTeta2Teta3Teta4Singular(&teta2, &teta3, &teta4,
                                           pz,
                                           angulosMaxGraus,
                                           angulosMinGraus,
                                           posicaoAtingivel);

            return new double[5]{teta1, teta2, teta3, teta4, teta5};
        }        
        else if(EhIgual(std::abs(alfa), M_PI, CASAS_DECIMAIS_ROTACOES_XYZ_RAD))
        {
            /*
             * teta1 = -teta5
             * sendo que -90.0 <= teta1 <= 90.0
             */
            teta5 = teta5Corrente;
            teta1 = -teta5;

            calculaTeta2Teta3Teta4Singular(&teta2, &teta3, &teta4,
                                           pz,
                                           angulosMaxGraus,
                                           angulosMinGraus,
                                           posicaoAtingivel);

            return new double[5]{teta1, teta2, teta3, teta4, teta5};
        }
    }

    return nullptr;
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

    double gama = *gamaGraus * M_PI /180;
    double beta = *betaGraus * M_PI /180;
    double alfa = *alfaGraus * M_PI /180;

    double sgama = arredondaPara(sin(gama), CASAS_DECIMAIS_SENOS_COSSENOS);
    double sbeta = arredondaPara(sin(beta), CASAS_DECIMAIS_SENOS_COSSENOS);
    double salfa = arredondaPara(sin(alfa), CASAS_DECIMAIS_SENOS_COSSENOS);

    double cgama = arredondaPara(cos(gama), CASAS_DECIMAIS_SENOS_COSSENOS);
    double cbeta = arredondaPara(cos(beta), CASAS_DECIMAIS_SENOS_COSSENOS);
    double calfa = arredondaPara(cos(alfa), CASAS_DECIMAIS_SENOS_COSSENOS);

    double r11;
    double r21;
    double r31;
    double r12 = calfa * sbeta * sgama - salfa * cgama;
    double r22 = salfa * sbeta * sgama + calfa * cgama;
    double r32 = cbeta * sgama;
    double r13 = calfa * sbeta * cgama + salfa * sgama;
    double r23 = salfa * sbeta * cgama - calfa * sgama;
    double r33 = cbeta * cgama;

    double px = *x - LgL3 * r13;
    double py = *y - LgL3 * r23;
    double pz = *z - LgL3 * r33;

    px = arredondaPara(px, CASAS_DECIMAIS_POSICAO_XYZ);
    py = arredondaPara(py, CASAS_DECIMAIS_POSICAO_XYZ);
    pz = arredondaPara(pz, CASAS_DECIMAIS_POSICAO_XYZ);

    if(EhIgual(px, 0.0, CASAS_DECIMAIS_POSICAO_XYZ) &&
       EhIgual(py, 0.0, CASAS_DECIMAIS_POSICAO_XYZ))
    {
        double* angulosJuntas = calculaPosicaoSingular(&pz,
                                                       gama, beta, alfa,
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

    double px2py2 = pow(px, 2.0) + pow(py, 2.0);
    double sqrtpx2py2 = sqrt(px2py2);

    // Projetando o ponto desejado no plano do braço. Esta projeção será a que a
    // garra poderá verdadeiramente assumir (ver cap. 4 do craig - The Yasukawa
    // Motoman L-3 página 121)

    // WARNING: Ponto da garra e origem do referencial da base: Notar que o ponto da garra (e o ponto do pulso) não está rente com a origem do referencial da base fixa.
    // WARNING: Vetor M e plano do braço robô: O plano cuja normal é M passa pelo pulso da garra e pela origem do referencial da base (não pega o ponto da garra).
    //          Ou seja, esse mesmo plano é oblíquo ao plano que pega a coordenada da garra e do pulso
    //          e também, a um plano paralelo que pega a origem da base fixa.
    // WARNING: Ver warnings do arquivo constantes.h referentes aos parâmetros d2, d3, d4 e d5
    QVector3D M;
    if(sqrtpx2py2 != 0.0)
    {
        M.setX(float(-py/sqrtpx2py2));
        M.setY(float(px/sqrtpx2py2));
        M.setZ(0.0f);
    }
    else
    {
        // Aqui o x ou o y chegam diferentes de zero, já que o caso em que
        // px, py, x e y são zero já é tratado no método calculaPosicaoSingular
        double sqrtx2y2 = sqrt(pow(*x, 2.0) + pow(*y, 2.0));
        M.setX(float(-(*y/sqrtx2y2)));
        M.setY(float(*x/sqrtx2y2));
        M.setZ(0.0f);
    }
    QVector3D Zt(static_cast<float>(r13), static_cast<float>(r23), static_cast<float>(r33));
    QVector3D Yt(static_cast<float>(r12), static_cast<float>(r22), static_cast<float>(r32));

    QVector3D K = QVector3D::crossProduct(M, Zt);

    QVector3D Ztl = QVector3D::crossProduct(K, M);

    /* WARNING: Vetor Ztl:  Se forem respeitados os parâmetros exatos de Denavit-Hatenberg obtidos para o braço robô (mais
     * especificamente d2, d3 e d4), Ztl sempre apontará para uma direção diferente do Zt, mesmo que a posição
     * da garra seja uma posição possível, por conta do vetor M calculado (ver warning referente ao vetor M e ao plano ao
     * qual ele é perpendicular).
     */
    float cteta = QVector3D::dotProduct(Zt, Ztl);
    float steta = QVector3D::dotProduct(QVector3D::crossProduct(Zt, Ztl), K);

    steta = arredondaPara(steta, CASAS_DECIMAIS_SENOS_COSSENOS);
    cteta = arredondaPara(cteta, CASAS_DECIMAIS_SENOS_COSSENOS);

    QVector3D Ytl = cteta * Yt
                 + steta * QVector3D::crossProduct(K, Yt)
                 + ((1 - cteta) * QVector3D::dotProduct(K, Yt)) * K;

    QVector3D Xtl = QVector3D::crossProduct(Ytl, Ztl);

    for(int i = 0; i < 3; i++)
    {
        // TODO: Cinemática inversa: Verificar se estes arredondamentos fazem sentido
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

    // Recalculando x, y e z da garra
    double xProj = px + LgL3 * r13;
    double yProj = py + LgL3 * r23;
    double zProj = pz + LgL3 * r33;

    //recalculando gama, beta e alfa
    double betaProj = atan2(-r31, sqrt(pow(r11,2)+pow(r21,2)));
    double alfaProj, gamaProj;

    if(EhIgual(betaProj, M_PI_2, CASAS_DECIMAIS_ROTACOES_XYZ_RAD))
    {
        alfaProj = 0;
        gamaProj = atan2(r12, r22);
    }    
    else if (EhIgual(betaProj, -M_PI_2, CASAS_DECIMAIS_ROTACOES_XYZ_RAD))
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

    xProj = arredondaPara(xProj, CASAS_DECIMAIS_POSICAO_XYZ);
    yProj = arredondaPara(yProj, CASAS_DECIMAIS_POSICAO_XYZ);
    zProj = arredondaPara(zProj, CASAS_DECIMAIS_POSICAO_XYZ);

    double alfaGrausProj = alfaProj * 180.0 / M_PI;
    double betaGrausProj = betaProj * 180.0 / M_PI;
    double gamaGrausProj = gamaProj * 180.0 / M_PI;

    alfaGrausProj = arredondaPara(alfaGrausProj, CASAS_DECIMAIS_ROTACOES_XYZ);
    betaGrausProj = arredondaPara(betaGrausProj, CASAS_DECIMAIS_ROTACOES_XYZ);
    gamaGrausProj = arredondaPara(gamaGrausProj, CASAS_DECIMAIS_ROTACOES_XYZ);

    if(posicaoProjetada != nullptr)
    {        
        *posicaoProjetada = !(EhIgual(xProj, *x, CASAS_DECIMAIS_POSICAO_XYZ) &&
                              EhIgual(yProj, *y, CASAS_DECIMAIS_POSICAO_XYZ) &&
                              EhIgual(zProj, *z, CASAS_DECIMAIS_POSICAO_XYZ) &&
                              EhIgual(alfaGrausProj, *alfaGraus, CASAS_DECIMAIS_ROTACOES_XYZ) &&
                              EhIgual(betaGrausProj, *betaGraus, CASAS_DECIMAIS_ROTACOES_XYZ) &&
                              EhIgual(gamaGrausProj, *gamaGraus, CASAS_DECIMAIS_ROTACOES_XYZ));
    }

    *x = xProj;
    *y = yProj;
    *z = zProj;
    *gamaGraus = gamaGrausProj;
    *betaGraus = betaGrausProj;
    *alfaGraus = alfaGrausProj;

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
    double teta1_4 = atan2(r23, r13);

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
        if(!solucaoTeta1.contains(teta1_3))
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

        solucao->colideComBaseGir = garraColideComBaseGiratoria(xgl, zgl, pxl, pzl, teta234);

        abordagemGeometrica(&teta2, teta2min, teta2max,
                            &teta3, teta3min, teta3max,
                            &teta4, teta4min, teta4max,
                            teta234,
                            pxl, pzl,
                            solucao);

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

        if(solucao->subSolucao != nullptr)
        {
            // Unificação da solução com a subsolução
            SubSolucaoCinematicaInversa *subSolucao = solucao->subSolucao;

            if(!(solucao->possivel
                 || (subSolucao->teta2possivel
                     && subSolucao->teta3possivel
                     && subSolucao->teta4possivel))
               )
            {
                // Substitui os ângulos da solução pelos que constam na subsolução
                // quando a solução em si não é possível. Esta é uma forma de
                // fazer a solução ficar mais próxima do possível
                solucao->teta2 = subSolucao->teta2;
                solucao->teta3 = subSolucao->teta3;
                solucao->teta4 = subSolucao->teta4;
            }
            else
            {
                // Pega a subsolução de maior peso.
                double difTeta2 = std::abs(angulosCorrentesJuntas[1] - subSolucao->teta2);
                double difTeta3 = std::abs(angulosCorrentesJuntas[2] - subSolucao->teta3);
                double difTeta4 = std::abs(angulosCorrentesJuntas[3] - subSolucao->teta4);

                subSolucao->peso = (pesoTeta2 * difTeta2 + pesoTeta3 * difTeta3 + pesoTeta4 * difTeta4)/(pesoTeta2 + pesoTeta3 + pesoTeta4);

                difTeta2 = std::abs(angulosCorrentesJuntas[1] - solucao->teta2);
                difTeta3 = std::abs(angulosCorrentesJuntas[2] - solucao->teta3);
                difTeta4 = std::abs(angulosCorrentesJuntas[3] - solucao->teta4);

                double pesoSolucao = (pesoTeta2 * difTeta2 + pesoTeta3 * difTeta3 + pesoTeta4 * difTeta4)/(pesoTeta2 + pesoTeta3 + pesoTeta4);

                if(subSolucao->peso >= pesoSolucao)
                {
                    solucao->teta2 = subSolucao->teta2;
                    solucao->teta3 = subSolucao->teta3;
                    solucao->teta4 = subSolucao->teta4;
                }
            }

            delete subSolucao;
        }
    }
    else
    {
        // Ver no livro do Craig páginas 104 e 105 sobre pesos das juntas e solução de menor peso.
        QList<SolucaoCinematicaInversa *> solucoesValidas;

        for(int i = 0; i < solucoes.count(); i++)
        {
            solucao = solucoes.at(i);

            if(solucao->subSolucao != nullptr)
            {
                // Unifica a subsolução com base nos pesos
                SubSolucaoCinematicaInversa *subSolucao = solucao->subSolucao;

                double difTeta2 = std::abs(angulosCorrentesJuntas[1] - subSolucao->teta2);
                double difTeta3 = std::abs(angulosCorrentesJuntas[2] - subSolucao->teta3);
                double difTeta4 = std::abs(angulosCorrentesJuntas[3] - subSolucao->teta4);

                subSolucao->peso = (pesoTeta2 * difTeta2 + pesoTeta3 * difTeta3 + pesoTeta4 * difTeta4)/(pesoTeta2 + pesoTeta3 + pesoTeta4);

                difTeta2 = std::abs(angulosCorrentesJuntas[1] - solucao->teta2);
                difTeta3 = std::abs(angulosCorrentesJuntas[2] - solucao->teta3);
                difTeta4 = std::abs(angulosCorrentesJuntas[3] - solucao->teta4);

                double pesoSolucao = (pesoTeta2 * difTeta2 + pesoTeta3 * difTeta3 + pesoTeta4 * difTeta4)/(pesoTeta2 + pesoTeta3 + pesoTeta4);

                if(subSolucao->peso >= pesoSolucao)
                {
                    solucao->teta2 = subSolucao->teta2;
                    solucao->teta3 = subSolucao->teta3;
                    solucao->teta4 = subSolucao->teta4;
                }

                delete subSolucao;
            }

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
            // Obtém a solução válida de maior peso, ou a primeira, em caso de pesos iguais
            double difTeta[QTD_SERVOS - 1];
            double angTeta[QTD_SERVOS - 1];
            double pesosTeta[QTD_SERVOS - 1];

            pesosTeta[0] = pesoTeta1;
            pesosTeta[1] = pesoTeta2;
            pesosTeta[2] = pesoTeta3;
            pesosTeta[3] = pesoTeta4;
            pesosTeta[4] = pesoTeta5;

            for(int i = 0; i < solucoes.count(); i++)
            {
                solucao = solucoes.at(i);

                angTeta[0] = solucao->teta1;
                angTeta[1] = solucao->teta2;
                angTeta[2] = solucao->teta3;
                angTeta[3] = solucao->teta4;
                angTeta[4] = solucao->teta5;

                for(int j = 0; j < QTD_SERVOS - 1; j++)
                    difTeta[j] = std::abs(angulosCorrentesJuntas[j] - angTeta[j]);

                double somatorio = 0, somaPesos = 0;

                for(int j = 0; j < QTD_SERVOS - 1; j++)
                {
                    somatorio += pesosTeta[j] * difTeta[j];
                    somaPesos = pesosTeta[j];
                }

                solucao->peso = somatorio/somaPesos;
            }

            int idxSolucaoPesoMaior = 0;
            double maiorPeso = solucoes.at(0)->peso;

            for(int i = 1; i < solucoes.count(); i++)
            {
                if(solucoes.at(i)->peso > maiorPeso)
                {
                    maiorPeso = solucoes.at(i)->peso;
                    idxSolucaoPesoMaior = i;
                }
            }

            solucao = solucoes.at(idxSolucaoPesoMaior);
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

