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
    p.setColor(QPalette::Text, Qt::green);
    setPalette(p);


    QFont font = document()->defaultFont();
    font.setFamily("Terminal");
    font.setPointSize(10);
    document()->setDefaultFont(font);

    this->setFixedHeight(350);
    this->setFixedWidth(393);
}

void Console::putData(const QByteArray &data)
{
    this->textCursor().clearSelection();

    if (data.contains("\010") || data.contains("\r"))
    {
        for(int i = 0; i < data.size(); i++)
        {
            if(data.at(i) == '\010')
            {
                int qtLinhas = this->document()->blockCount();
                this->textCursor().deletePreviousChar();
                if(qtLinhas > this->document()->blockCount())
                    insertPlainText(QString("\n"));
            }
            else if(data.at(i) == '\r')
            {
                //ignora o \r
            }
            else
                insertPlainText(QString(data.at(i)));

        }
    }
    else
        insertPlainText(QString(data));

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
    //case Qt::Key_Backspace:
        //this->textCursor().deletePreviousChar();
    //case Qt::Key_Enter:
    case Qt::Key_Left:
    case Qt::Key_Right:
    case Qt::Key_Up:
    case Qt::Key_Down:
        break;
    default:
        if (localEchoEnabled)
            QPlainTextEdit::keyPressEvent(e);
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
