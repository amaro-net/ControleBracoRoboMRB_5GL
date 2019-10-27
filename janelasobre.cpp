#include "janelasobre.h"

#if defined(Q_OS_WIN)
    #include "ui_janelasobre.h"
#elif defined(Q_OS_LINUX)
    #include "ui_janelasobre_linux.h"
#endif

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
