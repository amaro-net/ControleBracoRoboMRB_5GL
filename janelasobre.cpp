#include "janelasobre.h"
#include "ui_janelasobre.h"

JanelaSobre::JanelaSobre(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::JanelaSobre)
{
    ui->setupUi(this);
    janelaLicencaGPL = new JanelaLicencaGPL(this);
}

JanelaSobre::~JanelaSobre()
{
    delete ui;
}

void JanelaSobre::on_btGPLv3_clicked()
{
    janelaLicencaGPL->show();
}
