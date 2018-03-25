#include "montagemdecomandosdialog.h"
#include "ui_montagemdecomandosdialog.h"
#include "constantes.h"

MontagemDeComandosDialog::MontagemDeComandosDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MontagemDeComandosDialog)
{
    ui->setupUi(this);

    if(mainWindow == NULL)
        mainWindow = (MainWindow *) parent;

    connect(ui->chkLEDP0, &QCheckBox::clicked, this, MontagemDeComandosDialog::on_chkLEDPi_clicked);
    connect(ui->chkLEDP1, &QCheckBox::clicked, this, MontagemDeComandosDialog::on_chkLEDPi_clicked);
    connect(ui->chkLEDP2, &QCheckBox::clicked, this, MontagemDeComandosDialog::on_chkLEDPi_clicked);
    connect(ui->chkLEDP3, &QCheckBox::clicked, this, MontagemDeComandosDialog::on_chkLEDPi_clicked);
    connect(ui->chkLEDP4, &QCheckBox::clicked, this, MontagemDeComandosDialog::on_chkLEDPi_clicked);
    connect(ui->chkLEDP5, &QCheckBox::clicked, this, MontagemDeComandosDialog::on_chkLEDPi_clicked);
    connect(ui->chkLEDP6, &QCheckBox::clicked, this, MontagemDeComandosDialog::on_chkLEDPi_clicked);
    connect(ui->chkLEDP7, &QCheckBox::clicked, this, MontagemDeComandosDialog::on_chkLEDPi_clicked);

    alimentarListasDeComponentes();    
}

MontagemDeComandosDialog::~MontagemDeComandosDialog()
{
    delete ui;
}

void MontagemDeComandosDialog::alimentarListasDeComponentes()
{
    QString nomeJunta[6] = {"Junta0", "Junta1", "Junta2", "Junta3", "Junta4", "Garra"};

    QString rdbJuntaCTZ_template("rdb%1");
    QString chkJuntaJST_template("chk%1");
    QString spnAlvo_template("spn%1Alvo");
    QString spnAlvoGraus_template("spn%1AlvoGraus");
    QString rdbJuntaVEL_template("rdb%1Vel");
    QString rdbJuntaACL_template("rdb%1Acl");

    for(int i = 0; i < QTD_SERVOS; i++)
    {
        QRadioButton *rdbJuntaCTZ = ui->pgCTZ->findChild<QRadioButton *>(rdbJuntaCTZ_template.arg(nomeJunta[i]));
        QCheckBox *chkJuntaJST = ui->pgJST->findChild<QCheckBox *>(chkJuntaJST_template.arg(nomeJunta[i]));
        QSpinBox *spnPosAlvo = ui->pgJST->findChild<QSpinBox *>(spnAlvo_template.arg(junta[i]));
        QDoubleSpinBox *spnPosAlvoGraus = ui->pgJST->findChild<QDoubleSpinBox *>(spnAlvoGraus_template.arg(junta[i]));
        QRadioButton *rdbJuntaVEL = ui->pgVel->findChild<QRadioButton *>(rdbJuntaVEL_template.arg(nomeJunta[i]));
        QRadioButton *rdbJuntaACL = ui->pgAcl->findChild<QRadioButton *>(rdbJuntaACL_template.arg(nomeJunta[i]));

        lstRdbJuntaCTZ.append(rdbJuntaCTZ);
        lstChkJuntaJST.append(chkJuntaJST);
        lstSpnPosAlvo.append(spnPosAlvo);
        lstSpnPosAlvoGraus.append(spnPosAlvoGraus);
        lstRdbJuntaVEL.append(rdbJuntaVEL);
        lstRdbJuntaACL.append(rdbJuntaACL);
    }

    QString chkLEDPi_template("chkLEDP%1");

    for(int i = 0; i < 8; i++)
    {
        QCheckBox *chkLEDPi = ui->pgLED->findChild<QCheckBox *>(chkLEDPi_template.arg(i));

        lstChkLEDPi.append(chkLEDPi);
    }
}

