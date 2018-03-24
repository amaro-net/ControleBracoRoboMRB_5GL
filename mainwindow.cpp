#include <QMessageBox>
#include <QtSerialPort/QSerialPortInfo>
#include <QFile>
#include <QTextStream>
#include <QFileDialog>
#include <math.h>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "constantes.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QLocale::setDefault(QLocale::system());

    console = new Console(ui->tabTerminal);
    console->setEnabled(false);
    console->setVisible(true);

    serial = new QSerialPort(this);

    montagemDeComandosDialog = new MontagemDeComandosDialog(this);
    montagemDeComandosDialog->setMainWindow(this);

    preencheCombosPortaSerial();    

    connect(serial, static_cast<void (QSerialPort::*)(QSerialPort::SerialPortError)>(&QSerialPort::error),
            this, &MainWindow::handleError);

    connect(serial, &QSerialPort::readyRead, this, &MainWindow::readData);

    connect(console, &Console::getData, this, &MainWindow::writeData);

    conectarComponentes();

    alimentarListasDeComponentes();    

    bufferProto = "";

    ui->lblComandoAcionado->setText("");
    ui->listaUltimoComandoAcionado->clear();
    ui->edtComandoLEDAcionado->setText("");

    on_btNovaSequencia_clicked();

    ui->tabPrincipal->setCurrentIndex(0);

    timerDLY = new QTimer(this);
    timerDLY->setSingleShot(true);

    timer = new QTimer(this);
    timer->setSingleShot(true);

    timerEnvioImediato = new QTimer(this);
    timerEnvioImediato->setSingleShot(true);

    connect(timerDLY, &QTimer::timeout, this, &MainWindow::timeoutDLY);
    connect(timer, &QTimer::timeout, this, &MainWindow::timeoutConfig);
    connect(timerEnvioImediato, &QTimer::timeout, this, &MainWindow::timeoutEnvioImediato);

    //configurarConversaoEntreMicrossegundosEAngulos(true);
}

void MainWindow::conectarComponentes()
{
    connect(ui->btAbrirPortaSerial, &QPushButton::clicked, this, &MainWindow::abrirPortaSerial);
    connect(ui->btFecharPortaSerial, &QPushButton::clicked, this, &MainWindow::fecharPortaSerial);

    connect(ui->chkLEDP0, &QCheckBox::clicked, this, MainWindow::on_chkLEDPi_clicked);
    connect(ui->chkLEDP1, &QCheckBox::clicked, this, MainWindow::on_chkLEDPi_clicked);
    connect(ui->chkLEDP2, &QCheckBox::clicked, this, MainWindow::on_chkLEDPi_clicked);
    connect(ui->chkLEDP3, &QCheckBox::clicked, this, MainWindow::on_chkLEDPi_clicked);
    connect(ui->chkLEDP4, &QCheckBox::clicked, this, MainWindow::on_chkLEDPi_clicked);
    connect(ui->chkLEDP5, &QCheckBox::clicked, this, MainWindow::on_chkLEDPi_clicked);
    connect(ui->chkLEDP6, &QCheckBox::clicked, this, MainWindow::on_chkLEDPi_clicked);
    connect(ui->chkLEDP7, &QCheckBox::clicked, this, MainWindow::on_chkLEDPi_clicked);
}

void MainWindow::alimentarListasDeComponentes()
{
    QString chkHab_template("chk%1");
    QString slider_template("slider%1");

    QString spnAlvo_template("spn%1Alvo");
    QString edtAtual_template("edt%1Atual");
    QString spnVel_template("spn%1Vel");
    QString spnAcl_template("spn%1Acl");

    QString spnAlvoGraus_template("spn%1AlvoGraus");
    QString edtAtualGraus_template("edt%1AtualGraus");
    QString spnVelGrausPorSeg_template("spn%1VelGrausPorSeg");
    QString spnAclGrausPorSegQuad_template("spn%1AclGrausPorSegQuad");

    for(int i = 0; i < QTD_SERVOS; i++)
    {
        QCheckBox *chkHab = ui->tabPosicoesDasJuntas->findChild<QCheckBox *>(chkHab_template.arg(junta[i]));
        QSlider *slider = ui->tabPosicoesDasJuntas->findChild<QSlider *>(slider_template.arg(junta[i]));

        QSpinBox *spnAlvo = ui->tabLarguraDePulso->findChild<QSpinBox *>(spnAlvo_template.arg(junta[i]));
        QLineEdit *edtAtual = ui->tabLarguraDePulso->findChild<QLineEdit *>(edtAtual_template.arg(junta[i]));
        QSpinBox *spnVel = ui->tabLarguraDePulso->findChild<QSpinBox *>(spnVel_template.arg(junta[i]));
        QSpinBox *spnAcl = ui->tabLarguraDePulso->findChild<QSpinBox *>(spnAcl_template.arg(junta[i]));

        QDoubleSpinBox *spnAlvoGraus = ui->tabAngulosDasJuntas->findChild<QDoubleSpinBox *>(spnAlvoGraus_template.arg(junta[i]));
        QLineEdit *edtAtualGraus = ui->tabAngulosDasJuntas->findChild<QLineEdit *>(edtAtualGraus_template.arg(junta[i]));
        QDoubleSpinBox *spnVelGrausPorSeg  = ui->tabAngulosDasJuntas->findChild<QDoubleSpinBox *>(spnVelGrausPorSeg_template.arg(junta[i]));
        QDoubleSpinBox *spnAclGrausPorSegQuad = ui->tabAngulosDasJuntas->findChild<QDoubleSpinBox *>(spnAclGrausPorSegQuad_template.arg(junta[i]));

        lstChkHab.append(chkHab);
        lstSlider.append(slider);

        lstSpnAlvo.append(spnAlvo);
        lstEdtAtual.append(edtAtual);
        lstSpnVel.append(spnVel);
        lstSpnAcl.append(spnAcl);

        lstSpnAlvoGraus.append(spnAlvoGraus);
        lstEdtAtualGraus.append(edtAtualGraus);
        lstSpnVelGrausPorSeg.append(spnVelGrausPorSeg);
        lstSpnAclGrausPorSegQuad.append(spnAclGrausPorSegQuad);
    }

    QString chkLed_template("chkLEDP%1");
    for(int i = 0; i < 8; i++)
    {
        QCheckBox *chkLed = ui->tabComandos->findChild<QCheckBox *>(chkLed_template.arg(i));
        lstChkLeds.append(chkLed);
    }
}

void MainWindow::alimentarFilaDeComandosDeInicializacao()
{
    filaComandosInicializacao.clear();

    filaComandosInicializacao.enqueue("[FRS1]");
    filaComandosInicializacao.enqueue("[CSB0]");    

    filaComandosInicializacao.enqueue("[TMXJ0]");
    filaComandosInicializacao.enqueue("[TMXJ1]");
    filaComandosInicializacao.enqueue("[TMXJ2]");
    filaComandosInicializacao.enqueue("[TMXJ3]");
    filaComandosInicializacao.enqueue("[TMXJ4]");
    filaComandosInicializacao.enqueue("[TMXGR]");

    filaComandosInicializacao.enqueue("[TMNJ0]");
    filaComandosInicializacao.enqueue("[TMNJ1]");
    filaComandosInicializacao.enqueue("[TMNJ2]");
    filaComandosInicializacao.enqueue("[TMNJ3]");
    filaComandosInicializacao.enqueue("[TMNJ4]");
    filaComandosInicializacao.enqueue("[TMNGR]");

    filaComandosInicializacao.enqueue("[T90J0]");
    filaComandosInicializacao.enqueue("[T90J1]");
    filaComandosInicializacao.enqueue("[T90J2]");
    filaComandosInicializacao.enqueue("[T90J3]");
    filaComandosInicializacao.enqueue("[T90J4]");
    filaComandosInicializacao.enqueue("[T90GR]");

    filaComandosInicializacao.enqueue("[TRPJ0]");
    filaComandosInicializacao.enqueue("[TRPJ1]");
    filaComandosInicializacao.enqueue("[TRPJ2]");
    filaComandosInicializacao.enqueue("[TRPJ3]");
    filaComandosInicializacao.enqueue("[TRPJ4]");
    filaComandosInicializacao.enqueue("[TRPGR]");

    filaComandosInicializacao.enqueue("[JST]");

    filaComandosInicializacao.enqueue("[VELJ00016]");
    filaComandosInicializacao.enqueue("[VELJ10008]");
    filaComandosInicializacao.enqueue("[VELJ20008]");
    filaComandosInicializacao.enqueue("[VELJ30008]");
    filaComandosInicializacao.enqueue("[VELJ4]");
    filaComandosInicializacao.enqueue("[VELGR]");

    filaComandosInicializacao.enqueue("[ACLJ0]");
    filaComandosInicializacao.enqueue("[ACLJ1]");
    filaComandosInicializacao.enqueue("[ACLJ2]");
    filaComandosInicializacao.enqueue("[ACLJ3]");
    filaComandosInicializacao.enqueue("[ACLJ4]");
    filaComandosInicializacao.enqueue("[ACLGR]");
}




void MainWindow::configuracoesIniciais()
{
    //connect(serial, &QSerialPort::readyRead, this, &MainWindow::readData);
    tentativaConfig = 0;
    enviaComando("[ECH0]");
    timer->start(2000);
}

