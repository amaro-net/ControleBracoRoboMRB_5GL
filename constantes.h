#ifndef CONSTANTES_H
#define CONSTANTES_H

#include <qstring.h>

#define QTD_SERVOS 6

#define CASAS_DECIMAIS_POSICAO     3
#define CASAS_DECIMAIS_VELOCIDADE  3
#define CASAS_DECIMAIS_ACELERACAO  4

#define DIV_CD_POSICAO             1000
#define DIV_CD_VELOCIDADE          1000
#define DIV_CD_ACELERACAO          10000

#define STATUS_SEQCOM_PARADA_VAZIA      "Sequência parada"
#define STATUS_SEQCOM_PARADA_NAO_VAZIA  "Sequência parada. ENTER ou duplo clique para executar um comando."
#define STATUS_SEQCOM_EM_EXECUCAO       "Em execução"
#define STATUS_SEQCOM_CONTINUANDO_EXEC  "Continuando execução"
#define STATUS_SEQCOM_EM_LOOP           "Em loop"
#define STATUS_SEQCOM_CONTINUANDO_LOOP  "Continuando loop"
#define STATUS_SEQCOM_EM_DLY_SEM_PAR    "Em espera. Pressione ENTER ou duplo clique na sequência."

#define TEMPO_TIMER_ENVIO_IMEDIATO_US   200

static QString junta[QTD_SERVOS] = {"J0", "J1", "J2", "J3", "J4", "GR"};
static QString idJST[QTD_SERVOS] = {"A", "B", "C", "D", "E", "G"};

static int tabelaPosLimitesDefault[QTD_SERVOS][4] = {{2432, 480, 1405,          480},
                                                     {2256, 768, 1798,         2256},
                                                     {2208, 800, 2208,          959},
                                                     {2480, 528, 1251,         1836},
                                                     {2432, 512, 1472,         1472},
                                                     {2000, 416, (2000+416)/2, 2000}};

// Estes ângulos condizem com a notação de Denavit-Hatenberg para o nosso braço-robô
static double tabelaPosLimitesGrausDefault[QTD_SERVOS][5] = {{100,  -90, 0, 0, 0},
                                                             {130,    0, 0, 0, 0},
                                                             {  0, -133, 0, 0, 0},
                                                             {164,  -36, 0, 0, 1},
                                                             { 90,  -90, 0, 0, 0},
                                                             {180,    0, 0, 0, 0}};

static int velTmpPulsoDefault[QTD_SERVOS] = {64, 32, 32, 32, 128, 100};
static int aclTmpPulsoDefault[QTD_SERVOS] = {8, 4, 8, 8, 32, 0};

static int velocidadesMax[QTD_SERVOS] = {128, 128, 128, 92, 128, 100};
static int aceleracoesMax[QTD_SERVOS] = {128, 128, 128, 92, 128, 100};

#endif // CONSTANTES_H