void MontagemDeComandosDialog::setMainWindow(MainWindow *mainUi)
{
    mainWindow = mainUi;
}

void MontagemDeComandosDialog::setEmEdicao(bool emEdicao)
{
    this->emEdicao = emEdicao;

    if(emEdicao)
    {
        ui->btAdicionarOuModificar->setText("Modificar");
        this->setWindowTitle("Edição de comando");
    }
    else
    {
        ui->btAdicionarOuModificar->setText("Adicionar");
        this->setWindowTitle("Novo comando");
    }
}

void MontagemDeComandosDialog::setTextEdtComando(QString comando)
{
    ui->edtComando->setText(comando);

    if(comando.trimmed().isEmpty() || comando.contains("GA") || comando.contains("GF") || comando.contains("RPS"))
        ui->toolBoxComandos->setCurrentIndex(0);
    else if(comando.contains("CTZ"))
    {
        ui->toolBoxComandos->setCurrentIndex(1);

        QString strJunta = comando.mid(4,2);

        int idxJunta = -1;

        if(strJunta.at(0) == 'J')
            idxJunta = strJunta.at(1).toLatin1() - '0';
        else if(strJunta.at(0) == 'G')
            idxJunta = 5;

        if(idxJunta != -1)
        {
            for(int i = 0; i < QTD_SERVOS; i++)
                lstRdbJuntaCTZ[i]->setChecked(false);
            lstRdbJuntaCTZ[idxJunta]->setChecked(true);
        }
    }
    else if(comando.contains("JST"))
    {
        ui->toolBoxComandos->setCurrentIndex(2);

        for(int i = 0; i < QTD_SERVOS; i++)
        {
            lstChkJuntaJST[i]->setChecked(false);
            lstSpnPosAlvo[i]->setEnabled(false);
            lstSpnPosAlvoGraus[i]->setEnabled(false);
        }

        int tam = comando.length();
        if(tam >= 10 && tam <= 35 && tam % 5 == 0)
        {
            int qtJuntas = (tam-5)/5;

            for(int i = 0; i < qtJuntas; i++)
            {
                char juntaJST = comando.at(5*i+4).toLatin1();

                int idxJunta = -1;
                if(juntaJST >= 'A' && juntaJST <= 'E')
                    idxJunta = juntaJST - 'A';
                else if (juntaJST == 'G')
                    idxJunta = 5;

                if(idxJunta != -1)
                {
                    lstChkJuntaJST[idxJunta]->setChecked(true);

                    int valor = comando.mid(5*(i+1), 4).toInt();
                    lstSpnPosAlvo[idxJunta]->setValue(valor);
                    lstSpnPosAlvo[idxJunta]->setEnabled(true);
                    lstSpnPosAlvoGraus[idxJunta]->setEnabled(true);
                }
            }
        }
    }
    else if(comando.contains("VEL"))
    {
        ui->toolBoxComandos->setCurrentIndex(3);

        if(comando.length() == 11)
        {
            setarVelOuAcl(comando, ui->spnVel);
        }
    }
    else if(comando.contains("ACL"))
    {
        ui->toolBoxComandos->setCurrentIndex(4);

        setarVelOuAcl(comando, ui->spnAcl);
    }
    else if(comando.contains("LED"))
    {
        ui->toolBoxComandos->setCurrentIndex(5);

        if(comando.length() == 13)
        {
            for(int i = 0; i < 8; i++)
            {
                lstChkLEDPi[i]->setChecked(comando.at(11-i) == '1');
            }
        }
    }
    else if(comando.contains("DLY"))
    {
        ui->toolBoxComandos->setCurrentIndex(6);

        if(comando.length() == 10)
        {
            int valor = comando.mid(4, 4).toInt();
            ui->spnTempoMs->setValue(valor);
        }
    }
    else
        ui->toolBoxComandos->setCurrentIndex(0);
}


