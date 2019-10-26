#include "janelalicencagpl.h"
#include "ui_janelalicencagpl.h"

JanelaLicencaGPL::JanelaLicencaGPL(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::JanelaLicencaGPL)
{
    ui->setupUi(this);
}

JanelaLicencaGPL::~JanelaLicencaGPL()
{
    delete ui;
}
