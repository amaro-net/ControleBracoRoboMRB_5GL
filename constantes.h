#ifndef CONSTANTES_H
#define CONSTANTES_H

#include <qstring.h>

#define QTD_SERVOS 6

#define CASAS_DECIMAIS_POSICAO_ANGULAR     3
#define CASAS_DECIMAIS_VELOCIDADE_ANGULAR  3
#define CASAS_DECIMAIS_ACELERACAO_ANGULAR  4

#define DIV_CD_POSICAO_ANGULAR             1000
#define DIV_CD_VELOCIDADE_ANGULAR          1000
#define DIV_CD_ACELERACAO_ANGULAR          10000

#define CASAS_DECIMAIS_POSICAO_XYZ    2
#define CASAS_DECIMAIS_ROTACOES_XYZ   2

#define CASAS_DECIMAIS_SENOS_COSSENOS 3

#define DIV_CD_POSICAO_XYZ   100
#define DIV_CD_ROTACOES_XYZ  100

#define DIV_CD_SENOS_COSSENOS 1000

#define STATUS_SEQCOM_PARADA_VAZIA      "Sequência parada"
#define STATUS_SEQCOM_PARADA_NAO_VAZIA  "Sequência parada. ENTER ou duplo clique para executar um comando."
#define STATUS_SEQCOM_EM_EXECUCAO       "Em execução"
#define STATUS_SEQCOM_CONTINUANDO_EXEC  "Continuando execução"
#define STATUS_SEQCOM_EM_LOOP           "Em loop"
#define STATUS_SEQCOM_CONTINUANDO_LOOP  "Continuando loop"
#define STATUS_SEQCOM_EM_DLY_SEM_PAR    "Em espera. Pressione ENTER, duplo clique na sequência, Continuar ou Continuar loop."

#define TEMPO_TIMER_ENVIO_IMEDIATO_MS   256

#define LBL_POS_ATUAL_X_MIN 50
#define LBL_POS_ATUAL_X_MAX 218

#define STR_UND_MICROSSEGUNDOS " μs"
#define STR_UND_GRAUS " º"
#define STR_UND_CM " cm"

static QString junta[QTD_SERVOS] = {"J0", "J1", "J2", "J3", "J4", "GR"};
static QString idJST[QTD_SERVOS] = {"A", "B", "C", "D", "E", "G"};

static const int tabelaPosLimitesDefault[QTD_SERVOS][4] = {{2432, 480, 1405,          480},
                                                           {2256, 768, 1798,         2256},
                                                           {2208, 800, 2208,          959},
                                                           {2480, 528, 1251,         1836},
                                                           {2432, 512, 1472,         1472},
                                                           {2000, 416, (2000+416)/2, 2000}};

// Estes ângulos condizem com a notação de Denavit-Hatenberg para o nosso braço-robô
static const double tabelaPosLimitesGrausDefault[QTD_SERVOS][5] = {{100,  -90, 0, 0, 0},
                                                                   {130,    0, 0, 0, 0},
                                                                   {  0, -133, 0, 0, 0},
                                                                   {164,  -36, 0, 0, 1},
                                                                   { 90,  -90, 0, 0, 0},
                                                                   {180,    0, 0, 0, 0}};

static const int velTmpPulsoDefault[QTD_SERVOS] = {64, 32, 32, 32, 128, 100};
static const int aclTmpPulsoDefault[QTD_SERVOS] = {8, 4, 8, 8, 32, 0};

static const int velocidadesMax[QTD_SERVOS] = {128, 128, 128, 92, 128, 100};
static const int aceleracoesMax[QTD_SERVOS] = {128, 128, 128, 92, 128, 100};


/****** PARÂMETROS DE DENAVIT-HATENBERG (PARÂMETROS DH) ******/

// ângulos alfa(i-1) em graus
const double alfa0 = 0.0;
const double alfa1 = 90.0;
const double alfa2 = 0.0;
const double alfa3 = 0.0;
const double alfa4 = 90.0;

// distâncias a(i-1) em centímetros
const double a0 = 0.0;
const double a1 = 0.025;
const double a2 = 11.65;
const double a3 = 5.825;
const double a4 = 0.45;

// distâncias d(i) em centímetros
const double d1 = 17.547644;
const double d2 = 4.293516;
const double d3 = -3.438032;
const double d4 = -2.174584;
const double d5 = 8.633297;


#endif // CONSTANTES_H