void MontagemDeComandosDialog::setarVelOuAcl(QString comando, QSpinBox *spinBox)
{
    QString strJunta = comando.mid(4,2);
    QString strValor = comando.mid(6,4);

    int idxJunta = -1;

    if(strJunta.at(0) == 'J')
    {
        idxJunta = strJunta.at(1).toLatin1() - '0';
    }
    else if(strJunta.at(0) == 'G')
    {
        idxJunta = 5;
    }

    if(idxJunta != -1)
    {
        for(int i = 0; i < QTD_SERVOS; i++)
        {
            if(comando.contains("VEL"))
                lstRdbJuntaVEL[i]->setChecked(false);
            else if(comando.contains("ACL"))
                lstRdbJuntaACL[i]->setChecked(false);
        }

        if(comando.contains("VEL"))
            lstRdbJuntaVEL[idxJunta]->setChecked(true);
        else if(comando.contains("ACL"))
            lstRdbJuntaACL[idxJunta]->setChecked(true);

        int valor = strValor.toInt();
        spinBox->setValue(valor);
    }
}


void MontagemDeComandosDialog::setTabelaPosLimites(QTableWidget *tabelaPosLimites)
{
    for (int i = 0; i < QTD_SERVOS; i++)
    {
        for(int j = 0; j < 4; j++)
        {
            this->tabelaPosLimites[i][j] = tabelaPosLimites->item(i,j)->text().toInt();
        }

        tempoPulsoMax[i] = this->tabelaPosLimites[i][0];
        tempoPulsoMin[i] = this->tabelaPosLimites[i][1];
    }
}

void MontagemDeComandosDialog::setTabelaPosLimitesGraus(QTableWidget *tabelaPosLimitesGraus)
{
    for (int i = 0; i < QTD_SERVOS; i++)
    {        
        for(int j = 0; j < 4; j++)
        {
            this->tabelaPosLimitesGraus[i][j] = tabelaPosLimitesGraus->item(i,j)->text().toInt();
        }

        angMax[i] = this->tabelaPosLimitesGraus[i][0];
        angMin[i] = this->tabelaPosLimitesGraus[i][1];
    }
}

void MontagemDeComandosDialog::setIncrementosAng(double incrementosAng[])
{
    for(int i = 0; i < QTD_SERVOS; i++)
    {
        lstSpnPosAlvoGraus[i]->setDecimals(CASAS_DECIMAIS_POSICAO);
        lstSpnPosAlvoGraus[i]->setSingleStep(incrementosAng[i]);
    }
}

void MontagemDeComandosDialog::setIncVelGrausPorSeg(double incVelGrausPorSeg[])
{
    for(int i = 0; i < QTD_SERVOS; i++)
    {
        this->incVelGrausPorSeg[i] = incVelGrausPorSeg[i];
    }
    ui->spnVelGrausPorSeg->setDecimals(CASAS_DECIMAIS_VELOCIDADE);
}

void MontagemDeComandosDialog::setIncAclGrausPorSegQuad(double incAclGrausPorSegQuad[])
{
    for(int i = 0; i < QTD_SERVOS; i++)
    {
        this->incAclGrausPorSegQuad[i] = incAclGrausPorSegQuad[i];
    }
    ui->spnAclGrausPorSegQuad->setDecimals(CASAS_DECIMAIS_ACELERACAO);
}

void MontagemDeComandosDialog::setVelocidades(QList<QSpinBox *> lstSpnVel)
{
    for (int i = 0; i < QTD_SERVOS; i++)
    {
        this->velocidade[i] = lstSpnVel[i]->value();
    }
}

void MontagemDeComandosDialog::setAceleracoes(QList<QSpinBox *> lstSpnAcl)
{
    for (int i = 0; i < QTD_SERVOS; i++)
    {
        this->aceleracao[i] = lstSpnAcl[i]->value();
    }
}