void MainWindow::configurarConversaoEntreMicrossegundosEAngulos(bool valoresDefault)
{    
    if(valoresDefault)
    {
        for(int i = 0; i < QTD_SERVOS; i++)
        {
            for(int j = 0; j < 4; j++)
            {
                setaValorItemTabela(ui->tabelaPosLimites, i, j, QString("%1").arg(tabelaPosLimitesDefault[i][j]));
                setaValorItemTabela(ui->tabelaPosLimitesGraus, i, j, QString("%1").arg(tabelaPosLimitesGrausDefault[i][j]));
            }

            if(tabelaPosLimitesGrausDefault[i][4] == 1)
                ui->tabelaPosLimitesGraus->item(i,4)->setCheckState(Qt::CheckState::Checked);
            else
                ui->tabelaPosLimitesGraus->item(i,4)->setCheckState(Qt::CheckState::Unchecked);

            lstSpnVel[i]->setValue(velTmpPulsoDefault[i]);
            lstSpnAcl[i]->setValue(aclTmpPulsoDefault[i]);
        }
    }

    int tempoPulsoNeutro[QTD_SERVOS];
    int tempoPulsoRepouso[QTD_SERVOS];
    bool propInv[QTD_SERVOS];

    for(int i = 0; i < QTD_SERVOS; i++)
    {                
        tempoPulsoMax[i] = ui->tabelaPosLimites->item(i,0)->text().toInt();
        tempoPulsoMin[i] = ui->tabelaPosLimites->item(i,1)->text().toInt();
        tempoPulsoNeutro[i] = ui->tabelaPosLimites->item(i,2)->text().toInt();
        tempoPulsoRepouso[i] = ui->tabelaPosLimites->item(i,3)->text().toInt();

        lstSlider[i]->setMaximum(tempoPulsoMax[i]);
        lstSlider[i]->setMinimum(tempoPulsoMin[i]);
        lstSlider[i]->setTickInterval(1);

        angMax[i] = ui->tabelaPosLimitesGraus->item(i,0)->text().toInt();
        angMin[i] = ui->tabelaPosLimitesGraus->item(i,1)->text().toInt();
        propInv[i] = (ui->tabelaPosLimitesGraus->item(i,4)->checkState() == Qt::CheckState::Checked);        
    }

    for (int i = 0; i < QTD_SERVOS; i++)
    {
        if(propInv[i])
        {
            coeffAng[i] = (angMin[i] - angMax[i])/(tempoPulsoMax[i] - tempoPulsoMin[i]);
            offsetAng[i] = angMin[i] - tempoPulsoMax[i] * coeffAng[i];
        }
        else
        {
            coeffAng[i] = (angMax[i] - angMin[i]) / (tempoPulsoMax[i] - tempoPulsoMin[i]);
            offsetAng[i] = angMax[i] - tempoPulsoMax[i] * coeffAng[i];
        }

        double anguloNeutro = coeffAng[i] * tempoPulsoNeutro[i] + offsetAng[i];
        anguloNeutro = round(anguloNeutro * DIV_CD_POSICAO) / DIV_CD_POSICAO;
        setaValorItemTabela(ui->tabelaPosLimitesGraus, i, 2, QString("%1").arg(anguloNeutro));
        double anguloRepouso = coeffAng[i] * tempoPulsoRepouso[i] + offsetAng[i];        
        anguloRepouso = round(anguloRepouso * DIV_CD_POSICAO) / DIV_CD_POSICAO;
        setaValorItemTabela(ui->tabelaPosLimitesGraus, i, 3, QString("%1").arg(anguloRepouso));

        if(ui->rdbReadyForPIC->isChecked())
        {
            qtdPosicoesTmpPulso[i] = (tempoPulsoMax[i] - tempoPulsoMin[i]);  // us

            incrementosAng[i] = (angMax[i] - angMin[i])/qtdPosicoesTmpPulso[i]; // Graus / us
            if(round(incrementosAng[i] * DIV_CD_POSICAO) / DIV_CD_POSICAO > 0)
                incrementosAng[i] = round(incrementosAng[i] * DIV_CD_POSICAO) / DIV_CD_POSICAO;
            else
                incrementosAng[i] = 0.001;
            incVelGrausPorSeg[i] = incrementosAng[i] / 0.25 * (10e-3);
            if(round(incVelGrausPorSeg[i] * DIV_CD_VELOCIDADE) / DIV_CD_VELOCIDADE > 0)
                incVelGrausPorSeg[i]  = round(incVelGrausPorSeg[i] * DIV_CD_VELOCIDADE) / DIV_CD_VELOCIDADE;
            else
                incVelGrausPorSeg[i] = 0.001;
            incAclGrausPorSegQuad[i] = incrementosAng[i] / 0.25 * (10e-3) * (80e-3);
            if(round(incAclGrausPorSegQuad[i] * DIV_CD_ACELERACAO) / DIV_CD_ACELERACAO > 0)
                incAclGrausPorSegQuad[i] = round(incAclGrausPorSegQuad[i] * DIV_CD_ACELERACAO) / DIV_CD_ACELERACAO;
            else
                incAclGrausPorSegQuad[i] = 0.001;

            lstSpnAlvoGraus[i]->setDecimals(CASAS_DECIMAIS_POSICAO);
            lstSpnAlvoGraus[i]->setSingleStep(incrementosAng[i]);
            lstSpnVelGrausPorSeg[i]->setDecimals(CASAS_DECIMAIS_VELOCIDADE);
            lstSpnVelGrausPorSeg[i]->setSingleStep(incVelGrausPorSeg[i]);
            lstSpnAclGrausPorSegQuad[i]->setDecimals(CASAS_DECIMAIS_ACELERACAO);
            lstSpnAclGrausPorSegQuad[i]->setSingleStep(incAclGrausPorSegQuad[i]);

            // TODO: Verificar se estes vetores serão usados nas velocidades e acelerações
            int velTmpPulso = lstSpnVel[i]->value();
            velGrausPorSeg[i] = velTmpPulso * incrementosAng[i] / 0.25 * (10e-3);  // (0.25us)/(10ms)
                            // (0.25us)/(10ms) * Graus/(us) / 0.25
                            // (0.25us)/(10ms) * Graus/(0.25us)
                            //        1/(10ms) * Graus
                            //     Graus /(10ms)
                            // Graus / (10e-3 s) * 10e-3
                            // Graus / s
            velGrausPorSeg[i] = round(velGrausPorSeg[i] * DIV_CD_VELOCIDADE) / DIV_CD_VELOCIDADE;
            lstSpnVelGrausPorSeg[i]->setValue(velGrausPorSeg[i]);

            int aclTmpPulso = lstSpnAcl[i]->value();
            aclGrausPorSegQuad[i] = aclTmpPulso * incrementosAng[i] / 0.25 * (10e-3) * (80e-3); // (0.25us)/(10ms)/(80ms)
            aclGrausPorSegQuad[i] = round(aclGrausPorSegQuad[i] * DIV_CD_ACELERACAO) / DIV_CD_ACELERACAO;
            lstSpnAclGrausPorSegQuad[i]->setValue(aclGrausPorSegQuad[i]);
        }
        else if(ui->rdbMiniMaestro24->isChecked())
        {
            qtdPosicoesTmpPulso[i] = (tempoPulsoMax[i] - tempoPulsoMin[i]) * 4; // 0.25us
            incrementosAng[i] = (angMax[i] - angMin[i])/qtdPosicoesTmpPulso[i]; // Graus / 0.25us
            if(round(incrementosAng[i] * DIV_CD_POSICAO) / DIV_CD_POSICAO > 0)
                incrementosAng[i] = round(incrementosAng[i] * DIV_CD_POSICAO) / DIV_CD_POSICAO;
            else
                incrementosAng[i] = 0.001;
            incVelGrausPorSeg[i] = incrementosAng[i] * (10e-3);
            if(round(incVelGrausPorSeg[i] * DIV_CD_VELOCIDADE) / DIV_CD_VELOCIDADE > 0)
                incVelGrausPorSeg[i]  = round(incVelGrausPorSeg[i] * DIV_CD_VELOCIDADE) / DIV_CD_VELOCIDADE;
            else
                incVelGrausPorSeg[i] = 0.001;
            incAclGrausPorSegQuad[i] = incrementosAng[i] * (10e-3) * (80e-3);
            if(round(incAclGrausPorSegQuad[i] * DIV_CD_ACELERACAO) / DIV_CD_ACELERACAO > 0)
                incAclGrausPorSegQuad[i] = round(incAclGrausPorSegQuad[i] * DIV_CD_ACELERACAO) / DIV_CD_ACELERACAO;
            else
                incAclGrausPorSegQuad[i] = 0.001;

            lstSpnAlvoGraus[i]->setSingleStep(incrementosAng[i]);
            lstSpnVelGrausPorSeg[i]->setSingleStep(incVelGrausPorSeg[i]);
            lstSpnAclGrausPorSegQuad[i]->setSingleStep(incAclGrausPorSegQuad[i]);

            // TODO: Verificar se estes vetores serão usados nas velocidades e acelerações
            int velTmpPulso = lstSpnVel[i]->value();
            velGrausPorSeg[i] = velTmpPulso * incrementosAng[i] * (10e-3);  // (0.25us)/(10ms)
                            // (0.25us)/(10ms) * Graus/(0.25us) * 10 * 10e-3
                            //      1 / (10ms) * Graus          * 10 * 10e-3
                            //       Graus / ((10e-3) * s) * (10e-3)
                            //       Graus / s
            velGrausPorSeg[i] = round(velGrausPorSeg[i] * DIV_CD_VELOCIDADE) / DIV_CD_VELOCIDADE;
            lstSpnVelGrausPorSeg[i]->setValue(velGrausPorSeg[i]);


            int aclTmpPulso = lstSpnAcl[i]->value();
            aclGrausPorSegQuad[i] = aclTmpPulso * incrementosAng[i] * (10e-3) * (80e-3); // (0.25us)/(10ms)/(80ms)
                            // (0.25us)/(10ms)/(80ms) * Graus/(0.25us)
                            // 1/(10ms)/(80ms) * Graus/1
                            // Graus/(10ms)/(80ms)
                            // Graus/((10ms)*(80ms))
                            // Graus / (10e-3 * s  * 80e-3 * s) * 10e-3 * 80e-3
                            // Graus / s^2
            aclGrausPorSegQuad[i] = round(aclGrausPorSegQuad[i] * DIV_CD_ACELERACAO) / DIV_CD_ACELERACAO;
            lstSpnAclGrausPorSegQuad[i]->setValue(aclGrausPorSegQuad[i]);
        }
    }
}

void MainWindow::timeoutConfig()
{
    if(tentativaConfig < 2)
    {
        tentativaConfig++;
        enviaComando("[ECH0]");
        showStatusMessage("Tentativa "+QString("%1").arg(tentativaConfig)+" de configuração.");
        timer->start();
    }
    else
    {
        tentativaConfig = 0;
        if(serial->isOpen())
            serial->close();
        habilitarComponentesConn(false);
        showStatusMessage("Não foi possível configurar o braço robô. Desconectado.");
    }
}

void MainWindow::habilitarComponentesConn(bool estadoHab)
{    
    ui->btAbrirPortaSerial->setEnabled(!estadoHab);
    ui->btFecharPortaSerial->setEnabled(estadoHab);
    habilitarComponentes(estadoHab);

    if(ui->rdbReadyForPIC->isChecked())
        habilitarComponentesReadyForPIC(estadoHab);
    else if(ui->rdbMiniMaestro24->isChecked())
        habilitarComponentesReadyForPIC(false);
    HabilitarComponentesComServosLigados();
    ui->rdbReadyForPIC->setEnabled(!estadoHab);
    ui->rdbMiniMaestro24->setEnabled(!estadoHab);
}

void MainWindow::habilitarComponentes(bool estadoHab)
{
    if (!estadoHab)
    {
        ui->btAbrirGarra->setEnabled(estadoHab);
        ui->btGarraSemiaberta->setEnabled(estadoHab);
        ui->btFecharGarra->setEnabled(estadoHab);
        ui->btGiroGarraMais90->setEnabled(estadoHab);
        ui->btGarraPosNeutra->setEnabled(estadoHab);
        ui->btGiroGarraMenos90->setEnabled(estadoHab);
        ui->btPosNeutraJST->setEnabled(estadoHab);
        ui->btPosNeutraCTZ->setEnabled(estadoHab);
    }
    ui->btPosicaoRepouso->setEnabled(estadoHab);
    ui->btDesligaServos->setEnabled(estadoHab);
    ui->btAdicionarComandoAASeqComandos->setEnabled(estadoHab);

    for(int i = 0; i < QTD_SERVOS; i++)
    {
        lstChkHab[i]->setEnabled(estadoHab);
        lstSlider[i]->setEnabled(estadoHab);
        bool ehNumeroEZero = false;

        lstEdtAtual[i]->text().toInt(&ehNumeroEZero);

        if(ehNumeroEZero)
        {
            ehNumeroEZero = (lstEdtAtual[i]->text().toInt() > 0);
        }

        lstSpnAlvo[i]->setEnabled(estadoHab && ehNumeroEZero);
        lstSpnVel[i]->setEnabled(estadoHab);
        lstSpnAcl[i]->setEnabled(estadoHab);

        lstSpnAlvoGraus[i]->setEnabled(estadoHab && ehNumeroEZero);
        lstSpnVelGrausPorSeg[i]->setEnabled(estadoHab);
        lstSpnAclGrausPorSegQuad[i]->setEnabled(estadoHab);
    }

    ui->spnPosXAlvo->setEnabled(estadoHab);
    ui->spnPosYAlvo->setEnabled(estadoHab);
    ui->spnPosZAlvo->setEnabled(estadoHab);
    ui->spnRxAlvo->setEnabled(estadoHab);
    ui->spnRyAlvo->setEnabled(estadoHab);
    ui->spnRzAlvo->setEnabled(estadoHab);
    ui->btCalcularXYZAlvo->setEnabled(estadoHab);
    ui->btCalcularAngulosAlvo->setEnabled(estadoHab);

    ui->btMover->setEnabled(estadoHab);
    ui->btMoverComVelEAcl->setEnabled(estadoHab);
    ui->chkEnviaComandoImediato->setEnabled(estadoHab);

    ui->tabelaPosLimites->setEnabled(estadoHab);
    ui->tabelaPosLimitesGraus->setEnabled(estadoHab);

    ui->listSequenciaComandos->setEnabled(estadoHab);
    ui->btNovaSequencia->setEnabled(estadoHab);
    ui->btAdicionarComando->setEnabled(estadoHab);
    ui->btAdicionarPosCorrente->setEnabled(estadoHab);
    ui->chkComVelocidades->setEnabled(estadoHab);
    ui->chkComAceleracoes->setEnabled(estadoHab);
    ui->btEditarComando->setEnabled(estadoHab);
    ui->btRemoverComando->setEnabled(estadoHab);
    ui->btCarregarSeqComandos->setEnabled(estadoHab);
    ui->btSalvarSeqComandos->setEnabled(estadoHab);
    ui->btExecutarSeqComandos->setEnabled(estadoHab);
    ui->btContinuarSeqComandos->setEnabled(estadoHab);
    ui->btExecutarLoopSeqComandos->setEnabled(estadoHab);
    ui->btContinuarLoopSeqComandos->setEnabled(estadoHab);
    ui->btPararSeqComandos->setEnabled(estadoHab);
    ui->btComandoUp->setEnabled(estadoHab);
    ui->btComandoDown->setEnabled(estadoHab);
}

void MainWindow::habilitarComponentesReadyForPIC(bool estadoHab)
{
    for(int i = 0; i < 8; i++)
    {
        lstChkLeds[i]->setEnabled(estadoHab);
    }
    ui->chkAtivarLeds->setEnabled(estadoHab);
    ui->btAtualizarLeds->setEnabled(estadoHab);
    ui->btAdicionarLedsSeqComandos->setEnabled(estadoHab);

    ui->btObterPosLimites->setEnabled(estadoHab);
    ui->btConfigurarPosLimites->setEnabled(estadoHab);
    ui->rdbSemFeedback->setEnabled(estadoHab);
    ui->rdbPosicoesCorrentesDasJuntas->setEnabled(estadoHab);
    ui->rdbSinalDeMovimento->setEnabled(estadoHab);
    ui->chkComandosBloqueantesDeMovimento->setEnabled(estadoHab);
    ui->btResetarPlacaControle->setEnabled(estadoHab);
    ui->btResetarPlacaServos->setEnabled(estadoHab);
}

