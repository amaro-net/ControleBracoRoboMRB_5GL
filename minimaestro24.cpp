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
#include "minimaestro24.h"

MiniMaestro24::MiniMaestro24(QSerialPort *serial)
{
    this->serial = serial;

    timerResposta = new QElapsedTimer;
    //timerResposta->setSingleShot(true);

    timerMonitoramentoPosicao = new QTimer(this);
    timerMonitoramentoPosicao->setSingleShot(true);
    timerMonitoramentoPosicao->setInterval(MM24_PERIODO_MONITORAMENTO_US);

    for(int i = 0; i < 6; i++)
    {
        posicaoAlvoSetada[i] = false;
    }

    //connect(timerResposta, &QTimer::timeout, this, &MiniMaestro24::timeoutResposta);
    connect(timerMonitoramentoPosicao, &QTimer::timeout, this, &MiniMaestro24::timeoutMonitoramentoPosicao);
}

short MiniMaestro24::SetTarget(char canal, uint16_t target)
{
    uint16_t target4 = target * 4; // multiplica o target por 4. Exigência do Compact Protocol da placa Mini Maestro

    char data[] = {
                   static_cast<char>(0x84),                 // Comando set target, Compact Protocol
                   static_cast<char>(canal),                // Canal
                   static_cast<char>(target4 & 0x7F),       // Bits menos significativos do target
                   static_cast<char>((target4 >> 7) & 0x7F)
                  };

    posicaoAlvoPronta = false;

    for(int i = 0; i < 6; i++)
    {
        posicaoAlvoSetada[i] = false;
    }

    this->posicaoAlvo[int(canal)] = target;
    this->posicaoAlvoSetada[int(canal)] = true;

    posicaoAlvoPronta = true;

    emit setouPosicaoAlvo(int(canal), target);

    // Envio do comando
    serial->write(data, sizeof(data));

    emit enviouParaPortaSerial(data, sizeof(data));

    return 1;
}

short MiniMaestro24::SetMultipleTargets(char numTargets, char primeiroCanal, uint16_t targets[])
{
    int tamData = 3 + 2 * numTargets;
    char* data = new char[tamData];
    //char data[3 + 2 * numTargets];
    //char* data = static_cast<char*>(malloc((3+2*numTargets)*sizeof(char)));
    uint16_t target;

    posicaoAlvoPronta = false;

    for(int i = 0; i < 6; i++)
    {
        posicaoAlvoSetada[i] = false;
    }

    data[0] = static_cast<char>(0x9F); // Comando Set Multiple Target, Compact Protocol
    data[1] = numTargets;
    data[2] = primeiroCanal;

    int j = 3;

    int aposUltimoCanal = numTargets + primeiroCanal;

    for(int i = primeiroCanal; i < aposUltimoCanal; i++)
    {
        target = targets[i];
        uint16_t target4 = target * 4;
        data[j++] = static_cast<char>(target4 & 0x7F);
        data[j++] = static_cast<char>((target4 >> 7) & 0x7F);

        this->posicaoAlvo[i] = target;
        this->posicaoAlvoSetada[i] = true;

        emit setouPosicaoAlvo(i, target);
    }

    posicaoAlvoPronta = true;

    // Envio do comando
    serial->write(data, tamData);

    emit enviouParaPortaSerial(data, tamData);

    delete[] data;

    return 1;
}

short MiniMaestro24::SetSpeed(char canal, uint16_t vel)
{
    char data[] = {
                   static_cast<char>(0x87),              // Comando set speed, Compact Protocol
                   static_cast<char>(canal),             // Canal
                   static_cast<char>(vel & 0x7F),        // Bits menos significativos da velocidade
                   static_cast<char>((vel >> 7) & 0x7F)
                  };

    emit setouVelocidade(int(canal), vel);

    // Envio do comando
    serial->write(data, sizeof(data));

    emit enviouParaPortaSerial(data, sizeof(data));

    return 1;
}

short MiniMaestro24::SetAcceleration(char canal, uint16_t accel)
{
    char data[] = {
                   static_cast<char>(0x89),                // Comando set acceleration, Compact Protocol
                   static_cast<char>(canal),               // Canal
                   static_cast<char>(accel & 0x7F),        // Bits menos significativos da aceleração
                   static_cast<char>((accel >> 7) & 0x7F)
                  };

    emit setouAceleracao(int(canal), accel);

    // Envio do comando
    serial->write(data, sizeof(data));

    emit enviouParaPortaSerial(data, sizeof(data));

    return 1;
}

unsigned int MiniMaestro24::GetPosition(char canal)
{
    char data[] = {
                   static_cast<char>(0x90), // Comando get position, Compact Protocol
                   canal,                   // Canal
                  };

    comando_mm cmd;
    cmd.comando = MM24_GET_POSITION;
    cmd.canal = canal;
    cmd.qtdBytesAReceber = 2;

    this->filaComMonitoramentoEnviados.enqueue(cmd);

    // Envio do comando
    serial->write(data, 2);

    emit enviouParaPortaSerial(data, 2);

    return 0;
}