void MontagemDeComandosDialog::setPosicoesAlvo(QList<QSpinBox *> lstSpnPosAlvo)
{
    for (int i = 0; i < QTD_SERVOS; i++)
    {
        this->posAlvo[i] = lstSpnPosAlvo[i]->value();
    }
}

void MontagemDeComandosDialog::on_edtComando_textChanged(const QString &newText)
{
    ui->btAdicionarOuModificar->setEnabled(newText.length() > 0);
}

void MontagemDeComandosDialog::on_btAdicionarOuModificar_clicked()
{    
    if(emEdicao)
    {
        mainWindow->editarComandoNaSequencia(ui->edtComando->text());
    }
    else
    {
        mainWindow->adicionarComandoAASequencia(ui->edtComando->text());
    }
    close();
}

void MontagemDeComandosDialog::on_btSair_clicked()
{    
    close();
}

void MontagemDeComandosDialog::on_btAbrirGarra_clicked()
{    
    ui->edtComando->setText("[GA]");
}

void MontagemDeComandosDialog::on_btFecharGarra_clicked()
{    
    ui->edtComando->setText("[GF]");
}

void MontagemDeComandosDialog::on_btRepouso_clicked()
{    
    ui->edtComando->setText("[RPS]");
}

void MontagemDeComandosDialog::on_btCTZ_clicked()
{    
    if(ui->rdbJunta0->isChecked())
    {
        ui->edtComando->setText("[CTZJ0]");
    }
    else if(ui->rdbJunta1->isChecked())
    {
        ui->edtComando->setText("[CTZJ1]");
    }
    else if(ui->rdbJunta2->isChecked())
    {
        ui->edtComando->setText("[CTZJ2]");
    }
    else if(ui->rdbJunta3->isChecked())
    {
        ui->edtComando->setText("[CTZJ3]");
    }
    else if(ui->rdbJunta4->isChecked())
    {
        ui->edtComando->setText("[CTZJ4]");
    }
    else if(ui->rdbGarra->isChecked())
    {
        ui->edtComando->setText("[CTZGR]");
    }
    else
    {
        ui->rdbJunta0->setChecked(true);
        ui->edtComando->setText("[CTZJ0]");
    }
}

void MontagemDeComandosDialog::onRdbJuntaCTZClicked(int idxJunta)
{
    ui->rdbJunta0->setChecked(idxJunta == 0);
    ui->rdbJunta2->setChecked(idxJunta == 1);
    ui->rdbJunta1->setChecked(idxJunta == 2);
    ui->rdbJunta3->setChecked(idxJunta == 3);
    ui->rdbJunta4->setChecked(idxJunta == 4);
    ui->rdbGarra->setChecked(idxJunta == 5);

    if(ui->edtComando->text().contains("CTZ"))
        on_btCTZ_clicked();
}

void MontagemDeComandosDialog::on_rdbJunta0_clicked()
{
    onRdbJuntaCTZClicked(0);
}

void MontagemDeComandosDialog::on_rdbJunta1_clicked()
{
    onRdbJuntaCTZClicked(1);
}

void MontagemDeComandosDialog::on_rdbJunta2_clicked()
{
    onRdbJuntaCTZClicked(2);
}

void MontagemDeComandosDialog::on_rdbJunta3_clicked()
{
    onRdbJuntaCTZClicked(3);
}

void MontagemDeComandosDialog::on_rdbJunta4_clicked()
{
    onRdbJuntaCTZClicked(4);
}

void MontagemDeComandosDialog::on_rdbGarra_clicked()
{
    onRdbJuntaCTZClicked(5);
}

void MontagemDeComandosDialog::on_btJST_clicked()
{    
    QString comandoJST = "[JST";

    for(int i = 0; i < QTD_SERVOS; i++)
    {
        if(lstChkJuntaJST[i]->isChecked())
        {
            int valor = lstSpnPosAlvo[i]->value();
            comandoJST += QString("%1%2").arg(idJST[i]).arg(valor, 4, 10, QChar('0'));
        }
    }

    comandoJST += "]";

    ui->edtComando->setText(comandoJST);
}