void MainWindow::HabilitarComponentesComServosLigados()
{    
    bool estadoHab = serial->isOpen() && ui->btPosicaoRepouso->isEnabled();
    for(int i = 0; (i < QTD_SERVOS - 1) && estadoHab; i++)
    {
        bool conversaoOk = false;
        float posAtual = lstEdtAtual[i]->text().toFloat(&conversaoOk);
        estadoHab = (conversaoOk && (posAtual > 0));
    }

    ui->btAbrirGarra->setEnabled(estadoHab);
    ui->btGarraSemiaberta->setEnabled(estadoHab);
    ui->btFecharGarra->setEnabled(estadoHab);
    ui->btGiroGarraMais90->setEnabled(estadoHab);
    ui->btGarraPosNeutra->setEnabled(estadoHab);
    ui->btGiroGarraMenos90->setEnabled(estadoHab);
    ui->btPosNeutraJST->setEnabled(estadoHab);
    ui->btPosNeutraCTZ->setEnabled(estadoHab);
}

MainWindow::~MainWindow()
{
    if(serial->isOpen())
        serial->close();

    delete montagemDeComandosDialog;
    delete ui;
}

void MainWindow::showStatusMessage(const QString &message)
{
    //statusBar()->setStatusTip(message);
    statusBar()->showMessage(message);
}



/* NOTE: ***** Porta Serial ***** */

void MainWindow::abrirPortaSerial()
{
    QString porta = ui->cmbPortaSerial->currentData().toString();

    qint32 baudRate;
    if (ui->cmbBaudRate->currentIndex() == 4) {
        baudRate = ui->cmbBaudRate->currentText().toInt();
    } else {
        baudRate = static_cast<QSerialPort::BaudRate>(
                    ui->cmbBaudRate->itemData(ui->cmbBaudRate->currentIndex()).toInt());
    }

    QSerialPort::DataBits dataBits = static_cast<QSerialPort::DataBits>(
                ui->cmbBitsDeDados->itemData(ui->cmbBitsDeDados->currentIndex()).toInt());;

    QSerialPort::Parity parity = static_cast<QSerialPort::Parity>(
                    ui->cmbParidade->itemData(ui->cmbParidade->currentIndex()).toInt());

    QSerialPort::StopBits stopBits = static_cast<QSerialPort::StopBits>(
                ui->cmbBitDeParada->itemData(ui->cmbBitDeParada->currentIndex()).toInt());

    serial->setPortName(porta);
    serial->setBaudRate(baudRate);
    serial->setDataBits(dataBits);
    serial->setParity(parity);
    serial->setStopBits(stopBits);
    serial->setFlowControl(QSerialPort::NoFlowControl);

    if (serial->open(QIODevice::ReadWrite))
    {
        QString strStatusMessage = "Conectado à porta "+porta+".";
        showStatusMessage(strStatusMessage);

        ui->btAbrirPortaSerial->setEnabled(false);
        console->setEnabled(true);
        inicializando = true;
        configuracoesIniciais();

        //habilitarComponentesConn(true);        
    }
    else
    {
        QMessageBox::critical(this, tr("Erro"), serial->errorString());
        showStatusMessage("Erro ao abrir a porta "+porta+".");
        habilitarComponentesConn(false);
    }
}

void MainWindow::fecharPortaSerial()
{
    if (serial->isOpen())
        serial->close();
    console->setEnabled(false);

    habilitarComponentesConn(false);

    showStatusMessage("Desconectado");
}

void MainWindow::writeData(const QByteArray &data)
{
    serial->write(data);
}

void MainWindow::readData()
{
    QByteArray data = serial->readAll();
    if(ui->rdbReadyForPIC->isChecked())
    {        
        console->putData(data, false);
        recebeCaracteresDeResposta(data);
    }
    else
    {
        // TODO: Aba Terminal: Tratamento do console para comandos diretos da Mini Maestro 24
        // TODO: Geral: Tratamento da recepção de bytes de resposta da Mini Maestro 24
    }

}

void MainWindow::handleError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError) {
        QMessageBox::critical(this, tr("Erro Crítico"), serial->errorString());
        fecharPortaSerial();
    }
}

void MainWindow::preencheCombosPortaSerial()
{
    ui->cmbPortaSerial->clear();
    const auto infos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &serialInfo : infos) {
        ui->cmbPortaSerial->addItem(serialInfo.portName(), serialInfo.portName());
    }

    ui->cmbBaudRate->clear();
    ui->cmbBaudRate->addItem(QStringLiteral("9600"), QSerialPort::Baud9600);
    ui->cmbBaudRate->addItem(QStringLiteral("19200"), QSerialPort::Baud19200);
    ui->cmbBaudRate->addItem(QStringLiteral("38400"), QSerialPort::Baud38400);
    ui->cmbBaudRate->addItem(QStringLiteral("115200"), QSerialPort::Baud115200);

    ui->cmbBitsDeDados->clear();
    ui->cmbBitsDeDados->addItem(QStringLiteral("5"), QSerialPort::Data5);
    ui->cmbBitsDeDados->addItem(QStringLiteral("6"), QSerialPort::Data6);
    ui->cmbBitsDeDados->addItem(QStringLiteral("7"), QSerialPort::Data7);
    ui->cmbBitsDeDados->addItem(QStringLiteral("8"), QSerialPort::Data8);
    ui->cmbBitsDeDados->setCurrentIndex(3);

    ui->cmbParidade->clear();
    ui->cmbParidade->addItem(tr("None"), QSerialPort::NoParity);
    ui->cmbParidade->addItem(tr("Even"), QSerialPort::EvenParity);
    ui->cmbParidade->addItem(tr("Odd"), QSerialPort::OddParity);
    ui->cmbParidade->addItem(tr("Mark"), QSerialPort::MarkParity);
    ui->cmbParidade->addItem(tr("Space"), QSerialPort::SpaceParity);

    ui->cmbBitDeParada->clear();
    ui->cmbBitDeParada->addItem(QStringLiteral("1"), QSerialPort::OneStop);
#ifdef Q_OS_WIN
    ui->cmbBitDeParada->addItem(tr("1.5"), QSerialPort::OneAndHalfStop);
#endif
    ui->cmbBitDeParada->addItem(QStringLiteral("2"), QSerialPort::TwoStop);
}


/* NOTE: ***** Funções de conversão de posição, velocidade e aceleração ***** */
double MainWindow::converteMicrossegundosParaGraus(int idxJunta, int posicaoMicrossegundos)
{
    double a, b, x, y;

    a = coeffAng[idxJunta];
    b = offsetAng[idxJunta];
    x = posicaoMicrossegundos;

    y = a * x + b;

    return round(y*DIV_CD_POSICAO)/DIV_CD_POSICAO;
}

int MainWindow::converteGrausParaMicrossegundos(int idxJunta, double posicaoGraus)
{
    double a, b, x, y;

    y = posicaoGraus;
    a = coeffAng[idxJunta];
    b = offsetAng[idxJunta];

    x = (y - b)/a;

    return qRound(x);
}

double MainWindow::converteVelTmpPulsoParaGrausPorSeg(int idxJunta, int velTmpPulso)
{
    double velGrausPorSeg;

    if(ui->rdbReadyForPIC->isChecked())
        velGrausPorSeg = velTmpPulso * incrementosAng[idxJunta] / 0.25 * 10e-3;
    else
        velGrausPorSeg = velTmpPulso * incrementosAng[idxJunta] * 10e-3;

    return round(velGrausPorSeg * DIV_CD_VELOCIDADE) / DIV_CD_VELOCIDADE;

}

int MainWindow::converteVelGrausPorSegParaTmpPulso(int idxJunta, double velGrausPorSeg)
{
    double velTmpPulso;

    if(ui->rdbReadyForPIC->isChecked())
        velTmpPulso = velGrausPorSeg / (incrementosAng[idxJunta] / 0.25 * 10e-3);
    else
        velTmpPulso = velGrausPorSeg / (incrementosAng[idxJunta] * 10e-3);

    return qRound(velTmpPulso);
}

double MainWindow::converteAclTmpPulsoParaGrausPorSegQuad(int idxJunta, int aclTmpPulso)
{
    double aclGrausPorSegQuad;

    if(ui->rdbReadyForPIC->isChecked())
        aclGrausPorSegQuad = aclTmpPulso * incrementosAng[idxJunta] / 0.25 * 10e-3 * 80e-3;
    else
        aclGrausPorSegQuad = aclTmpPulso * incrementosAng[idxJunta] * 10e-3 * 80e-3;

    return round(aclGrausPorSegQuad * DIV_CD_ACELERACAO) / DIV_CD_ACELERACAO;
}

int MainWindow::converteAclGrausPorSegQuadParaTmpPulso(int idxJunta, double aclGrausPorSegQuad)
{
    double aclTmpPulso;
    if(ui->rdbReadyForPIC->isChecked())
        aclTmpPulso = aclGrausPorSegQuad / (incrementosAng[idxJunta] / 0.25 * 10e-3 * 80e-3);
    else
        aclTmpPulso = aclGrausPorSegQuad / (incrementosAng[idxJunta] * 10e-3 * 80e-3);

    return qRound(aclTmpPulso);
}

void MainWindow::converteSpnAlvoParaGraus(int idxJunta, int posicaoAlvo)
{
    if(posicaoAlvo > 0)
    {
        lstChkHab[idxJunta]->setChecked(true);
        lstSpnAlvo[idxJunta]->setEnabled(true);
        lstSpnAlvoGraus[idxJunta]->setEnabled(true);

        lstSlider[idxJunta]->setEnabled(true);
        if(posicaoAlvo != lstSlider[idxJunta]->value())
            lstSlider[idxJunta]->setValue(posicaoAlvo);

        double angulo = converteMicrossegundosParaGraus(idxJunta, posicaoAlvo);
        double anguloAtual = lstSpnAlvoGraus[idxJunta]->value();        

        // Esta verificação é feita para que não se ative o evento valueChanged desnecessariamente
        if(angulo != anguloAtual)
            lstSpnAlvoGraus[idxJunta]->setValue(angulo);
    }
    else
    {
        lstChkHab[idxJunta]->setChecked(false);
        lstSpnAlvo[idxJunta]->setEnabled(false);
        lstSpnAlvoGraus[idxJunta]->setEnabled(false);
    }
}

void MainWindow::converteSpnVelParaGrausPorSeg(int idxJunta, int velocidade)
{
    double velocidadeAngular = converteVelTmpPulsoParaGrausPorSeg(idxJunta, velocidade);
    double velocidadeAngularAtual = lstSpnVelGrausPorSeg[idxJunta]->value();

    // Esta verificação é feita para que não se ative o evento valueChanged desnecessariamente
    if(velocidadeAngular != velocidadeAngularAtual)
        lstSpnVelGrausPorSeg[idxJunta]->setValue(velocidadeAngular);
}

void MainWindow::converteSpnAclParaGrausPorSegQuad(int idxJunta, int aceleracao)
{
    double aceleracaoAngular = converteAclTmpPulsoParaGrausPorSegQuad(idxJunta, aceleracao);
    double aceleracaoAngularAtual = lstSpnAclGrausPorSegQuad[idxJunta]->value();

    // Esta verificação é feita para que não se ative o evento valueChanged desnecessariamente
    if(aceleracaoAngular != aceleracaoAngularAtual)
        lstSpnAclGrausPorSegQuad[idxJunta]->setValue(aceleracaoAngular);
}

void MainWindow::converteSpnAlvoGrausParaTmpPulso(int idxJunta, double posicaoGraus)
{
    int posicaoMicrossegundos = converteGrausParaMicrossegundos(idxJunta, posicaoGraus);
    int posicaoMicrossegundosAtual = lstSpnAlvo[idxJunta]->value();

    // Esta verificação é feita para que não se ative o evento valueChanged desnecessariamente
    if(posicaoMicrossegundos != posicaoMicrossegundosAtual)
        lstSpnAlvo[idxJunta]->setValue(posicaoMicrossegundos);
}

void MainWindow::converteSpnVelGrausPorSegParaTmpPulso(int idxJunta, double velGrausPorSeg)
{
    int velocidadeTmpPulso = converteVelGrausPorSegParaTmpPulso(idxJunta, velGrausPorSeg);
    int velocidadeTmpPulsoAtual = lstSpnVel[idxJunta]->value();

    // Esta verificação é feita para que não se ative o evento valueChanged desnecessariamente
    if(velocidadeTmpPulso != velocidadeTmpPulsoAtual)
        lstSpnVel[idxJunta]->setValue(velocidadeTmpPulso);
}

void MainWindow::converteSpnAclGrausPorSegQuadParaTmpPulso(int idxJunta, double aclGrausPorSegQuad)
{
    int aceleracaoTmpPulso = converteAclGrausPorSegQuadParaTmpPulso(idxJunta, aclGrausPorSegQuad);
    int aceleracaoTmpPulsoAtual = lstSpnAcl[idxJunta]->value();

    // Esta verificação é feita para que não se ative o evento valueChanged desnecessariamente
    if(aceleracaoTmpPulso != aceleracaoTmpPulsoAtual)
        lstSpnAcl[idxJunta]->setValue(aceleracaoTmpPulso);
}



/* NOTE: ***** Resposta dos comandos ***** */

