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
#ifndef MINIMAESTRO24_H
#define MINIMAESTRO24_H

#include <QtSerialPort/QSerialPort>
#include <QQueue>
#include <QTimer>
#include <QElapsedTimer>

#define MM24_PERIODO_MONITORAMENTO_US  10

#define MM24_SET_TARGET           "setTarget"
#define MM24_SET_MULTIPLE_TARGETS "setMultipleTargets"
#define MM24_SET_SPEED            "setSpeed"
#define MM24_SET_ACCELERATION     "setAcceleration"
#define MM24_GET_POSITION         "getPosition"
#define MM24_GET_MOVING_STATE     "getMovingState"
#define MM24_GET_ERRORS           "getErrors"

typedef struct _comando_mm{
    QString comando;
    char canal;
    char numTargets;
    uint16_t target[6];
    short int qtdBytesAReceber;
    char resposta[2];
}comando_mm;

const QString descricaoBitErro[9] = {
    "Serial Signal Error",
    "Serial Overrun Error",
    "Serial RX buffer full",
    "Serial CRC error",
    "Serial protocol error",
    "Serial timeout error",
    "Script stack error",
    "Script call stack error",
    "Script program counter error"
};

class MiniMaestro24 : public QObject
{
    Q_OBJECT

    private:
        QSerialPort *serial;        

    signals:
        void positionChanged(unsigned int posicao[]);
        void fimMovimento(unsigned int posicao[]);
        void semMovimento(unsigned int posicao[]);
        void setouPosicaoAlvo(int canal, unsigned int valor);
        void setouVelocidade(int canal, unsigned int valor);
        void setouAceleracao(int canal, unsigned int valor);
        void respostaGetErrors(unsigned char bytesErro[]);
        void enviouParaPortaSerial(const char *data, qint64 len);

    public:
        MiniMaestro24(QSerialPort *serial);
        /**
         * Envia para a placa Mini Maestro um comando para setar o tempo de pulso (target) do servo
         * @param canal Canal ao qual o servo está conectado na placa Mini Maestro
         * @param target Valor correspondente ao tempo de pulso do servo, em microssegundos
         * @return 1 se o comando foi executado corretamente, 0 caso haja erros.
         */
        short SetTarget(char canal, uint16_t target);

        /**
         * Envia para a placa Mini Maestro um comando para setar, ao mesmo tempo, os tempos de pulso (targets) de uma sequência
         * de servos, determinada pelos parâmetros numTargets e primeiroCanal. Ex.: Se a quantidade de servos for 3, e o primeiro
         * canal for 2, serão setados os targets dos canais 2, 3 e 4. Os tempos a serem setados deverão ser armazenados no
         * vetor de servos antes desta função ser chamada.
         * @param numTargets Quantidade de servos na sequência que terão suas posições modificadas (seus targets setados)
         * @param primeiroCanal Primeiro canal da sequência de servos
         * @return 1 se o comando foi executado corretamente, 0 caso contrário
         */
        short SetMultipleTargets(char numTargets, char primeiroCanal, uint16_t targets[]);

        /**
         * Envia para a placa Mini Maestro um comando para setar a velocidade de variação
         * do pulso do servo, em (0,25us)/(10ms). Valor zero (ou valores muito altos) significa
         * que o servo irá assumir sua máxima velocidade.
         * @param canal Canal ao qual o servo está conectado na placa Mini Maestro
         * @param vel Velocidade a ser setada
         * @return 1 se a velocidade foi corretamente setada, 0 caso contrário.
         */
        short SetSpeed(char canal, uint16_t vel);

        /**
         * Envia para a placa Mini Maestro um comando para setar a aceleração do pulso
         * do servo, em (o,25us)/(10ms)/(80ms). Valor zero (ou valores muito altos) significa
         * que o servo assumirá sua máxima aceleração/desaceleração ao iniciar/concluir
         * um movimento.
         * @param canal Canal ao qual o servo está conectado na placa Mini Maestro
         * @param accel Aceleração a ser setada
         * @return 1 se a aceleração foi corretamente setada, 0 caso contrário.
         */
        short SetAcceleration(char canal, uint16_t accel);

        /**
         * Obtém da placa Mini Maestro o valor corrente da posição (tempo de pulso ou target) do servo
         * @param canal Canal ao qual está conectado o servo do qual se deseja saber a posição
         * @return Valor do tempo de pulso do servo em microssegundos. Caso haja desconexão da Mini Maestro, retorna 0xFFFF.
         */
        unsigned int GetPosition(char canal);

        /**
         * Verifica na placa Mini Maestro se há algum servo em movimento.
         * @return 1 se a placa Mini Maestro responder que há servos em movimento, 0 caso contrário.
         *          Caso haja desconexão da placa Mini Maestro, retorna 0xFF.
         */
        unsigned char GetMovingState();

        /**
         * Obtém bytes que representam o tipo de erro ocorrido na placa Mini Maestro.
         * Mais detalhes sobre os valores do erro na documentação da placa Mini Maestro.
         * @return Valor correspondente ao erro. Em caso de desconexão com a Mini Maestro, retorna 0xFFFF.
         */
        unsigned int GetErrors();

        void timeoutMonitoramentoPosicao();


        /* Checagens da posição dos canais */
        bool todasPosicoesDasJuntasAtualizadas();
        bool todasPosicoesDosCanaisAtualizados();
        bool algumaPosicaoDeJuntaZerada();
        bool todasPosicoesDasJuntasZeradas();
        bool posicaoAtualEhDeRepouso();        
        bool posicaoAlvoAtingida();

        void decodificaResposta();

        QQueue<comando_mm> filaComandosAEnviar;
        QQueue<comando_mm> filaComMonitoramentoEnviados;
        QQueue<comando_mm> filaComRespostaRecebida;
        QQueue<comando_mm> filaComAcionamento;

        QElapsedTimer *timerResposta;
        QTimer *timerMonitoramentoPosicao;

        unsigned int posicao[6];
        unsigned int posicaoAlvo[6];
        bool posicaoAlvoSetada[6];
        bool posicaoAlvoPronta = false;

        bool flagMovimento = false;

        bool posicaoAtualizadaCanal[6];

        unsigned int posicaoRepouso[5];

        bool posicaoRepousoAcionada = false;
        bool desligaServosAcionado = false;
        bool inicializacaoSolicitada = false;
        bool atualizacaoPosicaoSolicitada = true;

        bool paradaTotalSolicitada = false;

        unsigned char bytesDeErro[2] = {0x00, 0x00};
};

#endif // MINIMAESTRO24_H