void MontagemDeComandosDialog::onChkJuntaJSTClicked(int idxJunta, bool checked)
{
    lstSpnPosAlvo[idxJunta]->setEnabled(checked);
    lstSpnPosAlvoGraus[idxJunta]->setEnabled(checked);

    if(ui->edtComando->text().contains("JST"))
        on_btJST_clicked();
}

void MontagemDeComandosDialog::on_chkJunta0_clicked(bool checked)
{
    onChkJuntaJSTClicked(0, checked);
}

void MontagemDeComandosDialog::on_chkJunta1_clicked(bool checked)
{
    onChkJuntaJSTClicked(1, checked);
}

void MontagemDeComandosDialog::on_chkJunta2_clicked(bool checked)
{
    onChkJuntaJSTClicked(2, checked);
}

void MontagemDeComandosDialog::on_chkJunta3_clicked(bool checked)
{
    onChkJuntaJSTClicked(3, checked);
}

void MontagemDeComandosDialog::on_chkJunta4_clicked(bool checked)
{
    onChkJuntaJSTClicked(4, checked);
}

void MontagemDeComandosDialog::on_chkGarra_clicked(bool checked)
{
    onChkJuntaJSTClicked(5, checked);
}

void MontagemDeComandosDialog::onSpnAlvo_valueChanged(int idxJunta, int posicaoMicrossegundos)
{
    if(posicaoMicrossegundos > 1 && posicaoMicrossegundos <= tempoPulsoMin[idxJunta] - 1)
        lstSpnPosAlvo[idxJunta]->setValue(0);
    else if(posicaoMicrossegundos == 1)
        lstSpnPosAlvo[idxJunta]->setValue(tempoPulsoMin[idxJunta]);
    else if(posicaoMicrossegundos > 0 && posicaoMicrossegundos > tempoPulsoMax[idxJunta])
        lstSpnPosAlvo[idxJunta]->setValue(tempoPulsoMax[idxJunta]);
    else if(posicaoMicrossegundos >= tempoPulsoMin[idxJunta] &&
            posicaoMicrossegundos <= tempoPulsoMax[idxJunta])
    {
        double angulo = mainWindow->converteMicrossegundosParaGraus(idxJunta, posicaoMicrossegundos);
        double anguloAtual = lstSpnPosAlvoGraus[idxJunta]->value();

        if(angulo != anguloAtual)
            lstSpnPosAlvoGraus[idxJunta]->setValue(angulo);
    }
    if(ui->edtComando->text().contains("JST"))
        on_btJST_clicked();
}

void MontagemDeComandosDialog::on_spnJ0Alvo_valueChanged(int posicaoMicrossegundos)
{
    onSpnAlvo_valueChanged(0, posicaoMicrossegundos);
}

void MontagemDeComandosDialog::on_spnJ1Alvo_valueChanged(int posicaoMicrossegundos)
{    
    onSpnAlvo_valueChanged(1, posicaoMicrossegundos);
}

void MontagemDeComandosDialog::on_spnJ2Alvo_valueChanged(int posicaoMicrossegundos)
{
    onSpnAlvo_valueChanged(2, posicaoMicrossegundos);
}

void MontagemDeComandosDialog::on_spnJ3Alvo_valueChanged(int posicaoMicrossegundos)
{
    onSpnAlvo_valueChanged(3, posicaoMicrossegundos);
}

void MontagemDeComandosDialog::on_spnJ4Alvo_valueChanged(int posicaoMicrossegundos)
{
    onSpnAlvo_valueChanged(4, posicaoMicrossegundos);
}

void MontagemDeComandosDialog::on_spnGRAlvo_valueChanged(int posicaoMicrossegundos)
{
    onSpnAlvo_valueChanged(5, posicaoMicrossegundos);
}