void MainWindow::recebeCaracteresDeResposta(QByteArray data)
{
    char rChar;

    for(int i = 0; i < data.length(); i++)
    {
        rChar = data.at(i);

        if(!flagProtocolo && rChar == '[')
        {
            bufferProto = "";
            bufferProto += rChar;
            flagProtocolo = true;
        }
        else if(flagProtocolo)
        {
            if(rChar == '\010') // Backspace
            {
                if(bufferProto.length() > 0)
                    bufferProto.chop(1);
                else
                    flagProtocolo = false;
            }
            else if(rChar == '\r')
            {
                // ignora o \r
            }
            else if(rChar == '[')
            {
                bufferProto = "";
                bufferProto += rChar;
                flagProtocolo = true;
            }
            else if(rChar == ']')
            {
                bufferProto += rChar;
                flagProtocolo = false;
                bufferPronto = true;
            }
            else
                bufferProto += rChar;
        }

        if(bufferPronto)
        {
            bufferPronto = false;
            filaBufferEntrada.enqueue(bufferProto);

            decodificaResposta();
        }
    }
}

void MainWindow::decodificaResposta()
{
    QString resposta = filaBufferEntrada.dequeue();
    int tamanhoResposta = resposta.length();
    bool ehRespostaFinal = false;
    bool ehRespostaConfigPosLimite = false;
    bool ehRespostaCTZ = false;
    bool ehRespostaMoverComVelAcl = false;

    if(tamanhoResposta == 35 && (resposta.contains("MOV") || resposta.contains("JST") || resposta.contains("RPS") || resposta.contains("IN1")) )
    {
        QString strValor;
        int valor;
        double graus;

        for(int i = 0; i < QTD_SERVOS; i++)
        {
            strValor = resposta.mid(5*(i+1), 4);
            valor = strValor.toInt();

            lstEdtAtual[i]->setText(strValor.setNum(valor));

            if(valor > 0)
            {
                graus = converteMicrossegundosParaGraus(i, valor);
                lstEdtAtualGraus[i]->setText(QString("%L1").arg(graus, 0, 'f', CASAS_DECIMAIS_POSICAO));
                // TODO: Aba Posições das Juntas: incluir widgets que plotem no slider a posição atual da junta
            }
        }

        //if(resposta.contains("JST") || resposta.contains("RPS") || resposta.contains("IN1"))
        if(!resposta.contains("MOV"))
        {
            for(int i = 0; i < QTD_SERVOS; i++)
            {
                valor = resposta.mid(5*(i+1), 4).toInt();

                if(valor > 0)
                {
                    if(valor != lstSpnAlvo[i]->value())
                    {
                        if(countAbaPosicoesValueChanged <= 0)
                        {
                            countAbaPosicoesValueChanged = -1;
                            lstSpnAlvo[i]->setValue(valor);
                        }
                        else
                        {
                            countAbaPosicoesValueChanged--;
                        }
                    }

                    if(!this->lstSpnAlvo[i]->isEnabled() || !this->lstSpnAlvoGraus[i]->isEnabled())
                    {                        
                        habilitaCamposAbaPosicaoAlvoJunta(0, i, true);
                        habilitaCamposAbaPosicaoAlvoJunta(1, i, true);

                        this->lstChkHab[i]->setChecked(true);
                    }
                }
                else
                {
                    lstChkHab[i]->setChecked(false);
                    lstSpnAlvo[i]->setEnabled(false);                    
                    lstSpnAlvoGraus[i]->setEnabled(false);
                }

                // TODO: Cinemática direta
            }
            HabilitarComponentesComServosLigados();

            ehRespostaFinal = true;
            ehRespostaMoverComVelAcl = resposta.contains("JST");
        }
    }
    else if(tamanhoResposta == 11)
    {
        if(resposta.contains("MOV") || resposta.contains("CTZ") || resposta.contains("IN1"))
        {
            QString junta = resposta.mid(4,2);
            QString strValor = resposta.mid(6,4);

            int i;
            int valor;

            if(junta.contains("J"))
                i = junta.at(1).toLatin1() - '0';
            else
                i = 5;

            valor = strValor.toInt();

            lstEdtAtual[i]->setText(strValor.setNum(valor));

            if(valor > 0)
            {
                double graus = converteMicrossegundosParaGraus(i, valor);
                lstEdtAtualGraus[i]->setText(QString("%L1").arg(graus, 0, 'f', CASAS_DECIMAIS_POSICAO));
            }

            //if(resposta.contains("CTZ") || resposta.contains("IN1"))
            if(!resposta.contains("MOV"))
            {
                if(valor > 0)
                {
                    if(valor != lstSpnAlvo[i]->value())
                    {
                        if(countAbaPosicoesValueChanged <= 0)
                        {
                            countAbaPosicoesValueChanged = -1;
                            lstSpnAlvo[i]->setValue(valor);
                        }
                        else
                        {
                            countAbaPosicoesValueChanged--;
                        }
                    }

                    if(!this->lstSpnAlvo[i]->isEnabled() || !this->lstSpnAlvoGraus[i]->isEnabled())
                    {
                        this->lstChkHab[i]->setChecked(true);
                    }
                }
                else
                {
                    lstChkHab[i]->setChecked(false);
                    lstSpnAlvo[i]->setEnabled(false);
                    lstSpnAlvoGraus[i]->setEnabled(false);
                }

                HabilitarComponentesComServosLigados();
                ehRespostaFinal = true;
                ehRespostaCTZ = resposta.contains("CTZ");
            }

        }
        else if(resposta.contains("VEL"))
        {
            setarVelOuAclResposta(resposta, lstSpnVel);            
            ehRespostaFinal = true;
            ehRespostaMoverComVelAcl = true;
        }
        else if(resposta.contains("ACL"))
        {
            setarVelOuAclResposta(resposta, lstSpnAcl);
            ehRespostaFinal = true;
            ehRespostaMoverComVelAcl = true;
        }
        else if(resposta.contains("TMX") || resposta.contains("TMN") || resposta.contains("T90") || resposta.contains("TRP"))
        {
            setarValorPosLimiteResposta(resposta);
            ehRespostaConfigPosLimite = true;
        }
    }
    else if(resposta.contains("[PRONTO]"))
    {
        inicializando = true;
        enviaComando("[ECH]");
    }
    else if(resposta.contains("ECH"))
    {
        timer->stop();

        if(tamanhoResposta == 5)
        {
            // Podemos deduzir que, se o [ECH] está sendo ecoado, o eco de caracteres está ativo no braço robô
            ecoCaracteresAtivado = true;
        }
        else if(tamanhoResposta == 6)
        {
            if (resposta.at(4) == '1')
            {
                if(inicializando)
                {
                    // Neste momento, o mesmo comando enviado será ecoado na entrada.
                    enviaComando("[ECH0]");
                    ecoEch0 = true; // Sinaliza que virá eco do [ECH0] na entrada
                }
                else if(ecoCaracteresAtivado)
                {
                    // Ignora o [ECH1]
                }
                else
                {
                    ecoCaracteresAtivado = true;
                }
            }
            else if(!ecoEch0 && resposta.at(4) == '0') // Se for resposta
            {
                // Se a resposta for [ECH0], e estiver inicializando, monta a fila de comandos de inicialização.
                if(inicializando)
                    alimentarFilaDeComandosDeInicializacao();

                ecoCaracteresAtivado = false;
            }
            else if (ecoEch0) // Se for eco do comando enviado
            {
                ecoEch0 = false; // Ignora o eco do comando e sinaliza que a próxima resposta é a resposta do comando ECH.
            }
        }
    }
    else if(resposta.contains("FRS"))
    {
        QChar valorFRS = resposta.at(4);

        ui->rdbSemFeedback->setChecked(valorFRS == '0');
        ui->rdbPosicoesCorrentesDasJuntas->setChecked(valorFRS == '1');
        ui->rdbSinalDeMovimento->setChecked(valorFRS == '2');
    }
    else if(resposta.contains("CSB"))
    {
        ui->chkComandosBloqueantesDeMovimento->setChecked(resposta.at(4) == '1');
    }
    else if(resposta.length() == 8 && (resposta.contains("GA") || resposta.contains("GF")))
    {
        QString valor = resposta.mid(3,4);
        valor.setNum(valor.toInt());

        ui->edtGRAtual->setText(valor);
        double graus = converteMicrossegundosParaGraus(5, valor.toInt());
        ui->edtGRAtualGraus->setText(QString("%L1").arg(graus, 0, 'f', CASAS_DECIMAIS_POSICAO));

        ui->spnGRAlvo->setValue(valor.toInt());

        if(!ui->spnGRAlvo->isEnabled())
        {
            ui->spnGRAlvo->setEnabled(true);
            ui->chkGR->setChecked(true);
            ui->spnGRAlvoGraus->setEnabled(true);            
        }

        ehRespostaFinal = true;
    }
    else if(resposta.contains("RST"))
    {
        //habilitarComponentesConn(false);
        habilitarComponentes(false);

        if(ui->rdbReadyForPIC->isChecked())
            habilitarComponentesReadyForPIC(false);
        else if(ui->rdbMiniMaestro24->isChecked())
            habilitarComponentesReadyForPIC(false);

    }
    else if(resposta.contains("LED"))
    {
        for(int i = 0; i < 8; i++)
        {
            lstChkLeds[i]->setChecked(resposta.at(11-i) == '1');
        }

        ui->edtComandoLEDAcionado->setText(resposta);

        ehRespostaFinal = true;
    }


    if(filaComandosInicializacao.count() > 0)
    {
        if(resposta.contains("TRPGR"))
            configurarConversaoEntreMicrossegundosEAngulos();

        enviaComando(filaComandosInicializacao.dequeue());
    }
    else if(!(resposta.contains("ECH") || resposta.contains("PRONTO")))
    {
        if (inicializando && serial->isOpen())
        {
            habilitarComponentesConn(true);            
        }
        inicializando = false;

        if(filaComandosPosicaoNeutraCTZ.count() > 0 && ehRespostaCTZ)
        {
            parser(filaComandosPosicaoNeutraCTZ.dequeue());
        }
        else if(filaComandosMoverComVelAcl.count() > 0 && ehRespostaMoverComVelAcl)
        {
            parser(filaComandosMoverComVelAcl.dequeue());
        }
        else if(ehRespostaConfigPosLimite)
        {
            if(filaConfigPosLimites.count() > 0)
                enviaComando(filaConfigPosLimites.dequeue());
            else
                configurarConversaoEntreMicrossegundosEAngulos();
        }
        else if(seqEmExecucao && ehRespostaFinal)
        {            
            executaComandoDaSequencia();
        }

    }
}

void MainWindow::setarValorPosLimiteResposta(QString resposta)
{
    QStringList listaComandosPosLimite;

    listaComandosPosLimite.append("TMX");
    listaComandosPosLimite.append("TMN");
    listaComandosPosLimite.append("T90");
    listaComandosPosLimite.append("TRP");

    QString strComando = resposta.mid(1,3);
    QString strJunta = resposta.mid(4,2);
    QString strValor = resposta.mid(6,4);

    int idxComando = listaComandosPosLimite.indexOf(strComando);
    int idxJunta = -1;

    if(strJunta.at(0) == 'J')
    {
        idxJunta = strJunta.at(1).toLatin1() - '0';
    }
    else if(strJunta.at(0) == 'G')
    {
        idxJunta = 5;
    }

    if(idxComando != -1 && idxJunta != -1)
    {
        int valor = strValor.toInt();
        strValor.setNum(valor);

        setaValorItemTabela(ui->tabelaPosLimites, idxJunta, idxComando, strValor);
    }
}

void MainWindow::setaValorItemTabela(QTableWidget *tableWidget, int idxLinha, int idxColuna, QString strValor)
{
    if(tableWidget->item(idxLinha, idxColuna) == 0)
    {
        QTableWidgetItem *item = new QTableWidgetItem();
        item->setText(strValor);
        tableWidget->setItem(idxLinha, idxColuna, item);
    }
    else
    {
        tableWidget->item(idxLinha, idxColuna)->setText(strValor);
    }
}

void MainWindow::setarVelOuAclResposta(QString resposta, QList<QSpinBox *> listaSpinBox)
{
    QString strJunta = resposta.mid(4,2);
    QString strValor = resposta.mid(6,4);

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
        int valor = strValor.toInt();
        if(valor != listaSpinBox[idxJunta]->value())
        {
            if(countAbaPosicoesValueChanged <= 0)
            {
                countAbaPosicoesValueChanged = -1;
                listaSpinBox[idxJunta]->setValue(valor);
            }
            else
                countAbaPosicoesValueChanged--;
        }
    }
}



/* NOTE: ***** Comandos ***** */

void MainWindow::enviaComando(QString comando)
{
    if(ui->chkEcoLocal->isChecked())
    {
        console->putData(comando.toLocal8Bit(), true);
    }
    this->writeData(comando.toLocal8Bit());
}

