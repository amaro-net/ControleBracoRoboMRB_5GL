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
