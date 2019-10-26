#ifndef JANELASOBRE_H
#define JANELASOBRE_H

#include <QMainWindow>
#include "janelalicencagpl.h"

namespace Ui {
    class JanelaSobre;
}

class JanelaLicencaGPL;

class JanelaSobre : public QMainWindow
{
    Q_OBJECT

public:
    explicit JanelaSobre(QWidget *parent = nullptr);
    ~JanelaSobre();

private slots:
    void on_btGPLv3_clicked();

private:
    Ui::JanelaSobre *ui;
    JanelaLicencaGPL *janelaLicencaGPL;
};

#endif // JANELASOBRE_H
