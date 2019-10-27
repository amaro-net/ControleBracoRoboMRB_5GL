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
#include "console.h"

#include <QScrollBar>

#include <QtCore/QDebug>

#include <qtextdocument.h>

Console::Console(QWidget *parent)
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

    this->setFixedHeight(350);
#if defined(Q_OS_WIN)
    this->setFixedWidth(393);
#elif defined(Q_OS_LINUX)
    this->setFixedWidth(471);
#endif
}

void Console::putData(const QByteArray &data, bool ehEnvioDeCaracteres)
{
    this->textCursor().clearSelection();

    QTextCursor cursor(this->textCursor());

    QTextCharFormat format;

    if(ehEnvioDeCaracteres)
    {
        format.setForeground(QBrush(QColor(Qt::yellow)));
    }
    else
    {
        format.setForeground(QBrush(QColor(Qt::green)));
    }

    cursor.setCharFormat(format);

    if (data.contains("\010") || data.contains("\r"))
    {
        for(int i = 0; i < data.size(); i++)
        {
            if(data.at(i) == '\010')
            {
                int qtLinhas = this->document()->blockCount();
                this->textCursor().deletePreviousChar();
                if(qtLinhas > this->document()->blockCount())
                    cursor.insertText(QString("\n"));
            }
            else if(data.at(i) == '\r')
            {
                //ignora o \r
            }
            else
                cursor.insertText(QString(data.at(i)));

        }
    }
    else
        cursor.insertText(QString(data));

    QScrollBar *bar = verticalScrollBar();
    bar->setValue(bar->maximum());
}

void Console::setLocalEchoEnabled(bool set)
{
    localEchoEnabled = set;
}

void Console::keyPressEvent(QKeyEvent *e)
{
    switch (e->key()) {
        case Qt::Key_Backspace:
        {
            int qtLinhas = this->document()->blockCount();
            this->textCursor().deletePreviousChar();
            if(qtLinhas > this->document()->blockCount())
                insertPlainText(QString("\n"));
            //QPlainTextEdit::keyPressEvent(e);
            emit getData(e->text().toLocal8Bit());
            break;
        }
            //case Qt::Key_Enter:
        case Qt::Key_Left:
        case Qt::Key_Right:
        case Qt::Key_Up:
            // TODO: Incluir lista/fila de comandos já executados. Avaliar se faz sentido incluir os que foram executados automaticamente.
        case Qt::Key_Down:
            break;
        default:
            if (localEchoEnabled)
            {
                QTextCursor cursor(this->textCursor());
                QTextCharFormat format;
                format.setForeground(QBrush(QColor(Qt::yellow)));
                cursor.setCharFormat(format);
                cursor.insertText(e->text().toUpper());
            }

            emit getData(e->text().toUpper().toLocal8Bit());
    }
}

void Console::mousePressEvent(QMouseEvent *e)
{
    Q_UNUSED(e)
    setFocus();
}

void Console::mouseDoubleClickEvent(QMouseEvent *e)
{
    Q_UNUSED(e)
}

void Console::contextMenuEvent(QContextMenuEvent *e)
{
    Q_UNUSED(e)
}