void MainWindow::executaComandoDaSequencia()
{
    // BUG: Com a sequência em execução, se clicar em uma linha diferente do comando que está sendo executado, a linha clicada é executada.
    int index_selected = ui->listSequenciaComandos->currentRow();

    if(index_selected + 1 < ui->listSequenciaComandos->count())
    {
        ui->listSequenciaComandos->setCurrentRow(++index_selected);

        while (!parser(ui->listSequenciaComandos->currentItem()->text()) &&
                (index_selected + 1 < ui->listSequenciaComandos->count()) )
        {
            ui->listSequenciaComandos->setCurrentRow(++index_selected);
        }
    }
    else
    {
        if(emLoop)
        {
            int index_selected = 0;
            ui->listSequenciaComandos->setCurrentRow(index_selected);

            while (!parser(ui->listSequenciaComandos->currentItem()->text()) &&
                    (index_selected + 1 < ui->listSequenciaComandos->count()) )
            {
                ui->listSequenciaComandos->setCurrentRow(++index_selected);
            }
        }
        else
        {
            seqEmExecucao = false;

            if(ui->listSequenciaComandos->count() == 0)
                ui->lblStatusSeqComandos->setText(STATUS_SEQCOM_PARADA_VAZIA);
            else
            {
                ui->lblStatusSeqComandos->setText(STATUS_SEQCOM_PARADA_NAO_VAZIA);
                ui->lblStatusSeqComandos->setFocus();
            }

        }
    }
}

void MainWindow::abrirGarra()
{    
    parser("[GA]");
}

void MainWindow::garraSemiaberta()
{    
    parser("[CTZGR]");
}

void MainWindow::fecharGarra()
{    
    parser("[GF]");
}

void MainWindow::giroGarraMais90()
{
    int valor = ui->tabelaPosLimites->item(4,0)->text().toInt();    

    QString comandoGiroGarra = QString("[JSTE%1]").arg(valor, 4, 10, QChar('0'));

    parser(comandoGiroGarra);
}

void MainWindow::garraPosNeutra()
{
    parser("[CTZJ4]");
}

void MainWindow::giroGarraMenos90()
{
    int valor = ui->tabelaPosLimites->item(4,1)->text().toInt();    

    QString comandoGiroGarra = QString("[JSTE%1]").arg(valor, 4, 10, QChar('0'));

    parser(comandoGiroGarra);
}

void MainWindow::posicaoDeRepouso()
{    
    parser("[RPS]");
}

void MainWindow::posicaoNeutraJST()
{
    QString comandoJST = "[JST";
    for(int i = 0; i < QTD_SERVOS; i++)
    {
        int valor = ui->tabelaPosLimites->item(i, 2)->text().toInt();
        comandoJST += QString("%1%2").arg(idJST[i]).arg(valor, 4, 10, QChar('0'));
    }

    comandoJST += "]";
    parser(comandoJST);
}

void MainWindow::posicaoNeutraCTZ()
{    
    filaComandosPosicaoNeutraCTZ.clear();

    filaComandosPosicaoNeutraCTZ.enqueue("[CTZJ0]");
    filaComandosPosicaoNeutraCTZ.enqueue("[CTZJ1]");
    filaComandosPosicaoNeutraCTZ.enqueue("[CTZJ2]");
    filaComandosPosicaoNeutraCTZ.enqueue("[CTZJ3]");
    filaComandosPosicaoNeutraCTZ.enqueue("[CTZJ4]");
    filaComandosPosicaoNeutraCTZ.enqueue("[CTZGR]");

    parser(filaComandosPosicaoNeutraCTZ.dequeue());
}

void MainWindow::desligaServos()
{    
    parser("[JSTA0000B0000C0000D0000E0000G0000]");
}

void MainWindow::comandoJST()
{
    QString comandoJST = "[JST";

    for(int i = 0; i < QTD_SERVOS; i++)
    {
        int valor = lstSpnAlvo[i]->value();
        comandoJST += idJST[i] + QString("%1").arg(valor, 4, 10, QChar('0'));
    }

    comandoJST += "]";

    enviaComando(comandoJST);
}

void MainWindow::comandoLED()
{
    QString cmdLED = "[LED";

    for (int i = 7; i >= 0; i--)
    {
        cmdLED += lstChkLeds[i]->isChecked() ? "1" : "0";
    }
    cmdLED += "]";

    enviaComando(cmdLED);
}

void MainWindow::comandoLEDSemParametros()
{
    enviaComando("[LED]");
}


/* NOTE: ***** Aba Comandos ***** */

void MainWindow::on_btAbrirGarra_clicked()
{
    on_btPararSeqComandos_clicked();
    ui->chkEnviaComandoImediato->setChecked(false);

    ui->lblComandoAcionado->setText(ui->btAbrirGarra->text());
    ui->listaUltimoComandoAcionado->clear();
    ui->listaUltimoComandoAcionado->addItem("[GA]");

    if(ui->rdbReadyForPIC->isChecked())
        abrirGarra();
    else if(ui->rdbMiniMaestro24->isChecked())
    {
        // TODO: Aba Comandos: botão abrir garra com a mini maestro 24
    }
}

void MainWindow::on_btGarraSemiaberta_clicked()
{
    on_btPararSeqComandos_clicked();
    ui->chkEnviaComandoImediato->setChecked(false);

    ui->lblComandoAcionado->setText(ui->btGarraSemiaberta->text());
    ui->listaUltimoComandoAcionado->clear();
    ui->listaUltimoComandoAcionado->addItem("[CTZGR]");

    if(ui->rdbReadyForPIC->isChecked())
        garraSemiaberta();
    else if(ui->rdbMiniMaestro24->isChecked())
    {
        // TODO: Aba Comandos: botão garra semiaberta com a mini maestro 24
    }
}

void MainWindow::on_btFecharGarra_clicked()
{
    on_btPararSeqComandos_clicked();
    ui->chkEnviaComandoImediato->setChecked(false);

    ui->lblComandoAcionado->setText(ui->btFecharGarra->text());
    ui->listaUltimoComandoAcionado->clear();
    ui->listaUltimoComandoAcionado->addItem("[GF]");

    if(ui->rdbReadyForPIC->isChecked())
        fecharGarra();
    else if(ui->rdbMiniMaestro24->isChecked())
    {
        // TODO: Aba Comandos: botão fechar garra da mini maestro 24
    }
}

void MainWindow::on_btGiroGarraMais90_clicked()
{
    on_btPararSeqComandos_clicked();
    ui->chkEnviaComandoImediato->setChecked(false);

    ui->lblComandoAcionado->setText(ui->btGiroGarraMais90->text());
    int valor = ui->tabelaPosLimites->item(4,0)->text().toInt();
    QString comandoGiroGarra = QString("[JSTE%1]").arg(valor, 4, 10, QChar('0'));

    ui->listaUltimoComandoAcionado->clear();
    ui->listaUltimoComandoAcionado->addItem(comandoGiroGarra);

    if(ui->rdbReadyForPIC->isChecked())
        giroGarraMais90();
    else if(ui->rdbMiniMaestro24->isChecked())
    {
        // TODO: Aba Comandos: botão giro garra +90º da mini maestro 24
    }
}

void MainWindow::on_btGarraPosNeutra_clicked()
{
    on_btPararSeqComandos_clicked();
    ui->chkEnviaComandoImediato->setChecked(false);

    ui->lblComandoAcionado->setText(ui->btGarraPosNeutra->text());
    ui->listaUltimoComandoAcionado->clear();
    ui->listaUltimoComandoAcionado->addItem("[CTZJ4]");

    if(ui->rdbReadyForPIC->isChecked())
        garraPosNeutra();
    else if(ui->rdbMiniMaestro24->isChecked())
    {
        // TODO: Aba Comandos: botão garra pos. neutra da mini maestro 24
    }
}

void MainWindow::on_btGiroGarraMenos90_clicked()
{
    on_btPararSeqComandos_clicked();
    ui->chkEnviaComandoImediato->setChecked(false);

    ui->lblComandoAcionado->setText(ui->btGiroGarraMenos90->text());
    int valor = ui->tabelaPosLimites->item(4,1)->text().toInt();
    QString comandoGiroGarra = QString("[JSTE%1]").arg(valor, 4, 10, QChar('0'));

    ui->listaUltimoComandoAcionado->clear();
    ui->listaUltimoComandoAcionado->addItem(comandoGiroGarra);

    if(ui->rdbReadyForPIC->isChecked())
        giroGarraMenos90();
    else if(ui->rdbMiniMaestro24->isChecked())
    {
        // TODO: Aba Comandos: botão giro garra -90º da mini maestro 24
    }
}

void MainWindow::on_btPosicaoRepouso_clicked()
{
    on_btPararSeqComandos_clicked();
    ui->chkEnviaComandoImediato->setChecked(false);

    ui->lblComandoAcionado->setText(ui->btPosicaoRepouso->text());
    ui->listaUltimoComandoAcionado->clear();
    ui->listaUltimoComandoAcionado->addItem("[RPS]");

    if(ui->rdbReadyForPIC->isChecked())
        posicaoDeRepouso();
    else if(ui->rdbMiniMaestro24->isChecked())
    {
        // TODO: Aba Comandos: botão posição de repouso da mini maestro 24
    }
}

void MainWindow::on_btPosNeutraJST_clicked()
{
    on_btPararSeqComandos_clicked();
    ui->chkEnviaComandoImediato->setChecked(false);

    ui->lblComandoAcionado->setText(ui->btPosNeutraJST->text());

    QString comandoJST = "[JST";
    for(int i = 0; i < QTD_SERVOS; i++)
    {
        int valor = ui->tabelaPosLimites->item(i, 2)->text().toInt();
        comandoJST += idJST[i] + QString("%1").arg(valor, 4, 10, QChar('0'));
    }
    comandoJST += "]";
    ui->listaUltimoComandoAcionado->clear();
    ui->listaUltimoComandoAcionado->addItem(comandoJST);

    if(ui->rdbReadyForPIC->isChecked())
        posicaoNeutraJST();
    else if(ui->rdbMiniMaestro24->isChecked())
    {
        // TODO: Aba Comandos: botão posição neutra (JST) da mini maestro 24
    }
}

void MainWindow::on_btDesligaServos_clicked()
{
    on_btPararSeqComandos_clicked();
    ui->chkEnviaComandoImediato->setChecked(false);

    ui->lblComandoAcionado->setText(ui->btDesligaServos->text());
    ui->listaUltimoComandoAcionado->clear();
    ui->listaUltimoComandoAcionado->addItem("[JSTA0000B0000C0000D0000E0000G0000]");

    if(ui->rdbReadyForPIC->isChecked())
        desligaServos();
    else if(ui->rdbMiniMaestro24->isChecked())
    {
        // TODO: Aba Comandos: botão desliga servos da mini maestro 24
    }
}

void MainWindow::on_btPosNeutraCTZ_clicked()
{
    on_btPararSeqComandos_clicked();
    ui->chkEnviaComandoImediato->setChecked(false);

    ui->lblComandoAcionado->setText(ui->btPosNeutraCTZ->text());
    ui->listaUltimoComandoAcionado->clear();
    for(int i = 0; i < QTD_SERVOS; i++)
    {
        ui->listaUltimoComandoAcionado->addItem("[CTZ"+junta[i]+"]");
    }

    if(ui->rdbReadyForPIC->isChecked())
        posicaoNeutraCTZ();
    else if(ui->rdbMiniMaestro24->isChecked())
    {
        // TODO: Aba Comandos: botão posição neutra (CTZ) da mini maestro 24
    }
}

void MainWindow::on_btAdicionarComandoAASeqComandos_clicked()
{
    on_btPararSeqComandos_clicked();

    if (ui->listaUltimoComandoAcionado->count() > 0)
        ui->tabPrincipal->setCurrentIndex(2);

    for(int i = 0; i < ui->listaUltimoComandoAcionado->count(); i++)
    {
        ui->listSequenciaComandos->addItem(ui->listaUltimoComandoAcionado->item(i)->text());
    }

    on_btPararSeqComandos_clicked(); // Repete para atualizar o status da sequência.
}

void MainWindow::on_chkAtivarLeds_clicked(bool checked)
{
    on_btPararSeqComandos_clicked();    

    QString cmdLED = "[LED";

    for (int i = 7; i >= 0; i--)
    {
        cmdLED += lstChkLeds[i]->isChecked() ? "1" : "0";
    }
    cmdLED += "]";

    ui->edtComandoLEDAcionado->setText(cmdLED);

    if (checked)
    {
        comandoLED();
    }
}

void MainWindow::on_chkLEDPi_clicked()
{
    //QCheckBox* chkLEDPi = qobject_cast<QCheckBox*>(sender());
    if(ui->chkAtivarLeds->isChecked())
        on_chkAtivarLeds_clicked(true);
}

void MainWindow::on_btAtualizarLeds_clicked()
{
    on_btPararSeqComandos_clicked();

    comandoLEDSemParametros();
}