void MontagemDeComandosDialog::onSpnJuntaAlvoGraus_valueChanged(int idxJunta, double posicaoGraus)
{
    if(posicaoGraus < angMin[idxJunta])
        lstSpnPosAlvoGraus[idxJunta]->setValue(angMin[idxJunta]);
    else if(posicaoGraus > angMax[idxJunta])
        lstSpnPosAlvoGraus[idxJunta]->setValue(angMax[idxJunta]);
    else
    {
        int posicaoMicrossegundos = mainWindow->converteGrausParaMicrossegundos(idxJunta, posicaoGraus);
        int posicaoMicrossegundosAtual = lstSpnPosAlvo[idxJunta]->value();

        if(posicaoMicrossegundos != posicaoMicrossegundosAtual)
            lstSpnPosAlvo[idxJunta]->setValue(posicaoMicrossegundos);
    }
}

void MontagemDeComandosDialog::on_spnJ0AlvoGraus_valueChanged(double posicaoGraus)
{
    onSpnJuntaAlvoGraus_valueChanged(0, posicaoGraus);
}

void MontagemDeComandosDialog::on_spnJ1AlvoGraus_valueChanged(double posicaoGraus)
{
    onSpnJuntaAlvoGraus_valueChanged(1, posicaoGraus);
}

void MontagemDeComandosDialog::on_spnJ2AlvoGraus_valueChanged(double posicaoGraus)
{
    onSpnJuntaAlvoGraus_valueChanged(2, posicaoGraus);
}

void MontagemDeComandosDialog::on_spnJ3AlvoGraus_valueChanged(double posicaoGraus)
{
    onSpnJuntaAlvoGraus_valueChanged(3, posicaoGraus);
}

void MontagemDeComandosDialog::on_spnJ4AlvoGraus_valueChanged(double posicaoGraus)
{
    onSpnJuntaAlvoGraus_valueChanged(4, posicaoGraus);
}

void MontagemDeComandosDialog::on_spnGRAlvoGraus_valueChanged(double posicaoGraus)
{
    onSpnJuntaAlvoGraus_valueChanged(5, posicaoGraus);
}



void MontagemDeComandosDialog::on_btVel_clicked()
{    
    QString comandoVEL = "[VEL";
    bool temAlgumaJuntaMarcada = false;

    for(int i = 0; i < QTD_SERVOS && !temAlgumaJuntaMarcada; i++)
    {
        if(lstRdbJuntaVEL[i]->isChecked())
        {
            ui->spnVelGrausPorSeg->setSingleStep(incVelGrausPorSeg[i]);

            int valor = ui->spnVel->value();
            comandoVEL += QString("%1%2").arg(junta[i]).arg(valor, 4, 10, QChar('0'));
            temAlgumaJuntaMarcada = true;
        }
    }

    if(!temAlgumaJuntaMarcada)
    {
        ui->rdbJunta0Vel->setChecked(true);

        ui->spnVelGrausPorSeg->setSingleStep(incVelGrausPorSeg[0]);

        int valor = ui->spnVel->value();
        comandoVEL += QString("%1%2").arg(junta[0]).arg(valor, 4, 10, QChar('0'));
    }

    comandoVEL += "]";

    ui->edtComando->setText(comandoVEL);
}

void MontagemDeComandosDialog::onRdbJuntaVelClicked(int idxJunta)
{
    ui->rdbJunta0Vel->setChecked(idxJunta == 0);
    ui->rdbJunta1Vel->setChecked(idxJunta == 1);
    ui->rdbJunta2Vel->setChecked(idxJunta == 2);
    ui->rdbJunta3Vel->setChecked(idxJunta == 3);
    ui->rdbJunta4Vel->setChecked(idxJunta == 4);
    ui->rdbGarraVel->setChecked(idxJunta == 5);

    ui->spnVelGrausPorSeg->setSingleStep(incVelGrausPorSeg[idxJunta]);

    if(ui->edtComando->text().contains("VEL"))
        on_btVel_clicked();
}

void MontagemDeComandosDialog::on_rdbJunta0Vel_clicked()
{
    onRdbJuntaVelClicked(0);
}

