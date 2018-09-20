#include "cinematica.h"
#include "utils.h"

Cinematica::Cinematica()
{

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
    // TODO: Cinemática direta: Alterar, mesmo que provisoriamente, a cinemática direta para considerar o teta3 sempre positivo.
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
QMatrix4x4 Cinematica::matrizPosGarra(double teta1graus, double teta2graus, double teta3graus, double teta4graus, double teta5graus)
{
    QMatrix4x4 matrizGarraParaPulso(1, 0, 0, 0,
                                    0, 1, 0, 0,
                                    0, 0, 1, float(LgL3),
                                    0, 0, 0, 1);

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
    return cinematicaDireta(teta1graus, teta2graus, teta3graus, teta4graus, teta5graus)
           * matrizGarraParaPulso;
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

    if(qFuzzyCompare(beta, 90.0))
    {
        alfa = 0;
        gama = atan2(r12, r22) * 180 / M_PI;
    }
    else if(qFuzzyCompare(beta, -90.0))
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
double *Cinematica::posicaoGarra(double teta1graus, double teta2graus, double teta3graus, double teta4graus, double teta5graus)
{
    QMatrix4x4 MGarra = matrizPosGarra(teta1graus, teta2graus, teta3graus, teta4graus, teta5graus);

    return coordenadasAngFixosOuEulerZXY(MGarra);
}


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
                              bool *posicaoProjetada, bool *posicaoAtingivel)
{
    if(posicaoAtingivel != nullptr)
        *posicaoAtingivel = true;

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

    double px2py2 = pow(px,2) + pow(py,2);
    double sqrtpx2py2 = sqrt(px2py2);

    // Projetando o ponto desejado no plano do braço. Esta projeção será a que a
    // garra poderá realmente assumir (ver cap. 4 do craig - The Yasukawa
    // Motoman L-3 página 121)

    // WARNING: Ponto da garra e origem do referencial da base: Notar que o ponto da garra (e o ponto do pulso) não está rente com a origem do referencial da base fixa.
    // WARNING: Vetor M e plano do braço robô: O plano cuja normal é M passa pelo pulso da garra e pela origem do referencial da base (não pega o ponto da garra).
    //          Ou seja, esse mesmo plano é oblíquo ao plano que pega a coordenada da garra e do pulso
    //          e também, a um plano paralelo que pega a origem da base fixa.
    // WARNING: Ver warnings do arquivo constantes.h referentes aos parâmetros d2, d3, d4 e d5

    // TODO: Cinemática inversa: Tratar o caso em que o braço robô está totalmente na vertical
    // TODO: Cinemática inversa: Tratar outros casos em que px = 0 e py = 0
    QVector3D M = QVector3D(float(-py/sqrtpx2py2), float(px/sqrtpx2py2), 0);
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

    if(qFuzzyCompare(betaProj, M_PI_2))
    {
        alfaProj = 0;
        gamaProj = atan2(r12, r22);
    }
    else if (qFuzzyCompare(betaProj, -M_PI_2))
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

    double alfaGrausProj = alfaProj * 180 / M_PI;
    double betaGrausProj = betaProj * 180 / M_PI;
    double gamaGrausProj = gamaProj * 180 / M_PI;

    alfaGrausProj = arredondaPara(alfaGrausProj, CASAS_DECIMAIS_ROTACOES_XYZ);
    betaGrausProj = arredondaPara(betaGrausProj, CASAS_DECIMAIS_ROTACOES_XYZ);
    gamaGrausProj = arredondaPara(gamaGrausProj, CASAS_DECIMAIS_ROTACOES_XYZ);

    if(posicaoProjetada != nullptr)
    {
        *posicaoProjetada = !(qFuzzyCompare(xProj, *x) &&
                              qFuzzyCompare(yProj, *y) &&
                              qFuzzyCompare(zProj, *z) &&
                              qFuzzyCompare(alfaGrausProj, *alfaGraus) &&
                              qFuzzyCompare(betaGrausProj, *betaGraus) &&
                              qFuzzyCompare(gamaGrausProj, *gamaGraus));
    }

    *x = xProj;
    *y = yProj;
    *z = zProj;
    *gamaGraus = gamaGrausProj;
    *betaGraus = betaGrausProj;
    *alfaGraus = alfaGrausProj;

    // Início da cinemática inversa propriamente dita
    SolucaoCinematicaInversa *solucao;
    QList<SolucaoCinematicaInversa *> solucoes;
    QList<double> solucaoTeta1;

    const double d234 = d2 + d3 + d4;
    const double d234quad = d234 * d234;

    double atan2pypx = atan2(-px, py);
    double atan2sqrt = atan2(sqrt(px2py2 - d234quad),-d234);

    double teta1_1 = atan2pypx + atan2sqrt;
    double teta1_2 = atan2pypx - atan2sqrt;
    double teta1_3 = atan2(r23, r13);
    double teta1_4 = atan2(r23, r13);

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


    double teta1, teta2, teta3, teta4, teta5;

    if((teta1_1 >= teta1min) && (teta1_1 <= teta1max))
        solucaoTeta1.append(teta1_1);

    if((teta1_2 >= teta1min) && (teta1_2 <= teta1max))
    {
        if(!solucaoTeta1.contains(teta1_2))
            solucaoTeta1.append(teta1_2);
    }

    if((teta1_3 >= teta1min) && (teta1_3 <= teta1max))
    {
        if(!solucaoTeta1.contains(teta1_3))
            solucaoTeta1.append(teta1_3);
    }

    if((teta1_4 >= teta1min) && (teta1_4 <= teta1max))
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
        else if (teta1_1 > teta1max || teta1_2 > teta1max || teta1_3 > teta1max || teta1_4 > teta1max)
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

    double pzd1 = pz - d1;

    for(int i = 0; i < solucoes.count(); i++)
    {
        solucao = solucoes.at(i);

        teta1 = solucoes.at(i)->teta1;
        double s1 = solucoes.at(i)->s1;
        double c1 = solucoes.at(i)->c1;

        double s5 = s1 * r11 - c1 * r21;
        double c5 = s1 * r12 - c1 * r22;

        teta5 = atan2(s5, c5);

        if(teta5 >= teta5min && teta5 <= teta5max)
        {
            solucao->teta5 = teta5;
            solucao->s5 = s5;
            solucao->c5 = c5;
            solucao->teta5possivel = true;
        }
        else
        {
            if(teta5 < teta5min)
                teta5 = teta5min;
            else if(teta5 > teta5max)
                teta5 = teta5max;

            s5 = arredondaPara(sin(teta5), CASAS_DECIMAIS_SENOS_COSSENOS);
            c5 = arredondaPara(cos(teta5), CASAS_DECIMAIS_SENOS_COSSENOS);

            solucao->teta5 = teta5;
            solucao->s5 = s5;
            solucao->c5 = c5;
            solucao->possivel = false;
            solucao->teta5possivel = false;
        }

        double c234, s234, teta234;

        s234 = c1 * r13 + s1 * r23;
        c234 = -r33;

        teta234 = atan2(s234, c234);

        double sinalpx2py2;

        if( (teta1 < 0 && (py > 0)) ||
            (teta1 > 0 && (py < 0)) ||
            (qFuzzyCompare(teta1, 0.0) && px < 0))
            sinalpx2py2 = -1;
        else
            sinalpx2py2 = 1;

        double px2py2pzdl2 = px2py2 + pow(pzd1, 2);

        double c3 = (px2py2pzdl2 - pow(a2, 2) - pow(a3, 2))/(2 * a2 * a3);

        if(std::abs(c3) > 1.0 && std::abs(c3) < 1.5)
            c3 = 1.0;

        double s3 = -sqrt(1 - pow(c3, 2)); // teta3 sempre será negativo

        teta3 = atan2(s3, c3);

        if(teta3 >= teta3min && teta3 <= teta3max)
        {
            solucao->teta3 = teta3;
            solucao->s3 = s3;
            solucao->c3 = c3;
            solucao->teta3possivel = true;
        }
        else
        {
            if(teta3 < teta3min)
                teta3 = teta3min;
            else if(teta3 > teta3max)
                teta3 = teta3max;

            s3 = arredondaPara(sin(teta3), CASAS_DECIMAIS_SENOS_COSSENOS);
            c3 = arredondaPara(cos(teta3), CASAS_DECIMAIS_SENOS_COSSENOS);

            solucao->teta3 = teta3;
            solucao->s3 = s3;
            solucao->c3 = c3;
            solucao->possivel = false;
            solucao->teta3possivel = false;
        }

        double beta2 = atan2(pzd1, sinalpx2py2 * sqrt(px2py2));

        double cksi = (px2py2pzdl2 + pow(a2, 2) - pow(a3, 2))/(2 * a2 * sqrt(px2py2pzdl2));

        if(cksi > 1.0 && cksi < 1.5)
            cksi = 1.0;

        double ksi = atan2(sqrt(1 - pow(cksi, 2)), cksi);

        teta2 = beta2 + ksi;        

        teta4 = teta234 - teta2 - teta3;

        // TODO: Cinemática inversa: Remover estas variáveis
        double beta2graus = beta2 * 180 / M_PI;
        double ksigraus = ksi * 180 / M_PI;

        double teta234graus = teta234 * 180 / M_PI;
        double teta2graus = teta2 * 180 / M_PI;
        double teta3graus = teta3 * 180 / M_PI;
        double teta4graus = teta4 * 180 / M_PI;

        if(teta2 >= teta2min && teta2 <= teta2max)
        {
            solucao->teta2 = teta2;
            solucao->teta2possivel = true;
        }
        else
        {
            if(teta2 < teta2min)
                teta2 = teta2min;
            else if(teta2 > teta2max)
                teta2 = teta2max;

            solucao->teta2 = teta2;
            solucao->possivel = false;
            solucao->teta3possivel = false;
        }

        if(teta4 >= teta4min && teta4 <= teta4max)
        {
            solucao->teta4 = teta4;
            solucao->teta4possivel = true;
        }
        else
        {
            if(teta4 < teta4min)
                teta4 = teta4min;
            else if(teta4 > teta4max)
                teta4 = teta4max;

            solucao->teta4 = teta4;
            solucao->possivel = false;
            solucao->teta4possivel = false;
        }

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

    if(posicaoAtingivel != nullptr)
    {
        *posicaoAtingivel = solucao->possivel;
    }

    teta1 = solucao->teta1;
    teta2 = solucao->teta2;
    teta3 = solucao->teta3;
    teta4 = solucao->teta4;
    teta5 = solucao->teta5;

    while(!solucoes.isEmpty())
    {
        delete solucoes.takeFirst();
    }

    teta1 = teta1 * 180 / M_PI;
    teta2 = teta2 * 180 / M_PI;
    teta3 = teta3 * 180 / M_PI;
    teta4 = teta4 * 180 / M_PI;
    teta5 = teta5 * 180 / M_PI;

    teta1 = arredondaPara(teta1, CASAS_DECIMAIS_POSICAO_ANGULAR);
    teta2 = arredondaPara(teta2, CASAS_DECIMAIS_POSICAO_ANGULAR);
    teta3 = arredondaPara(teta3, CASAS_DECIMAIS_POSICAO_ANGULAR);
    teta4 = arredondaPara(teta4, CASAS_DECIMAIS_POSICAO_ANGULAR);
    teta5 = arredondaPara(teta5, CASAS_DECIMAIS_POSICAO_ANGULAR);

    return new double[5]{teta1, teta2, teta3, teta4, teta5};
}

