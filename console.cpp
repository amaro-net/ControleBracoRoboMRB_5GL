#include "console.h"

#include <QScrollBar>

#include <QtCore/QDebug>

#include <qtextdocument.h>

Console::Console(QWidget *parent)
    : QPlainTextEdit(parent)
    , localEchoEnabled(false)
{
    document()->setMaximumBlockCount(100);
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
    this->setFixedWidth(393);
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
            break;
        }
            //case Qt::Key_Enter:
        case Qt::Key_Left:
        case Qt::Key_Right:
        case Qt::Key_Up:
        case Qt::Key_Down:
            break;
        default:
            if (localEchoEnabled)
            {
                /*
                QPalette p = palette();
                p.setColor(QPalette::Base, Qt::black);
                //p.setColor(QPalette::Text, Qt::green);
                p.setColor(QPalette::Text, Qt::yellow);
                setPalette(p);
                QPlainTextEdit::keyPressEvent(e);*/

                QTextCursor cursor(this->textCursor());
                QTextCharFormat format;
                format.setForeground(QBrush(QColor(Qt::yellow)));
                cursor.setCharFormat(format);
                cursor.insertText(e->text());
            }

            emit getData(e->text().toLocal8Bit());
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