void MontagemDeComandosDialog::on_rdbJunta1Vel_clicked()
{
    onRdbJuntaVelClicked(1);
}

void MontagemDeComandosDialog::on_rdbJunta2Vel_clicked()
{
    onRdbJuntaVelClicked(2);
}

void MontagemDeComandosDialog::on_rdbJunta3Vel_clicked()
{
    onRdbJuntaVelClicked(3);
}

void MontagemDeComandosDialog::on_rdbJunta4Vel_clicked()
{
    onRdbJuntaVelClicked(4);
}

void MontagemDeComandosDialog::on_rdbGarraVel_clicked()
{
    onRdbJuntaVelClicked(5);
}

int MontagemDeComandosDialog::pegaJuntaMarcadaVel()
{
    int idxJunta = -1;

    for(int i = 0; i < QTD_SERVOS && idxJunta == -1; i++)
    {
        if(lstRdbJuntaVEL[i]->isChecked())
        {
            idxJunta = i;
        }
    }

    if(idxJunta == -1)
    {
        idxJunta = 0;
        lstRdbJuntaVEL[idxJunta]->setChecked(true);
        onRdbJuntaVelClicked(idxJunta);
    }

    return idxJunta;
}

void MontagemDeComandosDialog::on_spnVel_valueChanged(int velTmpPulso)
{
    int idxJunta = pegaJuntaMarcadaVel();

    double velocidadeAngular = mainWindow->converteVelTmpPulsoParaGrausPorSeg(idxJunta, velTmpPulso);
    double velocidadeAngularAtual = ui->spnVelGrausPorSeg->value();

    if(velocidadeAngular != velocidadeAngularAtual)
    {
        ui->spnVelGrausPorSeg->setValue(velocidadeAngular);
    }

    if(ui->edtComando->text().contains("VEL"))
        on_btVel_clicked();
}

void MontagemDeComandosDialog::on_spnVelGrausPorSeg_valueChanged(double velGrausPorSeg)
{
    int idxJunta = pegaJuntaMarcadaVel();

    int velTmpPulso = mainWindow->converteVelGrausPorSegParaTmpPulso(idxJunta, velGrausPorSeg);
    int velTmpPulsoAtual = ui->spnVel->value();

    if(velTmpPulso != velTmpPulsoAtual)
    {
        ui->spnVel->setValue(velTmpPulso);
    }
}

void MontagemDeComandosDialog::on_btAcl_clicked()
{    
    QString comandoACL = "[ACL";
    bool temAlgumaJuntaMarcada = false;

    for(int i = 0; i < QTD_SERVOS && !temAlgumaJuntaMarcada; i++)
    {
        if(lstRdbJuntaACL[i]->isChecked())
        {
            ui->spnAclGrausPorSegQuad->setSingleStep(incAclGrausPorSegQuad[i]);

            int valor = ui->spnAcl->value();
            comandoACL += QString("%1%2").arg(junta[i]).arg(valor, 4, 10, QChar('0'));
            temAlgumaJuntaMarcada = true;
        }
    }

    if(!temAlgumaJuntaMarcada)
    {
        ui->rdbJunta0Acl->setChecked(true);

        ui->spnAclGrausPorSegQuad->setSingleStep(incAclGrausPorSegQuad[0]);

        int valor = ui->spnAcl->value();
        comandoACL += QString("%1%2").arg(junta[0]).arg(valor, 4, 10, QChar('0'));
    }

    comandoACL += "]";

    ui->edtComando->setText(comandoACL);
}

void MontagemDeComandosDialog::onRdbJuntaAclClicked(int idxJunta)
{
    ui->rdbJunta0Acl->setChecked(idxJunta == 0);
    ui->rdbJunta1Acl->setChecked(idxJunta == 1);
    ui->rdbJunta2Acl->setChecked(idxJunta == 2);
    ui->rdbJunta3Acl->setChecked(idxJunta == 3);
    ui->rdbJunta4Acl->setChecked(idxJunta == 4);
    ui->rdbGarraAcl->setChecked(idxJunta == 5);

    ui->spnAclGrausPorSegQuad->setSingleStep(incAclGrausPorSegQuad[idxJunta]);

    if(ui->edtComando->text().contains("ACL"))
        on_btAcl_clicked();
}