unsigned char MiniMaestro24::GetMovingState()
{

    comando_mm cmd;
    cmd.comando = MM24_GET_MOVING_STATE;
    cmd.qtdBytesAReceber = 1;

    this->filaComMonitoramentoEnviados.enqueue(cmd);

    //QByteArray comando("\x93");

    //QByteArray byte = QByteArray::fromHex(comando);

    char comando = static_cast<char>(0x93);
    serial->write(&comando, 1);

    emit enviouParaPortaSerial(&comando, 1);

    return 0;
}

unsigned int MiniMaestro24::GetErrors()
{
    comando_mm cmd;
    cmd.comando = MM24_GET_ERRORS;
    cmd.qtdBytesAReceber = 2;

    this->filaComMonitoramentoEnviados.enqueue(cmd);

    char comando = static_cast<char>(0xA1); // Comando get errors, Compact Protocol

    serial->putChar(comando);

    emit enviouParaPortaSerial(&comando, 1);

    return 0;
}

void MiniMaestro24::timeoutMonitoramentoPosicao()
{
    if(!this->filaComandosAEnviar.isEmpty())
    {
        comando_mm comando = this->filaComandosAEnviar.dequeue();
        if(comando.comando == MM24_GET_POSITION)
        {
            this->GetPosition(comando.canal);
        }
        else if(comando.comando == MM24_GET_MOVING_STATE)
        {
            this->GetMovingState();            
        }        
        else if(comando.comando == MM24_GET_ERRORS)
        {
            this->GetErrors();
        }
        else if(comando.comando == MM24_SET_TARGET)
        {
            this->SetTarget(comando.canal, comando.target[0]);
        }
        else if(comando.comando == MM24_SET_MULTIPLE_TARGETS)
        {
            this->SetMultipleTargets(comando.numTargets, comando.canal, comando.target);
        }
        else if(comando.comando == MM24_SET_SPEED)
        {
            this->SetSpeed(comando.canal, comando.target[0]);
        }
        else if(comando.comando == MM24_SET_ACCELERATION)
        {
            this->SetAcceleration(comando.canal, comando.target[0]);
        }
    }
    else
        this->GetMovingState();

    this->timerMonitoramentoPosicao->start();    
}

bool MiniMaestro24::todasPosicoesDasJuntasAtualizadas()
{
    bool resultado = true;
    for(int i = 0; i < 5 && resultado; i++)
    {
        resultado = posicaoAtualizadaCanal[i];
    }

    return resultado;
}

bool MiniMaestro24::todasPosicoesDosCanaisAtualizados()
{
    bool resultado = true;
    for(int i = 0; i < 6 && resultado; i++)
    {
        resultado = posicaoAtualizadaCanal[i];
    }

    return resultado;
}

bool MiniMaestro24::algumaPosicaoDeJuntaZerada()
{
    bool resultado = false;
    for(int i = 0; i < 5 && !resultado; i++)
    {
        resultado = (posicao[i] == 0);
    }

    return resultado;
}

bool MiniMaestro24::todasPosicoesDasJuntasZeradas()
{
    bool resultado = true;
    for(int i = 0; i < 5 && resultado; i++)
    {
        resultado = (posicao[i] == 0);
    }

    return resultado;
}

bool MiniMaestro24::posicaoAtualEhDeRepouso()
{
    bool resultado = true;
    for(int i = 0; i < 5 && resultado; i++)
    {
        resultado = (posicao[i] == posicaoRepouso[i]);
    }

    return resultado;
}

bool MiniMaestro24::posicaoAlvoAtingida()
{
    bool resultado = true;
    for(int i = 0; i < 6 && resultado; i++)
    {
        // Se posicaoAlvoSetada[i] então (posicao[i] == posicaoAlvo[i])
        // posicaoAlvoSetada[i] => (posicao[i] == posicaoAlvo[i])
        resultado = !posicaoAlvoSetada[i] || (posicao[i] == posicaoAlvo[i]);
    }
    return resultado;
}