void MainWindow::on_btAdicionarLedsSeqComandos_clicked()
{
    on_btPararSeqComandos_clicked();

    if (ui->edtComandoLEDAcionado->text().length() > 0)
        ui->tabPrincipal->setCurrentIndex(2);

    QString comandoLED = ui->edtComandoLEDAcionado->text();
    if(comandoLED.contains("LED"))
    {
        if(comandoLED.size() == 5 || comandoLED.size() == 13)
        {
            ui->listSequenciaComandos->addItem(comandoLED);
            on_btPararSeqComandos_clicked(); // Repete para atualizar o status da sequência.
        }
    }
}


/* NOTE: ***** Aba Posições das Juntas ***** */

void MainWindow::on_btMover_clicked()
{
    on_btPararSeqComandos_clicked();

    if(ui->rdbReadyForPIC->isChecked())
        comandoJST();
    else if(ui->rdbMiniMaestro24->isChecked())
    {
        // TODO: Aba Posições das Juntas: botão mover para a Mini Maestro 24
    }
}

void MainWindow::on_btMoverComVelEAcl_clicked()
{    
    on_btPararSeqComandos_clicked();

    if(ui->rdbReadyForPIC->isChecked())
    {
        QString comando, comandoJST;
        int valor;

        on_btPararSeqComandos_clicked();

        filaComandosMoverComVelAcl.clear();

        comandoJST = "[JST";
        for(int i = 0; i < QTD_SERVOS; i++)
        {
            valor = lstSpnVel[i]->value();
            comando = QString("[VEL%1%2]").arg(junta[i]).arg(valor, 4, 10, QChar('0'));
            filaComandosMoverComVelAcl.enqueue(comando);

            valor = lstSpnAcl[i]->value();
            comando = QString("[ACL%1%2]").arg(junta[i]).arg(valor, 4, 10, QChar('0'));
            filaComandosMoverComVelAcl.enqueue(comando);

            valor = lstSpnAlvo[i]->value();
            comandoJST += idJST[i] + QString("%1").arg(valor, 4, 10, QChar('0'));
        }

        comandoJST += "]";

        filaComandosMoverComVelAcl.enqueue(comandoJST);

        enviaComando(filaComandosMoverComVelAcl.dequeue());
    }
    else if(ui->rdbMiniMaestro24->isChecked())
    {
        // TODO: Aba Posições das Juntas: botão mover com vel e acl para a placa Mini Maestro 24
    }

}

void MainWindow::on_btCalcularXYZAlvo_clicked()
{
    // TODO: Aba Posições das Juntas: Botão de cinemática direta
}

void MainWindow::on_btCalcularAngulosAlvo_clicked()
{
    // TODO: Aba Posições das Juntas: Botão de cinemática inversa
}

void MainWindow::habilitaCamposAbaPosicaoAlvo(int posicaoAba, bool estadoHab)
{
    for(int i = 0; i < QTD_SERVOS; i++)
    {
        lstChkHab[i]->setEnabled(estadoHab);
        lstSlider[i]->setEnabled(estadoHab);
    }

    switch(posicaoAba)
    {
        case 0:
            for(int i = 0; i < QTD_SERVOS; i++)
            {
                lstSpnAlvo[i]->setEnabled(estadoHab);
                lstSpnVel[i]->setEnabled(estadoHab);
                lstSpnAcl[i]->setEnabled(estadoHab);
            }
            break;

        case 1:
            for(int i = 0; i < QTD_SERVOS; i++)
            {
                lstSpnAlvoGraus[i]->setEnabled(estadoHab);
                lstSpnVelGrausPorSeg[i]->setEnabled(estadoHab);
                lstSpnAclGrausPorSegQuad[i]->setEnabled(estadoHab);
            }
            break;
    }
}

void MainWindow::habilitaCamposAbaPosicaoAlvoJunta(int posicaoAba, int idxJunta, bool estadoHab)
{
    lstSlider[idxJunta]->setEnabled(estadoHab);
    lstChkHab[idxJunta]->setEnabled(estadoHab);

    switch(posicaoAba)
    {
        case 0:
            lstSpnAlvo[idxJunta]->setEnabled(estadoHab);
            lstSpnVel[idxJunta]->setEnabled(estadoHab);
            lstSpnAcl[idxJunta]->setEnabled(estadoHab);
            break;

        case 1:
            lstSpnAlvoGraus[idxJunta]->setEnabled(estadoHab);
            lstSpnVelGrausPorSeg[idxJunta]->setEnabled(estadoHab);
            lstSpnAclGrausPorSegQuad[idxJunta]->setEnabled(estadoHab);
            break;
    }
}

void MainWindow::enviaPosicaoAlvoAssimQueMudar(int idxJunta, int posicaoMicrossegundos)
{
    if(ui->chkEnviaComandoImediato->isChecked())
    {
        //habilitaCamposAbaPosicaoAlvo(ui->tabUnidadePos->currentIndex(), false);

        if(posicaoMicrossegundos > 0 && posicaoMicrossegundos < tempoPulsoMin[idxJunta])
        {
            lstSpnAlvo[idxJunta]->setValue(tempoPulsoMin[idxJunta]);
        }
        else if(posicaoMicrossegundos > 0 && posicaoMicrossegundos > tempoPulsoMax[idxJunta])
        {
            lstSpnAlvo[idxJunta]->setValue(tempoPulsoMax[idxJunta]);
        }
        else if(ui->rdbReadyForPIC->isChecked())
        {                        
            comandoEnvioImediato = QString("[JST%1%2]").arg(idJST[idxJunta]).arg(posicaoMicrossegundos, 4, 10, QChar('0'));

            if(!timerEnvioImediato->isActive())
            {
                timerEnvioImediato->start(TEMPO_TIMER_ENVIO_IMEDIATO_US);
            }

        }
        else if(ui->rdbMiniMaestro24->isChecked())
        {
            // TODO: Aba Posições das Juntas: envio imediato de posicao alvo para a Mini Maestro 24
        }
    }
}

void MainWindow::enviaVelocidadeAssimQueMudar(int idxJunta, int velocidadeMicrossegundos)
{
    if(ui->chkEnviaComandoImediato->isChecked())
    {
        //habilitaCamposAbaPosicaoAlvo(ui->tabUnidadePos->currentIndex(), false);
        if(velocidadeMicrossegundos > velocidadesMax[idxJunta])
        {
            lstSpnVel[idxJunta]->setValue(velocidadesMax[idxJunta]);
        }
        else if(ui->rdbReadyForPIC->isChecked())
        {
            if(velocidadeMicrossegundos > 0)
            {
                comandoEnvioImediato = QString("[VEL%1%2]").arg(junta[idxJunta]).arg(velocidadeMicrossegundos, 4, 10, QChar('0'));

                if(!timerEnvioImediato->isActive())
                {
                    timerEnvioImediato->start(TEMPO_TIMER_ENVIO_IMEDIATO_US);
                }
            }
            else
            {
                lstSpnVel[idxJunta]->setValue(1);
            }
        }
        else if(ui->rdbMiniMaestro24->isChecked())
        {
            // TODO: Aba Posições das Juntas: envio imediato de velocidade para a Mini Maestro 24
        }
    }
}

void MainWindow::enviaAceleracaoAssimQueMudar(int idxJunta, int aceleracaoMicrossegundos)
{
    if(ui->chkEnviaComandoImediato->isChecked())
    {
        //habilitaCamposAbaPosicaoAlvo(ui->tabUnidadePos->currentIndex(), false);
        if(aceleracaoMicrossegundos > aceleracoesMax[idxJunta])
        {
            lstSpnAcl[idxJunta]->setValue(aceleracoesMax[idxJunta]);
        }
        else if(ui->rdbReadyForPIC->isChecked())
        {
            if(aceleracaoMicrossegundos > 0)
            {
                comandoEnvioImediato = QString("[ACL%1%2]").arg(junta[idxJunta]).arg(aceleracaoMicrossegundos, 4, 10, QChar('0'));

                if(!timerEnvioImediato->isActive())
                {
                    timerEnvioImediato->start(TEMPO_TIMER_ENVIO_IMEDIATO_US);
                }
            }
            else
            {
                lstSpnAcl[idxJunta]->setValue(1);
            }
        }
        else if(ui->rdbMiniMaestro24->isChecked())
        {
            // TODO: Aba Posições das Juntas: envio imediato de aceleração para a Mini Maestro 24
        }
    }
}

void MainWindow::on_spnJ0Alvo_valueChanged(int posicaoMicrossegundos)
{
    converteSpnAlvoParaGraus(0, posicaoMicrossegundos);
    enviaPosicaoAlvoAssimQueMudar(0, posicaoMicrossegundos);
}

void MainWindow::on_spnJ1Alvo_valueChanged(int posicaoMicrossegundos)
{
    converteSpnAlvoParaGraus(1, posicaoMicrossegundos);
    enviaPosicaoAlvoAssimQueMudar(1, posicaoMicrossegundos);
}

void MainWindow::on_spnJ2Alvo_valueChanged(int posicaoMicrossegundos)
{
    converteSpnAlvoParaGraus(2, posicaoMicrossegundos);
    enviaPosicaoAlvoAssimQueMudar(2, posicaoMicrossegundos);
}

void MainWindow::on_spnJ3Alvo_valueChanged(int posicaoMicrossegundos)
{
    converteSpnAlvoParaGraus(3, posicaoMicrossegundos);
    enviaPosicaoAlvoAssimQueMudar(3, posicaoMicrossegundos);
}

void MainWindow::on_spnJ4Alvo_valueChanged(int posicaoMicrossegundos)
{
    converteSpnAlvoParaGraus(4, posicaoMicrossegundos);
    enviaPosicaoAlvoAssimQueMudar(4, posicaoMicrossegundos);
}

void MainWindow::on_spnGRAlvo_valueChanged(int posicaoMicrossegundos)
{
    converteSpnAlvoParaGraus(5, posicaoMicrossegundos);
    enviaPosicaoAlvoAssimQueMudar(5, posicaoMicrossegundos);
}

void MainWindow::on_spnJ0Vel_valueChanged(int velTmpPulso)
{
    converteSpnVelParaGrausPorSeg(0, velTmpPulso);
    enviaVelocidadeAssimQueMudar(0, velTmpPulso);
}

void MainWindow::on_spnJ1Vel_valueChanged(int velTmpPulso)
{
    converteSpnVelParaGrausPorSeg(1, velTmpPulso);
    enviaVelocidadeAssimQueMudar(1, velTmpPulso);
}

void MainWindow::on_spnJ2Vel_valueChanged(int velTmpPulso)
{
    converteSpnVelParaGrausPorSeg(2, velTmpPulso);
    enviaVelocidadeAssimQueMudar(2, velTmpPulso);
}

void MainWindow::on_spnJ3Vel_valueChanged(int velTmpPulso)
{
    converteSpnVelParaGrausPorSeg(3, velTmpPulso);
    enviaVelocidadeAssimQueMudar(3, velTmpPulso);
}

void MainWindow::on_spnJ4Vel_valueChanged(int velTmpPulso)
{
    converteSpnVelParaGrausPorSeg(4, velTmpPulso);
    enviaVelocidadeAssimQueMudar(4, velTmpPulso);
}

void MainWindow::on_spnGRVel_valueChanged(int velTmpPulso)
{
    converteSpnVelParaGrausPorSeg(5, velTmpPulso);
    enviaVelocidadeAssimQueMudar(5, velTmpPulso);
}

void MainWindow::on_spnJ0Acl_valueChanged(int aclTmpPulso)
{
    converteSpnAclParaGrausPorSegQuad(0, aclTmpPulso);
    enviaAceleracaoAssimQueMudar(0, aclTmpPulso);
}

void MainWindow::on_spnJ1Acl_valueChanged(int aclTmpPulso)
{
    converteSpnAclParaGrausPorSegQuad(1, aclTmpPulso);
    enviaAceleracaoAssimQueMudar(1, aclTmpPulso);
}

void MainWindow::on_spnJ2Acl_valueChanged(int aclTmpPulso)
{
    converteSpnAclParaGrausPorSegQuad(2, aclTmpPulso);
    enviaAceleracaoAssimQueMudar(2, aclTmpPulso);
}

void MainWindow::on_spnJ3Acl_valueChanged(int aclTmpPulso)
{
    converteSpnAclParaGrausPorSegQuad(3, aclTmpPulso);
    enviaAceleracaoAssimQueMudar(3, aclTmpPulso);
}

void MainWindow::on_spnJ4Acl_valueChanged(int aclTmpPulso)
{
    converteSpnAclParaGrausPorSegQuad(4, aclTmpPulso);
    enviaAceleracaoAssimQueMudar(4, aclTmpPulso);
}

void MainWindow::on_spnGRAcl_valueChanged(int aclTmpPulso)
{
    converteSpnAclParaGrausPorSegQuad(5, aclTmpPulso);
    enviaAceleracaoAssimQueMudar(5, aclTmpPulso);
}

void MainWindow::on_spnJ0AlvoGraus_valueChanged(double posicaoGraus)
{    
    converteSpnAlvoGrausParaTmpPulso(0, posicaoGraus);
}