void MontagemDeComandosDialog::on_rdbJunta0Acl_clicked()
{
    onRdbJuntaAclClicked(0);
}

void MontagemDeComandosDialog::on_rdbJunta1Acl_clicked()
{
    onRdbJuntaAclClicked(1);
}

void MontagemDeComandosDialog::on_rdbJunta2Acl_clicked()
{
    onRdbJuntaAclClicked(2);
}

void MontagemDeComandosDialog::on_rdbJunta3Acl_clicked()
{
    onRdbJuntaAclClicked(3);
}

void MontagemDeComandosDialog::on_rdbJunta4Acl_clicked()
{
    onRdbJuntaAclClicked(4);
}

void MontagemDeComandosDialog::on_rdbGarraAcl_clicked()
{
    onRdbJuntaAclClicked(5);
}

int MontagemDeComandosDialog::pegaJuntaMarcadaAcl()
{
    int idxJunta = -1;

    for(int i = 0; i < QTD_SERVOS && idxJunta == -1; i++)
    {
        if(lstRdbJuntaACL[i]->isChecked())
        {
            idxJunta = i;
        }
    }

    if(idxJunta == -1)
    {
        idxJunta = 0;
        lstRdbJuntaACL[idxJunta]->setChecked(true);
        onRdbJuntaAclClicked(idxJunta);
    }

    return idxJunta;
}

void MontagemDeComandosDialog::on_spnAcl_valueChanged(int aclTmpPulso)
{
    int idxJunta = pegaJuntaMarcadaAcl();

    double aceleracaoAngular = mainWindow->converteAclTmpPulsoParaGrausPorSegQuad(idxJunta, aclTmpPulso);
    double aceleracaoAngularAtual = ui->spnAclGrausPorSegQuad->value();

    if(aceleracaoAngular != aceleracaoAngularAtual)
    {
        ui->spnAclGrausPorSegQuad->setValue(aceleracaoAngular);
    }

    if(ui->edtComando->text().contains("ACL"))
        on_btAcl_clicked();    
}

void MontagemDeComandosDialog::on_spnAclGrausPorSegQuad_valueChanged(double aclGrausPorSegQuad)
{
    int idxJunta = pegaJuntaMarcadaAcl();

    int aclTmpPulso = mainWindow->converteAclGrausPorSegQuadParaTmpPulso(idxJunta, aclGrausPorSegQuad);
    int aclTmpPulsoAtual = ui->spnAcl->value();

    if(aclTmpPulso != aclTmpPulsoAtual)
    {
        ui->spnAcl->setValue(aclTmpPulso);
    }
}


void MontagemDeComandosDialog::on_btLED_clicked()
{    
    QString cmdLED = "[LED";

    for (int i = 7; i >= 0; i--)
    {
        cmdLED += lstChkLEDPi[i]->isChecked() ? "1" : "0";
    }
    cmdLED += "]";

    ui->edtComando->setText(cmdLED);
}

void MontagemDeComandosDialog::on_chkLEDPi_clicked()
{    
    if(ui->edtComando->text().contains("LED"))
        on_btLED_clicked();
}


void MontagemDeComandosDialog::on_btDLY_clicked()
{
    int valor = ui->spnTempoMs->value();
    QString cmdDLY = "[DLY" + QString("%1").arg(valor, 4, 10, QChar('0'))+"]";
    ui->edtComando->setText(cmdDLY);
}


void MontagemDeComandosDialog::on_spnTempoMs_valueChanged(int arg1)
{
    Q_UNUSED(arg1)

    if(ui->edtComando->text().contains("DLY"))
        on_btDLY_clicked();
}