void MiniMaestro24::decodificaResposta()
{
    comando_mm cmdrec = this->filaComRespostaRecebida.dequeue();

    if(cmdrec.comando == MM24_GET_MOVING_STATE)
    {
        char resposta = cmdrec.resposta[0];

        this->filaComandosAEnviar.clear();

        if(resposta == 0x01) // em movimento
        {
            this->flagMovimento = true;
        }
        else // fim de movimento
        {
            if(this->flagMovimento)
            {
                // Se posicaoAlvoPronta então posicaoAlvoAtingida()
                // posicaoAlvoPronta => posicaoAlvoAtingida()
                if(!this->posicaoAlvoPronta || posicaoAlvoAtingida())
                {
                    this->flagMovimento = false;

                    emit this->fimMovimento(this->posicao);

                    if(this->posicaoAlvoPronta)
                        posicaoAlvoPronta = false;
                }
            }
            else if(this->posicaoRepousoAcionada)
            {                
                if(!this->posicaoAtualEhDeRepouso() || this->desligaServosAcionado)
                {
                    // Aviso para atualizar this->posicao antes
                    this->atualizacaoPosicaoSolicitada = true;
                }
                else
                    this->posicaoRepousoAcionada = false;
            }
            else if(this->desligaServosAcionado)
            {
                if(!atualizacaoPosicaoSolicitada)
                    this->posicaoRepousoAcionada = true;
                // Aviso para atualizar this->posicao antes
                this->atualizacaoPosicaoSolicitada = true;
            }
            else if(this->inicializacaoSolicitada)
            {
                // Aviso para atualizar this->posicao antes
                this->atualizacaoPosicaoSolicitada = true;
            }

            if(!this->filaComAcionamento.isEmpty())
            {
                this->filaComandosAEnviar.enqueue(this->filaComAcionamento.dequeue());
                this->flagMovimento = !this->filaComAcionamento.isEmpty();
            }
            else
            {
                emit semMovimento(this->posicao);
            }
        }

        comando_mm comandoGetErrors;
        comandoGetErrors.comando = MM24_GET_ERRORS;
        comandoGetErrors.qtdBytesAReceber = 2;
        this->filaComandosAEnviar.enqueue(comandoGetErrors);

        for (char canal = 0; canal < 6; canal++)
        {
            comando_mm comandoGetPosition;

            comandoGetPosition.comando = MM24_GET_POSITION;
            comandoGetPosition.canal = canal;
            comandoGetPosition.qtdBytesAReceber = 2;

            this->filaComandosAEnviar.enqueue(comandoGetPosition);
        }

        /* Sempre manter o getMovingState como o último da fila, pois
           é durante a resposta dele que a fila é realimentada */
        comando_mm comandoGetMovingState;
        comandoGetMovingState.comando = MM24_GET_MOVING_STATE;
        comandoGetMovingState.qtdBytesAReceber = 1;
        this->filaComandosAEnviar.enqueue(comandoGetMovingState);
    }
    else if(cmdrec.comando == MM24_GET_POSITION)
    {
        // TODO: Mini Maestro 24: Mudar a representação do valor de posição para ponto flutuante
        unsigned char resposta0 = static_cast<unsigned char>(cmdrec.resposta[0]);
        unsigned char resposta1 = static_cast<unsigned char>(cmdrec.resposta[1]);
        unsigned int position = resposta0 + 256 * resposta1;
        position /= 4;
        int canal = int(cmdrec.canal);
        this->posicao[canal] = position;        

        this->posicaoAtualizadaCanal[canal] = true;

        if(this->todasPosicoesDosCanaisAtualizados())
        {
            uint16_t posicaoParadaTotal[6];

            for(int i = 0; i < 6; i++)
            {
                this->posicaoAtualizadaCanal[i] = false;
                posicaoParadaTotal[i] = uint16_t(this->posicao[i]);
            }

            if(paradaTotalSolicitada)
            {
                this->SetMultipleTargets(6, 0, posicaoParadaTotal);
            }

            emit this->positionChanged(this->posicao);

            if(this->atualizacaoPosicaoSolicitada && !this->flagMovimento)
            {
                if(this->posicaoRepousoAcionada)
                {
                    this->flagMovimento = true;
                    if(this->posicaoAtualEhDeRepouso() || (this->desligaServosAcionado && todasPosicoesDasJuntasZeradas()))
                        this->posicaoRepousoAcionada = false;

                    if(!this->desligaServosAcionado)
                        this->atualizacaoPosicaoSolicitada = false;
                }
                else if(this->desligaServosAcionado)
                {
                    this->flagMovimento = true;
                    if(this->todasPosicoesDasJuntasZeradas())
                    {
                        this->desligaServosAcionado = false;
                        this->atualizacaoPosicaoSolicitada = false;
                    }
                }
                else if(this->inicializacaoSolicitada)
                {
                    this->flagMovimento = true;
                    this->inicializacaoSolicitada = false;
                    this->atualizacaoPosicaoSolicitada = false;
                }
            }
        }        
    }
    else if(cmdrec.comando == MM24_GET_ERRORS)
    {        
        this->bytesDeErro[0] = static_cast<unsigned char>(cmdrec.resposta[0]);
        this->bytesDeErro[1] = static_cast<unsigned char>(cmdrec.resposta[1]);

        emit respostaGetErrors(this->bytesDeErro);
    }
}
