#ifndef CONSTANTES_H
#define CONSTANTES_H

#include <qstring.h>

#define QTD_SERVOS 6

#define CASAS_DECIMAIS_POSICAO_ANGULAR     3
#define CASAS_DECIMAIS_VELOCIDADE_ANGULAR  3
#define CASAS_DECIMAIS_ACELERACAO_ANGULAR  4

#define CASAS_DECIMAIS_POSICAO_XYZ    2
#define CASAS_DECIMAIS_ROTACOES_XYZ   2

#define CASAS_DECIMAIS_SENOS_COSSENOS 4

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


/************ Tamanhos dos segmentos ************/
const double L1 = 11.65;
const double L2 = 5.825;
const double L3 = 8.633297;

const double Lg = 7.5; // tamanho do eixo que corta verticalmente a base da garra

const double LgL3 = Lg + L3;

/****** PARÂMETROS DE DENAVIT-HATENBERG (PARÂMETROS DH) ******/

// ângulos alfa(i-1) em graus
const double alfa0 = 0.0;
const double alfa1 = 90.0;
// TODO: Cinemática direta/inversa: testar uma cinemática provisória com o alfa2 = -180 e o alfa3 = 180, para o teta3 ser sempre positivo
const double alfa2 = 0.0;
const double alfa3 = 0.0;
const double alfa4 = 90.0;

// distâncias a(i-1) em centímetros
const double a0 = 0.0; // Desconsidera o deslocamento para frente da junta 1 (eixos Z0 e Z1 passam a se cruzar)
//const double a1 = 0.025;
const double a1 = 0.0;
const double a2 = L1;
const double a3 = L2;
//const double a4 = 0.45;
const double a4 = 0.0; // Desconsidera o deslocamento a4 entre Z4 e Z5 (eixos Z4 e Z5 passam a se cruzar)

// distâncias d(i) em centímetros
const double d1 = 17.547644;
/* WARNING: d2, d3 e d4 zerados significa que o ponto do pulso da garra (e o ponto da garra, consequentemente),
   estará alinhado com a origem do sistema de coordenadas XYZ localizado na base do
   braço robô. O efeito disto é que o ponto
   efetivamente calculado para a garra estará deslocado a uma distância de
   1,3191 cm do ponto que fica rente com o eixo de giro da garra. */
/*const double d2 = 4.293516;
const double d3 = -3.438032;
const double d4 = -2.174584;*/

const double d2 = 0;
const double d3 = 0;
const double d4 = 0;

/* WARNING: O d5 zerado significa que, para efeito de cálculo, a posição do pulso da garra
 * (junta 4) será a mesma posição da junta 3, isto levando-se em conta que a
 * soma das distâncias d2, d3 e d4 serão zero.
   */
const double d5 = 0;


#endif // CONSTANTES_H
