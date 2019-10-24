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
#include "hexconsole.h"

#include <QScrollBar>

#include <QtCore/QDebug>

#include <qtextdocument.h>

HexConsole::HexConsole(QWidget *parent)
    : QPlainTextEdit(parent)
    , localEchoEnabled(false)
{
    document()->setMaximumBlockCount(10000);
    QPalette p = palette();
    p.setColor(QPalette::Base, Qt::black);
    //p.setColor(QPalette::Text, Qt::green);
    p.setColor(QPalette::Text, Qt::yellow);
    setPalette(p);


    QFont font = document()->defaultFont();
    font.setFamily("Terminal");
    font.setPointSize(10);
    document()->setDefaultFont(font);

    this->setFixedWidth(331);
    this->setFixedHeight(350);
    tamanhoMaximoLinha = 62;

    caracteresDigitados.clear();
    valorHex = 0;
}

void HexConsole::putData(const QByteArray &data, bool ehEnvioDeCaracteres)
{
    this->textCursor().clearSelection();

    if(!monitoramentoSerialHabilitado)
        return;

    QTextCursor cursor(this->textCursor());

    QTextCharFormat format;

    if(ehEnvioDeCaracteres)
    {        
        format.setForeground(QBrush(QColor(Qt::yellow)));
    }
    else
    {
        cursor.insertText("\n");
        format.setForeground(QBrush(QColor(Qt::green)));
    }

    cursor.setCharFormat(format);

    for(int i = 0; i < data.length(); i++)
    {
        unsigned char hx = static_cast<unsigned char>(data.at(i));
        QString str = QString("%1").arg(hx, 2, 16, QLatin1Char('0'));

        cursor.insertText(str);

        if(ehEnvioDeCaracteres)
        {
            qtdCaracteresDigitadosLinha += 2;

            if(qtdCaracteresDigitadosLinha > tamanhoMaximoLinha)
            {
                cursor.insertText("\n");
                qtdCaracteresDigitadosLinha = 0;
            }
        }
        else
        {
            if(i == data.length() - 1)
            {
                cursor.insertText("\n");
                qtdCaracteresDigitadosLinha = 0;
            }
        }
    }


    QScrollBar *bar = verticalScrollBar();
    bar->setValue(bar->maximum());
}

void HexConsole::setLocalEchoEnabled(bool set)
{
    localEchoEnabled = set;
}

void HexConsole::keyPressEvent(QKeyEvent *e)
{
    if(!monitoramentoSerialHabilitado)
        return;

    switch (e->key()) {
        case Qt::Key_Backspace:
        {
            if(idxCharDigitado > 0)
            {
                if(localEchoEnabled)
                {
                    int qtLinhas = this->document()->blockCount();
                    this->textCursor().deletePreviousChar();
                    if(qtLinhas > this->document()->blockCount())
                        insertPlainText(QString("\n"));
                    //QPlainTextEdit::keyPressEvent(e);
                    idxCharDigitado--;
                    qtdCaracteresDigitadosLinha--;
                }
                //emit getData(e->text().toLocal8Bit());
            }

            break;
        }
            //case Qt::Key_Enter:
        case Qt::Key_Left:
        case Qt::Key_Right:
        case Qt::Key_Up:
            // TODO: Mini Maestro 24: Incluir lista/fila de comandos já executados. Avaliar se faz sentido incluir os que foram executados automaticamente.
        case Qt::Key_Down:
            break;
        default:            
            if(e->text().trimmed().isNull() || e->text().trimmed().isEmpty())
                break;

            char c = e->text().toUpper().at(0).toLatin1();
            if ( !((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F')))
                break;

            if(idxCharDigitado < 2)
            {
                caracteresDigitados.append(c);
                char hexCorrespondente = 0;
                if (c >= 'A' && c <= 'F')
                {
                    hexCorrespondente = c - 'A' + 10;
                }
                else //if(c >= '0' && c <= '9')
                {
                    hexCorrespondente = c - '0';
                }

                unsigned char multiplicador = 1;
                for(int i = 0; i < 1 - idxCharDigitado; i++)
                    multiplicador *= 16;

                valorHex = char(static_cast<unsigned char>(valorHex) + multiplicador * static_cast<unsigned char>(hexCorrespondente));
                idxCharDigitado++;
            }

            if (localEchoEnabled)
            {
                QTextCursor cursor(this->textCursor());
                QTextCharFormat format;
                format.setForeground(QBrush(QColor(Qt::yellow)));
                cursor.setCharFormat(format);

                cursor.insertText(e->text());

                if(++qtdCaracteresDigitadosLinha > tamanhoMaximoLinha)
                {
                    cursor.insertText("\n");
                    qtdCaracteresDigitadosLinha = 0;
                }
            }

            if(idxCharDigitado >= 2)
            {
                idxCharDigitado = 0;

                QString str = QString("%1").arg(valorHex);

                emit getData(str.toLatin1());

                caracteresDigitados.clear();
                valorHex = 0;
            }
    }
}

void HexConsole::mousePressEvent(QMouseEvent *e)
{
    Q_UNUSED(e)
    setFocus();
}

void HexConsole::mouseDoubleClickEvent(QMouseEvent *e)
{
    Q_UNUSED(e)
}

void HexConsole::contextMenuEvent(QContextMenuEvent *e)
{
    Q_UNUSED(e)
}

bool HexConsole::getMonitoramentoSerialHabilitado() const
{
    return monitoramentoSerialHabilitado;
}

void HexConsole::setMonitoramentoSerialHabilitado(bool value)
{
    monitoramentoSerialHabilitado = value;
}