void MainWindow::on_spnJ1AlvoGraus_valueChanged(double posicaoGraus)
{
    converteSpnAlvoGrausParaTmpPulso(1, posicaoGraus);
}

void MainWindow::on_spnJ2AlvoGraus_valueChanged(double posicaoGraus)
{
    converteSpnAlvoGrausParaTmpPulso(2, posicaoGraus);
}

void MainWindow::on_spnJ3AlvoGraus_valueChanged(double posicaoGraus)
{
    converteSpnAlvoGrausParaTmpPulso(3, posicaoGraus);
}

void MainWindow::on_spnJ4AlvoGraus_valueChanged(double posicaoGraus)
{
    converteSpnAlvoGrausParaTmpPulso(4, posicaoGraus);
}

void MainWindow::on_spnGRAlvoGraus_valueChanged(double posicaoGraus)
{
    converteSpnAlvoGrausParaTmpPulso(5, posicaoGraus);
}

void MainWindow::on_spnJ0VelGrausPorSeg_valueChanged(double velGrausPorSeg)
{
    converteSpnVelGrausPorSegParaTmpPulso(0, velGrausPorSeg);
}

void MainWindow::on_spnJ1VelGrausPorSeg_valueChanged(double velGrausPorSeg)
{
    converteSpnVelGrausPorSegParaTmpPulso(1, velGrausPorSeg);
}

void MainWindow::on_spnJ2VelGrausPorSeg_valueChanged(double velGrausPorSeg)
{
    converteSpnVelGrausPorSegParaTmpPulso(2, velGrausPorSeg);
}

void MainWindow::on_spnJ3VelGrausPorSeg_valueChanged(double velGrausPorSeg)
{
    converteSpnVelGrausPorSegParaTmpPulso(3, velGrausPorSeg);
}

void MainWindow::on_spnJ4VelGrausPorSeg_valueChanged(double velGrausPorSeg)
{
    converteSpnVelGrausPorSegParaTmpPulso(4, velGrausPorSeg);
}

void MainWindow::on_spnGRVelGrausPorSeg_valueChanged(double velGrausPorSeg)
{
    converteSpnVelGrausPorSegParaTmpPulso(5, velGrausPorSeg);
}

void MainWindow::on_spnJ0AclGrausPorSegQuad_valueChanged(double aclGrausPorSegQuad)
{
    converteSpnAclGrausPorSegQuadParaTmpPulso(0, aclGrausPorSegQuad);
}

void MainWindow::on_spnJ1AclGrausPorSegQuad_valueChanged(double aclGrausPorSegQuad)
{
    converteSpnAclGrausPorSegQuadParaTmpPulso(1, aclGrausPorSegQuad);
}

void MainWindow::on_spnJ2AclGrausPorSegQuad_valueChanged(double aclGrausPorSegQuad)
{
    converteSpnAclGrausPorSegQuadParaTmpPulso(2, aclGrausPorSegQuad);
}

void MainWindow::on_spnJ3AclGrausPorSegQuad_valueChanged(double aclGrausPorSegQuad)
{
    converteSpnAclGrausPorSegQuadParaTmpPulso(3, aclGrausPorSegQuad);
}

void MainWindow::on_spnJ4AclGrausPorSegQuad_valueChanged(double aclGrausPorSegQuad)
{
    converteSpnAclGrausPorSegQuadParaTmpPulso(4, aclGrausPorSegQuad);
}

void MainWindow::on_spnGRAclGrausPorSegQuad_valueChanged(double aclGrausPorSegQuad)
{
    converteSpnAclGrausPorSegQuadParaTmpPulso(5, aclGrausPorSegQuad);
}

void MainWindow::habilitaJunta(int idxJunta, bool checked)
{
    if(checked)
    {
        int valor = lstSpnAlvo[idxJunta]->value();

        if(valor <= 0)
        {
            valor = ui->tabelaPosLimites->item(idxJunta, 3)->text().toInt();
            lstSpnAlvo[idxJunta]->setValue(valor);
        }

        if(!lstSpnAlvo[idxJunta]->isEnabled())
        {
            converteSpnAlvoParaGraus(idxJunta, valor);
        }

        enviaVelocidadeAssimQueMudar(idxJunta, lstSpnVel[idxJunta]->value());
        enviaAceleracaoAssimQueMudar(idxJunta, lstSpnAcl[idxJunta]->value());
        enviaPosicaoAlvoAssimQueMudar(idxJunta, valor);
    }
    else
    {
        lstSpnAlvo[idxJunta]->setEnabled(false);
        lstSpnAlvoGraus[idxJunta]->setEnabled(false);
        lstChkHab[idxJunta]->setChecked(false);
        lstSlider[idxJunta]->setEnabled(false);

        enviaPosicaoAlvoAssimQueMudar(idxJunta, 0);
    }
}

void MainWindow::on_chkJ0_clicked(bool checked)
{
    habilitaJunta(0, checked);
}

void MainWindow::on_chkJ1_clicked(bool checked)
{
    habilitaJunta(1, checked);
}

void MainWindow::on_chkJ2_clicked(bool checked)
{
    habilitaJunta(2, checked);
}

void MainWindow::on_chkJ3_clicked(bool checked)
{
    habilitaJunta(3, checked);
}

void MainWindow::on_chkJ4_clicked(bool checked)
{
    habilitaJunta(4, checked);
}

void MainWindow::on_chkGR_clicked(bool checked)
{
    habilitaJunta(5, checked);
}

void MainWindow::on_chkJ0Ang_clicked(bool checked)
{
    habilitaJunta(0, checked);
}

void MainWindow::on_chkJ1Ang_clicked(bool checked)
{
    habilitaJunta(1, checked);
}

void MainWindow::on_chkJ2Ang_clicked(bool checked)
{
    habilitaJunta(2, checked);
}

void MainWindow::on_chkJ3Ang_clicked(bool checked)
{
    habilitaJunta(3, checked);
}

void MainWindow::on_chkJ4Ang_clicked(bool checked)
{
    habilitaJunta(4, checked);
}

void MainWindow::on_chkGRAng_clicked(bool checked)
{
    habilitaJunta(5, checked);
}

void MainWindow::on_chkEnviaComandoImediato_toggled(bool checked)
{
    ui->btMover->setEnabled(!checked);
    ui->btMoverComVelEAcl->setEnabled(!checked);
}

void MainWindow::timeoutEnvioImediato()
{
    countAbaPosicoesValueChanged++;
    enviaComando(comandoEnvioImediato);
}

void MainWindow::sliderValueChanged(int idxJunta, int value)
{
    if(value != lstSpnAlvo[idxJunta]->value())
    {
        lstSpnAlvo[idxJunta]->setValue(value);
    }
}

void MainWindow::on_sliderJ0_valueChanged(int value)
{
    sliderValueChanged(0, value);
}

void MainWindow::on_sliderJ1_valueChanged(int value)
{
    sliderValueChanged(1, value);
}

void MainWindow::on_sliderJ2_valueChanged(int value)
{
    sliderValueChanged(2, value);
}

void MainWindow::on_sliderJ3_valueChanged(int value)
{
    sliderValueChanged(3, value);
}

void MainWindow::on_sliderJ4_valueChanged(int value)
{
    sliderValueChanged(4, value);
}

void MainWindow::on_sliderGR_valueChanged(int value)
{
    sliderValueChanged(5, value);
}


/* NOTE: ***** Aba Sequência de Comandos ***** */

void MainWindow::on_btNovaSequencia_clicked()
{
    on_btPararSeqComandos_clicked();
    // TODO: Adicionar messagebox de confirmação para nova sequência de comandos    
    ui->lblNomeArquivoSequencia->setText("Nova sequência");
    ui->listSequenciaComandos->clear();

    on_btPararSeqComandos_clicked(); // Repete para atualizar o status da sequência.
}

void MainWindow::on_btAdicionarComando_clicked()
{
    on_btPararSeqComandos_clicked();

    montagemDeComandosDialog->setMainWindow(this);
    montagemDeComandosDialog->setEmEdicao(false);
    montagemDeComandosDialog->setTextEdtComando("");
    montagemDeComandosDialog->setTabelaPosLimites(ui->tabelaPosLimites);
    montagemDeComandosDialog->setTabelaPosLimitesGraus(ui->tabelaPosLimitesGraus);
    montagemDeComandosDialog->setVelocidades(this->lstSpnVel);
    montagemDeComandosDialog->setAceleracoes(this->lstSpnAcl);

    montagemDeComandosDialog->show();
}

void MainWindow::adicionarComandoAASequencia(QString comando)
{
    ui->listSequenciaComandos->addItem(comando);
    on_btPararSeqComandos_clicked();
}

void MainWindow::editarComandoNaSequencia(QString comando)
{
    ui->listSequenciaComandos->currentItem()->setText(comando);
    on_btPararSeqComandos_clicked();
}

void MainWindow::on_btAdicionarPosCorrente_clicked()
{
    on_btPararSeqComandos_clicked();

    QString comandoJST = "[JST";
    QString comando;
    int valor;

    for(int i = 0; i < QTD_SERVOS; i++)
    {
        if(ui->chkComVelocidades->isChecked())
        {
            valor = lstSpnVel[i]->value();
            comando = QString("[VEL%1%2]").arg(junta[i]).arg(valor, 4, 10, QChar('0'));
            ui->listSequenciaComandos->addItem(comando);
        }

        if(ui->chkComAceleracoes->isChecked())
        {
            valor = lstSpnAcl[i]->value();
            comando = QString("[ACL%1%2]").arg(junta[i]).arg(valor, 4, 10, QChar('0'));
            ui->listSequenciaComandos->addItem(comando);
        }

        comandoJST += idJST[i] + QString("%1").arg(lstEdtAtual[i]->text().toInt(), 4, 10, QChar('0'));
    }

    comandoJST += "]";

    ui->listSequenciaComandos->addItem(comandoJST);
    on_btPararSeqComandos_clicked(); // Repete para atualizar o status da sequência.
}

void MainWindow::on_btComandoUp_clicked()
{
    on_btPararSeqComandos_clicked();

    int tamanho = ui->listSequenciaComandos->count();
    int index_selected = ui->listSequenciaComandos->currentRow();
    QString comandoTemp;

    if(tamanho > 1 && index_selected > 0)
    {
        comandoTemp = ui->listSequenciaComandos->currentItem()->text();
        ui->listSequenciaComandos->currentItem()->setText(ui->listSequenciaComandos->item(index_selected-1)->text());
        ui->listSequenciaComandos->item(index_selected-1)->setText(comandoTemp);

        ui->listSequenciaComandos->setCurrentRow(index_selected-1);
    }
}

void MainWindow::on_btComandoDown_clicked()
{
    on_btPararSeqComandos_clicked();

    int tamanho = ui->listSequenciaComandos->count();
    int index_selected = ui->listSequenciaComandos->currentRow();
    QString comandoTemp;

    if(tamanho > 1 && index_selected < tamanho - 1)
    {
        comandoTemp = ui->listSequenciaComandos->currentItem()->text();
        ui->listSequenciaComandos->currentItem()->setText(ui->listSequenciaComandos->item(index_selected+1)->text());
        ui->listSequenciaComandos->item(index_selected+1)->setText(comandoTemp);

        ui->listSequenciaComandos->setCurrentRow(index_selected+1);
    }
}

void MainWindow::on_btEditarComando_clicked()
{
    on_btPararSeqComandos_clicked();
    int index_selected = ui->listSequenciaComandos->currentRow();
    if(ui->listSequenciaComandos->count() > 0 && index_selected >= 0)
    {
        montagemDeComandosDialog->setEmEdicao(true);
        montagemDeComandosDialog->setTextEdtComando(ui->listSequenciaComandos->item(index_selected)->text());
        montagemDeComandosDialog->setTabelaPosLimites(ui->tabelaPosLimites);
        montagemDeComandosDialog->setTabelaPosLimitesGraus(ui->tabelaPosLimitesGraus);
        montagemDeComandosDialog->setVelocidades(this->lstSpnVel);
        montagemDeComandosDialog->setAceleracoes(this->lstSpnAcl);

        montagemDeComandosDialog->show();
    }
}

void MainWindow::on_btRemoverComando_clicked()
{
    on_btPararSeqComandos_clicked();

    int tamanho = ui->listSequenciaComandos->count();

    if(tamanho > 0)
    {
        int index_selected = ui->listSequenciaComandos->currentRow();

        QListWidgetItem* item = ui->listSequenciaComandos->currentItem();
        delete item;

        if(index_selected - 1 >= 0)
            index_selected = index_selected - 1;

        tamanho = ui->listSequenciaComandos->count();

        if(tamanho > 0)
        {
            int indiceASelecionar = (index_selected < tamanho) ? index_selected : tamanho;
            ui->listSequenciaComandos->setCurrentRow(indiceASelecionar);
        }
    }

    on_btPararSeqComandos_clicked(); // Repete para atualizar o status da sequência.
}

