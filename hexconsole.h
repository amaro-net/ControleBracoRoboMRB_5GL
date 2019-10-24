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

#ifndef HEXCONSOLE_H
#define HEXCONSOLE_H

#include <QPlainTextEdit>

class HexConsole : public QPlainTextEdit
{
        Q_OBJECT

    signals:
        void getData(const QByteArray &data);

    public:
        explicit HexConsole(QWidget *parent = nullptr);
        void putData(const QByteArray &data, bool ehEnvioDeCaracteres = false);
        void setLocalEchoEnabled(bool set);

        bool getMonitoramentoSerialHabilitado() const;
        void setMonitoramentoSerialHabilitado(bool value);

    protected:
        void keyPressEvent(QKeyEvent *e) Q_DECL_OVERRIDE;
        void mousePressEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
        void mouseDoubleClickEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
        void contextMenuEvent(QContextMenuEvent *e) Q_DECL_OVERRIDE;

    private:
        bool localEchoEnabled;
        bool monitoramentoSerialHabilitado = false;
        int idxCharDigitado = 0;
        QByteArray caracteresDigitados;
        char valorHex;
        int tamanhoMaximoLinha;
        int qtdCaracteresDigitadosLinha = 0;
};

#endif // HEXCONSOLE_H