void MainWindow::on_btCarregarSeqComandos_clicked()
{
    on_btPararSeqComandos_clicked();

    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Carregar Sequência de Comandos"), "", tr("Sequência (*.seq);;Arquivo TXT(*.txt)"));

    QFile file(fileName);

    if (file.open(QIODevice::ReadOnly | QIODevice::Text | QIODevice::ReadWrite))
    {
        ui->listSequenciaComandos->clear();

        while (!file.atEnd())
        {
            QByteArray line = file.readLine();
            ui->listSequenciaComandos->addItem(QString(line).trimmed());
        }

        QString nomeArquivo = fileName.section("/",-1,-1);

        ui->lblNomeArquivoSequencia->setText(nomeArquivo);

        on_btPararSeqComandos_clicked(); // Repete para atualizar o status da sequência.
    }
}

void MainWindow::on_btSalvarSeqComandos_clicked()
{    
    QString fileName = QFileDialog::getSaveFileName(this,
        tr("Salvar sequência de comandos"), "", tr("Sequência (*.seq);;Arquivo TXT(*.txt)"));

    QFile file(fileName);
    if ( file.open(QIODevice::ReadWrite | QIODevice::Text) )
    {
        QTextStream stream(&file);

        for(int i = 0; i < ui->listSequenciaComandos->count(); i++)
        {
            stream << ui->listSequenciaComandos->item(i)->text() << "\n" << flush;
        }

        QString nomeArquivo = fileName.section("/",-1,-1);

        ui->lblNomeArquivoSequencia->setText(nomeArquivo);
    }
}

void MainWindow::on_btExecutarSeqComandos_clicked()
{    
    if(ui->listSequenciaComandos->count() > 0)
    {        
        ui->chkEnviaComandoImediato->setChecked(false);
        seqEmExecucao = true;
        emLoop = false;
        ui->listSequenciaComandos->setCurrentRow(0);
        ui->lblStatusSeqComandos->setText(STATUS_SEQCOM_EM_EXECUCAO);
        if(ui->rdbReadyForPIC->isChecked())
            parser(ui->listSequenciaComandos->currentItem()->text());
        else if(ui->rdbMiniMaestro24->isChecked())
        {
            // TODO: Aba sequência: parser para a Mini Maestro 24
        }
    }
}

void MainWindow::on_btContinuarSeqComandos_clicked()
{    
    if(ui->listSequenciaComandos->count() > 0)
    {
        ui->chkEnviaComandoImediato->setChecked(false);
        seqEmExecucao = true;
        emLoop = false;
        int index_selected = ui->listSequenciaComandos->currentRow();
        if(index_selected < 0 || index_selected > ui->listSequenciaComandos->count())
            ui->listSequenciaComandos->setCurrentRow(0);
        ui->lblStatusSeqComandos->setText(STATUS_SEQCOM_CONTINUANDO_EXEC);
        if(ui->rdbReadyForPIC->isChecked())
            parser(ui->listSequenciaComandos->currentItem()->text());
        else if(ui->rdbMiniMaestro24->isChecked())
        {
            // TODO: Aba sequência: parser para a Mini Maestro 24
        }
    }
}

void MainWindow::on_btExecutarLoopSeqComandos_clicked()
{    
    if(ui->listSequenciaComandos->count() > 0)
    {
        ui->chkEnviaComandoImediato->setChecked(false);
        seqEmExecucao = true;
        emLoop = true;
        ui->listSequenciaComandos->setCurrentRow(0);
        ui->lblStatusSeqComandos->setText(STATUS_SEQCOM_EM_LOOP);
        if(ui->rdbReadyForPIC->isChecked())
            parser(ui->listSequenciaComandos->currentItem()->text());
        else if(ui->rdbMiniMaestro24->isChecked())
        {
            // TODO: Aba sequência: parser para a Mini Maestro 24
        }
    }
}

void MainWindow::on_btContinuarLoopSeqComandos_clicked()
{    
    if(ui->listSequenciaComandos->count() > 0)
    {
        ui->chkEnviaComandoImediato->setChecked(false);
        seqEmExecucao = true;
        emLoop = true;
        int index_selected = ui->listSequenciaComandos->currentRow();
        if(index_selected < 0 || index_selected > ui->listSequenciaComandos->count())
            ui->listSequenciaComandos->setCurrentRow(0);
        ui->lblStatusSeqComandos->setText(STATUS_SEQCOM_CONTINUANDO_LOOP);
        if(ui->rdbReadyForPIC->isChecked())
            parser(ui->listSequenciaComandos->currentItem()->text());
        else if(ui->rdbMiniMaestro24->isChecked())
        {
            // TODO: Aba sequência: parser para a Mini Maestro 24
        }
    }
}

void MainWindow::on_btPararSeqComandos_clicked()
{    
    seqEmExecucao = false;
    emLoop = false;
    emDLYSemParam = false;
    if(ui->listSequenciaComandos->count() == 0)
    {
        ui->lblStatusSeqComandos->setText(STATUS_SEQCOM_PARADA_VAZIA);
    }
    else
    {
        ui->lblStatusSeqComandos->setText(STATUS_SEQCOM_PARADA_NAO_VAZIA);
        ui->lblStatusSeqComandos->setFocus();
    }
}

bool MainWindow::parser(QString comando)
{
    if(comando.at(0) != '[' || comando.at(comando.length() - 1) != ']')
        return false;

    if(comando.contains("VEL"))
    {
        QString strJunta = comando.mid(4,2);

        if(comando.size() == 11)
        {
            int valor = comando.mid(6,4).toInt();

            int idxJunta = -1;

            if(strJunta.at(0) == 'J')
                idxJunta = strJunta.at(1).toLatin1() - '0';
            else if(strJunta.at(0) == 'G')
                idxJunta = 5;

            if(idxJunta != -1)
            {
                lstSpnVel[idxJunta]->setValue(valor);
                enviaComando(comando);
                return true;
            }
        }
        else if(comando.size() == 7)
        {
            enviaComando(comando);
            return true;
        }
    }
    else if(comando.contains("ACL"))
    {
        QString strJunta = comando.mid(4,2);

        if(comando.size() == 11)
        {
            int valor = comando.mid(6,4).toInt();

            int idxJunta = -1;

            if(strJunta.at(0) == 'J')
                idxJunta = strJunta.at(1).toLatin1() - '0';
            else if(strJunta.at(0) == 'G')
                idxJunta = 5;

            if(idxJunta != -1)
            {
                lstSpnAcl[idxJunta]->setValue(valor);
                enviaComando(comando);
                return true;
            }
        }
        else if(comando.size() == 7)
        {
            enviaComando(comando);
            return true;
        }
    }
    else if(comando.contains("JST"))
    {
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
                    int valor = comando.mid(5*(i+1), 4).toInt();                    

                    if(valor > 0)
                    {
                        lstSpnAlvo[idxJunta]->setValue(valor);
                    }
                    else
                        lstChkHab[idxJunta]->setChecked(false);
                }
            }
            enviaComando(comando);
            return true;
        }
    }
    else if(comando.contains("RPS"))
    {
        for (int i = 0; i < QTD_SERVOS; i++)
        {
            int valor = ui->tabelaPosLimites->item(i, 3)->text().toInt();
            this->lstSpnAlvo[i]->setValue(valor);
        }

        enviaComando(comando);
        return true;
    }
    else if(comando.contains("CTZ"))
    {
        QString strJunta = comando.mid(4,2);

        int idxJunta = -1;

        if(strJunta.at(0) == 'J')
            idxJunta = strJunta.at(1).toLatin1() - '0';
        else if(strJunta.at(0) == 'G')
            idxJunta = 5;

        if(idxJunta != -1)
        {            
            int valor = ui->tabelaPosLimites->item(idxJunta, 2)->text().toInt();
            lstSpnAlvo[idxJunta]->setValue(valor);
        }
        enviaComando(comando);
        return true;
    }
    else if(comando.contains("GA"))
    {        
        int valor = ui->tabelaPosLimites->item(5, 0)->text().toInt();
        ui->spnGRAlvo->setValue(valor);

        enviaComando(comando);
        return true;
    }
    else if(comando.contains("GF"))
    {        
        int valor = ui->tabelaPosLimites->item(5, 1)->text().toInt();
        ui->spnGRAlvo->setValue(valor);

        enviaComando(comando);
        return true;
    }
    else if(comando.contains("LED"))
    {
        enviaComando(comando);
        return true;
    }
    else if(comando.contains("DLY"))
    {
        if(comando.length() == 9)
        {
            int delayMs = comando.mid(4, 4).toInt();
            if(delayMs > 0)
                timerDLY->start(delayMs);
            else
            {
                iniciaDLYSemParametro();
            }
            return true;
        }
        else if(comando.length() == 5)
        {
            iniciaDLYSemParametro();
            return true;
        }
    }

    return false;
}

void MainWindow::iniciaDLYSemParametro()
{
    // Se cair aqui, a sequência para, pois não envia comando para o robô,
    // consequente não recebe resposta. Como o timer não é iniciado também,
    // não haverá a execução do estouro do timer, restando apenas o evento
    // que pega a tecla ENTER ou duplo clique.
    timerDLY->stop();
    emDLYSemParam = true;
    posUltimoDLYSemParam = ui->listSequenciaComandos->currentRow();
    ultimoStatusSeqComandos = ui->lblStatusSeqComandos->text();
    ui->listSequenciaComandos->setFocus();
    ui->lblStatusSeqComandos->setText(STATUS_SEQCOM_EM_DLY_SEM_PAR);
}

void MainWindow::timeoutDLY()
{
    if(!seqEmExecucao)
        return;

    executaComandoDaSequencia();
}

void MainWindow::on_listSequenciaComandos_itemActivated(QListWidgetItem *item)
{
    Q_UNUSED(item);

    if(!seqEmExecucao)
    {
        QString comando = item->text();

        parser(comando);

        return;
    }

    if(!emDLYSemParam)
    {
        return;
    }

    ui->lblStatusSeqComandos->setText(ultimoStatusSeqComandos);

    emDLYSemParam = false;

    if(posUltimoDLYSemParam >= 0)
    {
        ui->listSequenciaComandos->setCurrentRow(posUltimoDLYSemParam);
        posUltimoDLYSemParam = -1;
    }

    executaComandoDaSequencia();
}


/*NOTE: ***** Aba configurações ***** */

void MainWindow::on_rdbReadyForPIC_clicked()
{
    ui->rdbMiniMaestro24->setChecked(false);
}

void MainWindow::on_rdbMiniMaestro24_clicked()
{
    ui->rdbReadyForPIC->setChecked(false);
}

void MainWindow::on_btObterPosLimites_clicked()
{
    on_btPararSeqComandos_clicked();

    QString comandoConfig[4] = {"TMX", "TMN", "T90", "TRP"};

    for(int i = 0; i < QTD_SERVOS; i++)
    {
        for(int j = 0; j < 4; j++)
            filaConfigPosLimites.enqueue(QString("[%1%2]").arg(comandoConfig[j], junta[i]));
    }

    enviaComando(filaConfigPosLimites.dequeue());
}

void MainWindow::on_btConfigurarPosLimites_clicked()
{
    on_btPararSeqComandos_clicked();

    QString comandoConfig[4] = {"TMX", "TMN", "T90", "TRP"};

    for(int i = 0; i < QTD_SERVOS; i++)
    {
        for(int j = 0; j < 4; j++)
        {
            int valor = ui->tabelaPosLimites->item(i, j)->text().toInt();
            filaConfigPosLimites.enqueue(QString("[%1%2%3]").arg(comandoConfig[j], junta[i]).arg(valor, 4, 10, QChar('0')));
        }
    }

    enviaComando(filaConfigPosLimites.dequeue());
}

void MainWindow::on_btConfigurarPosLimitesGraus_clicked()
{    
    configurarConversaoEntreMicrossegundosEAngulos();
}

void MainWindow::on_rdbSemFeedback_clicked(bool checked)
{    
    if(checked)
    {
        on_btPararSeqComandos_clicked();
        enviaComando("[FRS0]");
    };
}

void MainWindow::on_rdbPosicoesCorrentesDasJuntas_clicked(bool checked)
{
    if(checked)
    {
        on_btPararSeqComandos_clicked();
        enviaComando("[FRS1]");
    }
}

void MainWindow::on_rdbSinalDeMovimento_clicked(bool checked)
{    
    if(checked)
    {
        on_btPararSeqComandos_clicked();
        enviaComando("[FRS2]");
    }
}

void MainWindow::on_chkComandosBloqueantesDeMovimento_clicked(bool checked)
{
    on_btPararSeqComandos_clicked();
    if(checked)
        enviaComando("[CSB1]");
    else
        enviaComando("[CSB0]");
}

void MainWindow::on_btResetarPlacaControle_clicked()
{
    on_btPararSeqComandos_clicked();
    enviaComando("[RST]");
}

void MainWindow::on_btResetarPlacaServos_clicked()
{
    on_btPararSeqComandos_clicked();
    enviaComando("[RSTM]");
}


/* NOTE: ***** Aba Terminal ***** */

void MainWindow::on_chkEcoLocal_clicked(bool checked)
{
    console->setLocalEchoEnabled(checked);
}


