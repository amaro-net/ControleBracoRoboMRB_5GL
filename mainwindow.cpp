#include <QMessageBox>
#include <QtSerialPort/QSerialPortInfo>
#include <QFile>
#include <QTextStream>
#include <QFileDialog>
#include <math.h>
#include <qmath.h>
#include <QThread>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "constantes.h"
#include "utils.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QLocale::setDefault(QLocale::system());

    console = new Console(ui->tabTerminal);
    console->setEnabled(false);
    console->setVisible(true);
    console->setLocalEchoEnabled(ui->chkEcoLocal->isChecked());

    serial = new QSerialPort(this);

    montagemDeComandosDialog = new MontagemDeComandosDialog(this);
    montagemDeComandosDialog->setMainWindow(this);

    preencheCombosPortaSerial();

    tabTerminalReadyForPIC = ui->tabTerminal;
    idxTerminal = ui->tabPrincipal->indexOf(ui->tabTerminal);
    tabTerminalReadyForPICLabel = ui->tabPrincipal->tabText(idxTerminal);

    tabTerminalMiniMaestro = ui->tabTerminalMiniMaestro;
    tabTerminalMiniMaestroLabel = ui->tabPrincipal->tabText(ui->tabPrincipal->indexOf(ui->tabTerminalMiniMaestro));

    ui->tabPrincipal->removeTab(ui->tabPrincipal->indexOf(ui->tabTerminalMiniMaestro));

    if(ui->rdbReadyForPIC->isChecked())
        mostrarAbaTerminal(ui->rdbReadyForPIC);
    else if(ui->rdbMiniMaestro24->isChecked())
    {
        mostrarAbaTerminal(ui->rdbMiniMaestro24);
        ui->listErrosAtivos->clear();
    }

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

    on_btNovaSequencia_clicked(true);

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

    for(int i = 0; i < QTD_SERVOS; i++)
    {
        lstPontoVerdeSliderPosAtual[i]->setVisible(false);
    }

    configurarConversaoEntreMicrossegundosEAngulos(true);
}

void MainWindow::conectarComponentes()
{
    connect(ui->btAbrirPortaSerial, &QPushButton::clicked, this, &MainWindow::abrirPortaSerial);
    connect(ui->btFecharPortaSerial, &QPushButton::clicked, this, &MainWindow::fecharPortaSerial);

    connect(ui->chkLEDP0, &QCheckBox::clicked, this, &MainWindow::on_chkLEDPi_clicked);
    connect(ui->chkLEDP1, &QCheckBox::clicked, this, &MainWindow::on_chkLEDPi_clicked);
    connect(ui->chkLEDP2, &QCheckBox::clicked, this, &MainWindow::on_chkLEDPi_clicked);
    connect(ui->chkLEDP3, &QCheckBox::clicked, this, &MainWindow::on_chkLEDPi_clicked);
    connect(ui->chkLEDP4, &QCheckBox::clicked, this, &MainWindow::on_chkLEDPi_clicked);
    connect(ui->chkLEDP5, &QCheckBox::clicked, this, &MainWindow::on_chkLEDPi_clicked);
    connect(ui->chkLEDP6, &QCheckBox::clicked, this, &MainWindow::on_chkLEDPi_clicked);
    connect(ui->chkLEDP7, &QCheckBox::clicked, this, &MainWindow::on_chkLEDPi_clicked);
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

    QString lblSliderPosAtual_template("lblSliderPosAtual%1");

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

        QLabel *lblSliderPosAtual = ui->tabPosicoesDasJuntas->findChild<QLabel *>(lblSliderPosAtual_template.arg(junta[i]));

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

        lstPontoVerdeSliderPosAtual.append(lblSliderPosAtual);
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

    filaComandosInicializacao.enqueue("[VELJ0]");
    filaComandosInicializacao.enqueue("[VELJ1]");
    filaComandosInicializacao.enqueue("[VELJ2]");
    filaComandosInicializacao.enqueue("[VELJ3]");
    filaComandosInicializacao.enqueue("[VELJ4]");
    filaComandosInicializacao.enqueue("[VELGR]");

    filaComandosInicializacao.enqueue("[ACLJ00004]");
    filaComandosInicializacao.enqueue("[ACLJ10004]");
    filaComandosInicializacao.enqueue("[ACLJ20004]");
    filaComandosInicializacao.enqueue("[ACLJ30004]");
    filaComandosInicializacao.enqueue("[ACLJ4]");
    filaComandosInicializacao.enqueue("[ACLGR0100]");

    qtdComandosInicializacao = filaComandosInicializacao.count();
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
                if(ui->rdbReadyForPIC->isChecked())
                {
                    setaValorItemTabela(ui->tabelaPosLimites, i, j, QString("%1").arg(tabelaPosLimitesDefault[i][j]));
                }
                else if(ui->rdbMiniMaestro24->isChecked())
                {
                    setaValorItemTabela(ui->tabelaPosLimites, i, j, QString("%1").arg(tabelaPosLimitesDefaultMM24[i][j]));
                }

                setaValorItemTabela(ui->tabelaPosLimitesGraus, i, j, QString("%1").arg(tabelaPosLimitesGrausDefault[i][j]));
            }

            if(tabelaPosLimitesGrausDefault[i][4] == 1.0)
                ui->tabelaPosLimitesGraus->item(i,4)->setCheckState(Qt::CheckState::Checked);
            else
                ui->tabelaPosLimitesGraus->item(i,4)->setCheckState(Qt::CheckState::Unchecked);            
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

            // 1.0 serve para que o resultado seja float ou double
            coeffPontoVerdePropDir[i] = (1.0 * (LBL_POS_ATUAL_X_MAX - LBL_POS_ATUAL_X_MIN))/(tempoPulsoMax[i] - tempoPulsoMin[i]);
            offsetPontoVerdePropDir[i] = LBL_POS_ATUAL_X_MAX - tempoPulsoMax[i] * coeffPontoVerdePropDir[i];

            coeffPontoVerdePropInv[i] = (1.0 * (LBL_POS_ATUAL_X_MIN - LBL_POS_ATUAL_X_MAX))/(tempoPulsoMax[i] - tempoPulsoMin[i]);
            offsetPontoVerdePropInv[i] = LBL_POS_ATUAL_X_MIN - tempoPulsoMax[i] * coeffPontoVerdePropInv[i];
        }
        else
        {
            coeffAng[i] = (angMax[i] - angMin[i]) / (tempoPulsoMax[i] - tempoPulsoMin[i]);
            offsetAng[i] = angMax[i] - tempoPulsoMax[i] * coeffAng[i];

            // 1.0 serve para que o resultado seja float ou double
            coeffPontoVerdePropDir[i] = (1.0 * (LBL_POS_ATUAL_X_MAX - LBL_POS_ATUAL_X_MIN))/(tempoPulsoMax[i] - tempoPulsoMin[i]);
            offsetPontoVerdePropDir[i] = LBL_POS_ATUAL_X_MAX - tempoPulsoMax[i] * coeffPontoVerdePropDir[i];

            coeffPontoVerdePropInv[i] = coeffPontoVerdePropDir[i];
            offsetPontoVerdePropInv[i] = offsetPontoVerdePropDir[i];
        }        

        double anguloNeutro = coeffAng[i] * tempoPulsoNeutro[i] + offsetAng[i];
        anguloNeutro = arredondaPara(anguloNeutro, CASAS_DECIMAIS_POSICAO_ANGULAR);
        setaValorItemTabela(ui->tabelaPosLimitesGraus, i, 2, QString("%1").arg(anguloNeutro));
        double anguloRepouso = coeffAng[i] * tempoPulsoRepouso[i] + offsetAng[i];
        anguloRepouso = arredondaPara(anguloRepouso, CASAS_DECIMAIS_POSICAO_ANGULAR);
        setaValorItemTabela(ui->tabelaPosLimitesGraus, i, 3, QString("%1").arg(anguloRepouso));

        QSlider* slider = lstSlider[i];
        slider->setValue(tempoPulsoRepouso[i]);
        slider->setMaximum(tempoPulsoMax[i]);
        slider->setMinimum(tempoPulsoMin[i]);
        slider->setTickInterval((tempoPulsoMax[i] - tempoPulsoMin[i]) / 4);

        if(ui->rdbReadyForPIC->isChecked())
        {
            qtdPosicoesTmpPulso[i] = (tempoPulsoMax[i] - tempoPulsoMin[i]);  // us

            incrementosAng[i] = (angMax[i] - angMin[i])/qtdPosicoesTmpPulso[i]; // Graus / us
            if(arredondaPara(incrementosAng[i], CASAS_DECIMAIS_POSICAO_ANGULAR) > 0)
                incrementosAng[i] = arredondaPara(incrementosAng[i], CASAS_DECIMAIS_POSICAO_ANGULAR);
            else
                incrementosAng[i] = 0.001;
            incVelGrausPorSeg[i] = incrementosAng[i] / 0.25 * (10e-3);
            if(arredondaPara(incVelGrausPorSeg[i], CASAS_DECIMAIS_VELOCIDADE_ANGULAR) > 0)
                incVelGrausPorSeg[i]  = arredondaPara(incVelGrausPorSeg[i], CASAS_DECIMAIS_VELOCIDADE_ANGULAR);
            else
                incVelGrausPorSeg[i] = 0.001;
            incAclGrausPorSegQuad[i] = incrementosAng[i] / 0.25 * (10e-3) * (80e-3);
            if(arredondaPara(incAclGrausPorSegQuad[i], CASAS_DECIMAIS_ACELERACAO_ANGULAR) > 0)
                incAclGrausPorSegQuad[i] = arredondaPara(incAclGrausPorSegQuad[i], CASAS_DECIMAIS_ACELERACAO_ANGULAR);
            else
                incAclGrausPorSegQuad[i] = 0.001;

            lstSpnAlvoGraus[i]->setDecimals(CASAS_DECIMAIS_POSICAO_ANGULAR);
            lstSpnAlvoGraus[i]->setSingleStep(incrementosAng[i]);
            lstSpnVelGrausPorSeg[i]->setDecimals(CASAS_DECIMAIS_VELOCIDADE_ANGULAR);
            lstSpnVelGrausPorSeg[i]->setSingleStep(incVelGrausPorSeg[i]);
            lstSpnAclGrausPorSegQuad[i]->setDecimals(CASAS_DECIMAIS_ACELERACAO_ANGULAR);
            lstSpnAclGrausPorSegQuad[i]->setSingleStep(incAclGrausPorSegQuad[i]);

            int velTmpPulso;

            if(valoresDefault)
                velTmpPulso = velTmpPulsoDefault[i];
            else
                velTmpPulso = lstSpnVel[i]->value();

            double velGrausPorSeg = velTmpPulso * incrementosAng[i] / 0.25 * (10e-3);  // (0.25us)/(10ms)
            // (0.25us)/(10ms) * Graus/(us) / 0.25
            // (0.25us)/(10ms) * Graus/(0.25us)
            //        1/(10ms) * Graus
            //     Graus /(10ms)
            // Graus / (10e-3 s) * 10e-3
            // Graus / s
            velGrausPorSeg = arredondaPara(velGrausPorSeg, CASAS_DECIMAIS_VELOCIDADE_ANGULAR);
            lstSpnVelGrausPorSeg[i]->setValue(velGrausPorSeg);

            int aclTmpPulso;

            if(valoresDefault)
                aclTmpPulso = aclTmpPulsoDefault[i];
            else
                aclTmpPulso = lstSpnAcl[i]->value();

            double aclGrausPorSegQuad = aclTmpPulso * incrementosAng[i] / 0.25 * (10e-3) * (80e-3); // (0.25us)/(10ms)/(80ms)
            aclGrausPorSegQuad = arredondaPara(aclGrausPorSegQuad,CASAS_DECIMAIS_ACELERACAO_ANGULAR);
            lstSpnAclGrausPorSegQuad[i]->setValue(aclGrausPorSegQuad);
        }
        else if(ui->rdbMiniMaestro24->isChecked())
        {
            qtdPosicoesTmpPulso[i] = (tempoPulsoMax[i] - tempoPulsoMin[i]) * 4; // 0.25us
            incrementosAng[i] = (angMax[i] - angMin[i])/qtdPosicoesTmpPulso[i]; // Graus / 0.25us
            if(arredondaPara(incrementosAng[i], CASAS_DECIMAIS_POSICAO_ANGULAR) > 0)
                incrementosAng[i] = arredondaPara(incrementosAng[i], CASAS_DECIMAIS_POSICAO_ANGULAR);
            else
                incrementosAng[i] = 0.001;
            incVelGrausPorSeg[i] = incrementosAng[i] * (10e-3);
            if(arredondaPara(incVelGrausPorSeg[i], CASAS_DECIMAIS_VELOCIDADE_ANGULAR) > 0)
                incVelGrausPorSeg[i]  = arredondaPara(incVelGrausPorSeg[i], CASAS_DECIMAIS_VELOCIDADE_ANGULAR);
            else
                incVelGrausPorSeg[i] = 0.001;
            incAclGrausPorSegQuad[i] = incrementosAng[i] * (10e-3) * (80e-3);
            if(arredondaPara(incAclGrausPorSegQuad[i], CASAS_DECIMAIS_ACELERACAO_ANGULAR) > 0)
                incAclGrausPorSegQuad[i] = arredondaPara(incAclGrausPorSegQuad[i], CASAS_DECIMAIS_ACELERACAO_ANGULAR);
            else
                incAclGrausPorSegQuad[i] = 0.001;

            lstSpnAlvoGraus[i]->setSingleStep(incrementosAng[i]);
            lstSpnVelGrausPorSeg[i]->setSingleStep(incVelGrausPorSeg[i]);
            lstSpnAclGrausPorSegQuad[i]->setSingleStep(incAclGrausPorSegQuad[i]);

            int velTmpPulso;

            if(valoresDefault)
                velTmpPulso = velTmpPulsoDefault[i];
            else
                velTmpPulso = lstSpnVel[i]->value();

            double velGrausPorSeg = velTmpPulso * incrementosAng[i] * (10e-3);  // (0.25us)/(10ms)
            // (0.25us)/(10ms) * Graus/(0.25us) * 10 * 10e-3
            //      1 / (10ms) * Graus          * 10 * 10e-3
            //       Graus / ((10e-3) * s) * (10e-3)
            //       Graus / s
            velGrausPorSeg = arredondaPara(velGrausPorSeg, CASAS_DECIMAIS_VELOCIDADE_ANGULAR);
            lstSpnVelGrausPorSeg[i]->setValue(velGrausPorSeg);


            int aclTmpPulso;

            if(valoresDefault)
                aclTmpPulso = aclTmpPulsoDefault[i];
            else
                aclTmpPulso = lstSpnAcl[i]->value();

            double aclGrausPorSegQuad = aclTmpPulso * incrementosAng[i] * (10e-3) * (80e-3); // (0.25us)/(10ms)/(80ms)
            // (0.25us)/(10ms)/(80ms) * Graus/(0.25us)
            // 1/(10ms)/(80ms) * Graus/1
            // Graus/(10ms)/(80ms)
            // Graus/((10ms)*(80ms))
            // Graus / (10e-3 * s  * 80e-3 * s) * 10e-3 * 80e-3
            // Graus / s^2
            aclGrausPorSegQuad = arredondaPara(aclGrausPorSegQuad, CASAS_DECIMAIS_ACELERACAO_ANGULAR);
            lstSpnAclGrausPorSegQuad[i]->setValue(aclGrausPorSegQuad);
        }
    }

    on_tabUnidadePos_currentChanged(ui->tabUnidadePos->currentIndex());
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
        bool ehNumeroMaiorQueZero = false;

        lstEdtAtual[i]->text().replace(STR_UND_MICROSSEGUNDOS, "").toInt(&ehNumeroMaiorQueZero);

        if(ehNumeroMaiorQueZero)
        {
            ehNumeroMaiorQueZero = (lstEdtAtual[i]->text().replace(STR_UND_MICROSSEGUNDOS, "").toInt() > 0);
        }

        lstSpnAlvo[i]->setEnabled(estadoHab && ehNumeroMaiorQueZero);
        lstSpnVel[i]->setEnabled(estadoHab);
        lstSpnAcl[i]->setEnabled(estadoHab);

        lstSpnAlvoGraus[i]->setEnabled(estadoHab && ehNumeroMaiorQueZero);
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
    ui->chkParadaTotal->setEnabled(estadoHab);
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
        float posAtual = lstEdtAtual[i]->text().replace(STR_UND_MICROSSEGUNDOS, "").toFloat(&conversaoOk);
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

    if(this->mm24 != nullptr)
        delete(this->mm24);

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
                                         ui->cmbBitsDeDados->itemData(ui->cmbBitsDeDados->currentIndex()).toInt());

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
        if(ui->rdbReadyForPIC->isChecked())
            configuracoesIniciais();
        else if(ui->rdbMiniMaestro24->isChecked())
        {            
            this->mm24 = new MiniMaestro24(serial);
            for(int canal = 0; canal < 5; canal++)
            {
                this->mm24->posicaoRepouso[canal] = static_cast<unsigned int>(tabelaPosLimitesDefaultMM24[canal][3]);
            }

            connect(this->mm24, &MiniMaestro24::positionChanged, this, &MainWindow::positionChangedMiniMaestro24);            
            connect(this->mm24, &MiniMaestro24::fimMovimento, this, &MainWindow::fimMovimentoMiniMaestro24);
            connect(this->mm24, &MiniMaestro24::semMovimento, this, &MainWindow::semMovimentoMiniMaestro24);
            connect(this->mm24, &MiniMaestro24::setouPosicaoAlvo, this, &MainWindow::setouPosicaoAlvoMiniMaestro24);
            connect(this->mm24, &MiniMaestro24::setouVelocidade, this, &MainWindow::setouVelocidadeMiniMaestro24);
            connect(this->mm24, &MiniMaestro24::setouAceleracao, this, &MainWindow::setouAceleracaoMiniMaestro24);
            connect(this->mm24, &MiniMaestro24::respostaGetErrors, this, &MainWindow::respostaGetErrorsMiniMaestro24);


            configurarConversaoEntreMicrossegundosEAngulos(true);

            for(char canal = 0; canal < 4; canal++)
            {
                this->mm24->SetAcceleration(canal, 0x0004);
            }

            this->mm24->SetAcceleration(0x05, 0x0000);

            this->mm24->timerMonitoramentoPosicao->start();

            this->mm24->inicializacaoSolicitada = true; // Para atualizar os campos de posição alvo

            habilitarComponentesConn(true);
            habilitaBotoesExecComandos(true);
            habilitaCamposAbaPosicaoAlvo(0, true);
            habilitaCamposAbaPosicaoAlvo(1, true);                
        }

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
    ui->chkEnviaComandoImediato->setChecked(false);

    if(this->mm24 != nullptr)
    {
        this->mm24->disconnect();
        delete(this->mm24);
        this->mm24 = nullptr;
    }

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
        recebeBytesMiniMaestro24(data);
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

    return arredondaPara(y, CASAS_DECIMAIS_POSICAO_ANGULAR);
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

    return arredondaPara(velGrausPorSeg, CASAS_DECIMAIS_VELOCIDADE_ANGULAR);

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

    return arredondaPara(aclGrausPorSegQuad, CASAS_DECIMAIS_ACELERACAO_ANGULAR);
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

bool MainWindow::doesContain(QListView *listView, QString expression)
{
    QAbstractItemModel* model = listView->model() ;
    int rowCount = model->rowCount();
    int columnCount = model->columnCount();

    for(int i = 0; i < rowCount; i++)
        for(int j = 0; j < columnCount; j++)
            if(model->index(i, j).data(Qt::DisplayRole).toString().contains(expression))
                return true;

    return false;
}

void MainWindow::mostrarAbaTerminal(QRadioButton *rdbPlaca)
{
    if(rdbPlaca == ui->rdbReadyForPIC)
    {
        ui->tabPrincipal->removeTab(idxTerminal);
        ui->tabPrincipal->insertTab(idxTerminal, tabTerminalReadyForPIC, tabTerminalReadyForPICLabel);
    }
    else if(rdbPlaca == ui->rdbMiniMaestro24)
    {
        ui->tabPrincipal->removeTab(idxTerminal);
        ui->tabPrincipal->insertTab(idxTerminal, tabTerminalMiniMaestro, tabTerminalMiniMaestroLabel);
    }
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
        if(!EhIgual(angulo, anguloAtual, CASAS_DECIMAIS_POSICAO_ANGULAR))
            lstSpnAlvoGraus[idxJunta]->setValue(angulo);        
    }
    else
    {
        lstChkHab[idxJunta]->setChecked(false);
        lstSpnAlvo[idxJunta]->setEnabled(false);
        lstSpnAlvoGraus[idxJunta]->setEnabled(false);
        lstSlider[idxJunta]->setEnabled(false);
    }
}

void MainWindow::converteSpnVelParaGrausPorSeg(int idxJunta, int velocidade)
{
    double velocidadeAngular = converteVelTmpPulsoParaGrausPorSeg(idxJunta, velocidade);
    double velocidadeAngularAtual = lstSpnVelGrausPorSeg[idxJunta]->value();

    // Esta verificação é feita para que não se ative o evento valueChanged desnecessariamente
    if(!EhIgual(velocidadeAngular, velocidadeAngularAtual, CASAS_DECIMAIS_VELOCIDADE_ANGULAR))
        lstSpnVelGrausPorSeg[idxJunta]->setValue(velocidadeAngular);
}

void MainWindow::converteSpnAclParaGrausPorSegQuad(int idxJunta, int aceleracao)
{
    double aceleracaoAngular = converteAclTmpPulsoParaGrausPorSegQuad(idxJunta, aceleracao);
    double aceleracaoAngularAtual = lstSpnAclGrausPorSegQuad[idxJunta]->value();

    // Esta verificação é feita para que não se ative o evento valueChanged desnecessariamente
    if(!EhIgual(aceleracaoAngular, aceleracaoAngularAtual, CASAS_DECIMAIS_ACELERACAO_ANGULAR))
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

void MainWindow::setaPosicaoPontoVerde(int idxJunta, int posicao)
{    
    int x, y, width, height;
    double xf;

    if(coeffPontoVerde != nullptr && offsetPontoVerde != nullptr)
    {
        QRect geometry = lstPontoVerdeSliderPosAtual[idxJunta]->geometry();

        geometry.getRect(&x, &y, &width, &height);

        xf = coeffPontoVerde[idxJunta] * posicao + offsetPontoVerde[idxJunta];

        x = qRound(xf);

        geometry.setRect(x, y, width, height);

        lstPontoVerdeSliderPosAtual[idxJunta]->setGeometry(geometry);
    }
}

void MainWindow::montaJSTParaPararMov1Junta()
{    
    comandoJSTParaPararMov = "[JST";
    for(int i = 0; i < QTD_SERVOS; i++)
    {
        int posAtual = lstEdtAtual[i]->text().replace(STR_UND_MICROSSEGUNDOS, "").toInt();
        comandoJSTParaPararMov += QString("%1%2").arg(idJST[i]).arg(posAtual, 4, 10, QChar('0'));
    }
    comandoJSTParaPararMov += "]";
}

void MainWindow::decodificaResposta()
{
    QString resposta = filaBufferEntrada.dequeue();
    int tamanhoResposta = resposta.length();
    bool ehRespostaFinal = false;
    bool ehRespostaConfigPosLimite = false;
    bool ehRespostaMovAbaComandos = false;
    bool ehRespostaMoverComVelAcl = false;
    double posicoesAtuaisGraus[6];

    if(tamanhoResposta == 35 && (resposta.contains("MOV") || resposta.contains("JST") || resposta.contains("RPS") || resposta.contains("IN2")) )
    {
        QString strValor;
        int valor;
        double graus;
        bool todasPosicoesMaioresQueZero = true;
        double *posGarra = nullptr;

        QString comandoRecebido = resposta.mid(1,3);
        comandoJSTParaPararMov = QString(resposta);
        comandoJSTParaPararMov.replace(comandoRecebido, "JST");

        for(int i = 0; i < QTD_SERVOS; i++)
        {
            strValor = resposta.mid(5*(i+1), 4);
            valor = strValor.toInt();            

            lstEdtAtual[i]->setText(strValor.setNum(valor) + STR_UND_MICROSSEGUNDOS);

            if(valor > 0)
            {
                graus = converteMicrossegundosParaGraus(i, valor);                
                posicoesAtuaisGraus[i] = graus;
                lstEdtAtualGraus[i]->setText(QString("%L1").arg(graus, 0, 'f', CASAS_DECIMAIS_POSICAO_ANGULAR) + STR_UND_GRAUS);
                lstPontoVerdeSliderPosAtual[i]->setVisible(true);
                setaPosicaoPontoVerde(i, valor);                
            }
            else
            {
                lstPontoVerdeSliderPosAtual[i]->setVisible(false);                
                posicoesAtuaisGraus[i] = -9999;
                if(i < 5)
                    todasPosicoesMaioresQueZero = false;
            }
        }

        if(todasPosicoesMaioresQueZero)
        {
            posGarra = preencheCamposXYZAtual(posicoesAtuaisGraus);
        }

        //if(resposta.contains("JST") || resposta.contains("RPS") || resposta.contains("IN2"))
        if(!resposta.contains("MOV"))
        {
            for(int i = 0; i < QTD_SERVOS; i++)
            {
                valor = resposta.mid(5*(i+1), 4).toInt();

                if(valor > 0)
                {
                    if(valor != lstSpnAlvo[i]->value())
                    {
                        if(!ui->chkEnviaComandoImediato->isChecked() || countAbaPosicoesValueChanged <= 0)
                        {
                            countAbaPosicoesValueChanged = -1;
                            lstSpnAlvo[i]->setValue(valor);

                            if(resposta.contains("IN2"))
                            {
                                habilitaCamposAbaPosicaoAlvoJunta(0, i, false);
                                habilitaCamposAbaPosicaoAlvoJunta(1, i, false);
                            }
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
                    lstSlider[i]->setEnabled(false);
                }                                
            }

            if(todasPosicoesMaioresQueZero)
            {
                preencheCamposXYZAlvo(posGarra);
                delete(posGarra);
            }

            HabilitarComponentesComServosLigados();

            ehRespostaFinal = true;
            ehRespostaMoverComVelAcl = resposta.contains("JST");
            ehRespostaMovAbaComandos = resposta.contains("[JSTA0000B0000C0000D0000E0000G0000]") ||
                                       resposta.contains("RPS");
        }
        else
        {
            delete(posGarra);
        }
    }
    else if(tamanhoResposta == 11)
    {
        if(resposta.contains("MOV") || resposta.contains("CTZ") || resposta.contains("IN1"))
        {
            bool todasPosicoesMaioresQueZero = true;
            double *posGarra = nullptr;
            QString junta = resposta.mid(4,2);
            QString strValor = resposta.mid(6,4);

            int i;
            int valor;

            if(junta.contains("J"))
                i = junta.at(1).toLatin1() - '0';
            else
                i = 5;

            valor = strValor.toInt();

            lstEdtAtual[i]->setText(strValor.setNum(valor) + STR_UND_MICROSSEGUNDOS);

            montaJSTParaPararMov1Junta();

            if(valor > 0)
            {
                double graus = converteMicrossegundosParaGraus(i, valor);
                lstEdtAtualGraus[i]->setText(QString("%L1").arg(graus, 0, 'f', CASAS_DECIMAIS_POSICAO_ANGULAR) + STR_UND_GRAUS);
                lstPontoVerdeSliderPosAtual[i]->setVisible(true);
                setaPosicaoPontoVerde(i, valor);                
            }
            else
            {
                lstPontoVerdeSliderPosAtual[i]->setVisible(false);
            }

            for(int k = 0; k < QTD_SERVOS && todasPosicoesMaioresQueZero; k++)
            {
                posicoesAtuaisGraus[k] = lstEdtAtualGraus[k]->text().replace(STR_UND_GRAUS, "").replace(",", ".").toDouble();
                if(k < 5 && lstEdtAtual[k]->text().replace(STR_UND_MICROSSEGUNDOS, "").toInt() == 0)
                    todasPosicoesMaioresQueZero = false;
            }

            if(todasPosicoesMaioresQueZero)
            {
                posGarra = preencheCamposXYZAtual(posicoesAtuaisGraus);
            }

            //if(resposta.contains("CTZ") || resposta.contains("IN1"))
            if(!resposta.contains("MOV"))
            {
                if(valor > 0)
                {
                    if(valor != lstSpnAlvo[i]->value())
                    {
                        if(!ui->chkEnviaComandoImediato->isChecked() || countAbaPosicoesValueChanged <= 0)
                        {
                            countAbaPosicoesValueChanged = -1;
                            lstSpnAlvo[i]->setValue(valor);

                            if(resposta.contains("IN1"))
                            {
                                habilitaCamposAbaPosicaoAlvoJunta(0, i, false);
                                habilitaCamposAbaPosicaoAlvoJunta(1, i, false);
                            }
                        }
                        else
                        {
                            countAbaPosicoesValueChanged--;
                        }
                    }

                    if(!this->lstSpnAlvo[i]->isEnabled() || !this->lstSpnAlvoGraus[i]->isEnabled())
                    {
                        this->lstChkHab[i]->setChecked(true);
                        this->lstSpnAlvo[i]->setEnabled(true);
                        this->lstSpnAlvoGraus[i]->setEnabled(true);
                    }
                }
                else
                {
                    this->lstChkHab[i]->setChecked(false);
                    this->lstSpnAlvo[i]->setEnabled(false);
                    this->lstSpnAlvoGraus[i]->setEnabled(false);
                }

                if(todasPosicoesMaioresQueZero)
                {
                    preencheCamposXYZAlvo(posGarra);
                    delete(posGarra);
                }

                HabilitarComponentesComServosLigados();
                ehRespostaFinal = true;
                ehRespostaMovAbaComandos = resposta.contains("CTZ");
            }
            else
            {
                if(todasPosicoesMaioresQueZero)
                    delete(posGarra);
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
    else if(resposta.length() == 8 && (resposta.contains("GA") || resposta.contains("GF")))
    {
        QString valor = resposta.mid(3,4);
        valor.setNum(valor.toInt());

        ui->edtGRAtual->setText(valor);
        double graus = converteMicrossegundosParaGraus(5, valor.toInt());
        ui->edtGRAtualGraus->setText(QString("%L1").arg(graus, 0, 'f', CASAS_DECIMAIS_POSICAO_ANGULAR));

        lstPontoVerdeSliderPosAtual[5]->setVisible(true);
        setaPosicaoPontoVerde(5, valor.toInt());

        ui->spnGRAlvo->setValue(valor.toInt());

        montaJSTParaPararMov1Junta();

        if(!ui->spnGRAlvo->isEnabled())
        {
            ui->spnGRAlvo->setEnabled(true);
            ui->chkGR->setChecked(true);
            ui->spnGRAlvoGraus->setEnabled(true);
        }

        ehRespostaFinal = true;
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

    /* Envio de comandos das filas após a decodificação da resposta */
    if(filaComandosInicializacao.count() > 0)
    {
        if(resposta.contains("TRPGR"))
            configurarConversaoEntreMicrossegundosEAngulos();

        double porcentagem = round((100.0 * (qtdComandosInicializacao - filaComandosInicializacao.count())) / qtdComandosInicializacao);

        QString strPorcentagem = QString("%L1%").arg(porcentagem);

        showStatusMessage("Configurando placa de controle ("+strPorcentagem+")");
        enviaComando(filaComandosInicializacao.dequeue());
    }
    else if(!(resposta.contains("ECH") || resposta.contains("PRONTO")))
    {
        if (inicializando && serial->isOpen())
        {
            habilitarComponentesConn(true);
            showStatusMessage("Conectado à porta " + ui->cmbPortaSerial->currentData().toString());
        }
        inicializando = false;

        if(paradaDeSequenciaSolicitada && ui->chkParadaTotal->isChecked())
        {
            if(filaComandosParaPararMov.count() > 0)
            {
                parser(filaComandosParaPararMov.dequeue());
            }
            else if(!foiEnviadoJSTParaPararMov)
            {
                parser(comandoJSTParaPararMov);
                //paradaDeSequenciaSolicitada = false;
                foiEnviadoJSTParaPararMov = true;
                comandoParaPararMovEnviado = comandoJSTParaPararMov;
            }
            else if (resposta.contains(comandoParaPararMovEnviado))
            {
                int valorVelAnterior;
                QString comandoVEL;
                filaComandosMoverComVelAcl.clear();
                for(int i = 0; i < QTD_SERVOS; i++)
                {
                    valorVelAnterior = velocidadesAnterioresAAParada[i];
                    comandoVEL = QString("[VEL%1%2]").arg(junta[i]).arg(valorVelAnterior, 4, 10, QChar('0'));
                    filaComandosMoverComVelAcl.enqueue(comandoVEL);
                }
                parser(filaComandosMoverComVelAcl.dequeue());
            }
            else if(resposta.contains("JST"))
            {
                parser(resposta);
                comandoParaPararMovEnviado = resposta;
            }
            else if(resposta.contains("RPS"))
            {
                parser("[RPS]");
                comandoParaPararMovEnviado = "RPS";
            }
            else if(resposta.contains("CTZ"))
            {
                QString juntaCTZ = resposta.mid(4,2);
                QString comandoCTZ = QString("[CTZ%1]").arg(juntaCTZ);
                parser(comandoCTZ);
                comandoParaPararMovEnviado = resposta;
            }
            else if (resposta.contains("GA"))
            {
                parser("[GA]");
                comandoParaPararMovEnviado = "GA";
            }
            else if(resposta.contains("GF"))
            {
                parser("[GF]");
                comandoParaPararMovEnviado = "GF";
            }
            else if(filaComandosMoverComVelAcl.count() > 0 && ehRespostaMoverComVelAcl)
            {
                QString comando = filaComandosMoverComVelAcl.dequeue();

                if(filaComandosMoverComVelAcl.count() <= 0)
                    ultimoVELcomVelocidadeAnterior = comando;

                parser(comando);
            }
            else if(ultimoVELcomVelocidadeAnterior.size() > 0 &&
                    resposta.contains(ultimoVELcomVelocidadeAnterior))
            {
                ultimoVELcomVelocidadeAnterior = "";
                paradaDeSequenciaSolicitada = false;
                // TODO: Aba Sequência de Comandos: Reimplementar a parada total com o novo comando de parada total implementado na placa de controle
                ui->chkParadaTotal->setEnabled(true);
                ui->btPararSeqComandos->setEnabled(true);
                foiEnviadoJSTParaPararMov = false;
            }
        }
        else if(seqEmExecucao && ehRespostaFinal)
        {
            executaComandoDaSequencia();
        }
        else if(filaComandosMovimentacaoAbaComandos.count() > 0 && ehRespostaMovAbaComandos)
        {
            parser(filaComandosMovimentacaoAbaComandos.dequeue());
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
    }
}

void MainWindow::recebeBytesMiniMaestro24(QByteArray data)
{    
    char rChar;

    comando_mm cmd;    

    if(mm24->filaComMonitoramentoEnviados.isEmpty())
        return;    

    cmd = mm24->filaComMonitoramentoEnviados.dequeue();    

    int qtd_bytes = 0;

    for(int i = 0; i < data.length(); i++)
    {
        rChar = data.at(i);
        cmd.resposta[qtd_bytes++] = rChar;

        if(qtd_bytes == cmd.qtdBytesAReceber)
        {
            mm24->filaComRespostaRecebida.enqueue(cmd);

            mm24->decodificaResposta();

            if(!mm24->filaComMonitoramentoEnviados.isEmpty())
            {
                cmd = mm24->filaComMonitoramentoEnviados.dequeue();
                qtd_bytes = 0;
            }
        }
    }
    // TODO: Mini Maestro 24: Avaliar se é adequado esta chamada do HabilitarComponentesComServosLigados() estar aqui;
    HabilitarComponentesComServosLigados();
}

void MainWindow::positionChangedMiniMaestro24(unsigned int posicao[])
{    
    QString strValor;
    int valor;
    double graus;
    double posicoesAtuaisGraus[6];
    bool todasPosicoesMaioresQueZero = false;
    double *posGarra = nullptr;

    for(int i = 0; i < QTD_SERVOS; i++)
    {
        valor = int(posicao[i]);

        lstEdtAtual[i]->setText(strValor.setNum(valor) + STR_UND_MICROSSEGUNDOS);

        if(valor > 0)
        {
            graus = converteMicrossegundosParaGraus(i, valor);
            posicoesAtuaisGraus[i] = graus;
            lstEdtAtualGraus[i]->setText(QString("%L1").arg(graus, 0, 'f', CASAS_DECIMAIS_POSICAO_ANGULAR) + STR_UND_GRAUS);
            lstPontoVerdeSliderPosAtual[i]->setVisible(true);
            setaPosicaoPontoVerde(i, valor);
        }
        else
        {
            lstPontoVerdeSliderPosAtual[i]->setVisible(false);
            posicoesAtuaisGraus[i] = -9999;
            if(i < 5)
                todasPosicoesMaioresQueZero = false;
        }
    }

    if(todasPosicoesMaioresQueZero)
    {
        posGarra = preencheCamposXYZAtual(posicoesAtuaisGraus);
        delete posGarra;
    }
}

void MainWindow::fimMovimentoMiniMaestro24(unsigned int posicao[])
{    
    QString strValor;
    int valor;
    double graus;
    double posicoesAtuaisGraus[6];
    bool todasPosicoesMaioresQueZero = false;
    double *posGarra = nullptr;

    for(int i = 0; i < QTD_SERVOS; i++)
    {
        valor = int(posicao[i]);

        lstEdtAtual[i]->setText(strValor.setNum(valor) + STR_UND_MICROSSEGUNDOS);

        if(valor > 0)
        {
            graus = converteMicrossegundosParaGraus(i, valor);
            posicoesAtuaisGraus[i] = graus;
            lstEdtAtualGraus[i]->setText(QString("%L1").arg(graus, 0, 'f', CASAS_DECIMAIS_POSICAO_ANGULAR) + STR_UND_GRAUS);
            lstPontoVerdeSliderPosAtual[i]->setVisible(true);
            setaPosicaoPontoVerde(i, valor);
        }
        else
        {
            lstPontoVerdeSliderPosAtual[i]->setVisible(false);
            posicoesAtuaisGraus[i] = -9999;
            if(i < 5)
                todasPosicoesMaioresQueZero = false;
        }
    }

    if(todasPosicoesMaioresQueZero)
    {
        posGarra = preencheCamposXYZAtual(posicoesAtuaisGraus);
    }

    for(int i = 0; i < QTD_SERVOS; i++)
    {
        valor = int(posicao[i]);

        if(valor > 0)
        {
            if(valor != lstSpnAlvo[i]->value())
            {
                if(!ui->chkEnviaComandoImediato->isChecked() || countAbaPosicoesValueChanged <= 0)
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
                this->lstSpnAlvo[i]->setEnabled(true);
                this->lstSpnAlvoGraus[i]->setEnabled(true);
                this->lstSlider[i]->setEnabled(true);
            }
        }
        else
        {
            this->lstChkHab[i]->setChecked(false);
            this->lstSpnAlvo[i]->setEnabled(false);
            this->lstSpnAlvoGraus[i]->setEnabled(false);
            this->lstSlider[i]->setEnabled(false);
        }
    }

    if(todasPosicoesMaioresQueZero)
    {
        preencheCamposXYZAlvo(posGarra);
        delete(posGarra);
    }

    //HabilitarComponentesComServosLigados();
}

void MainWindow::semMovimentoMiniMaestro24(unsigned int posicao[])
{
    Q_UNUSED(posicao);

    if(seqEmExecucao && !timerDLY->isActive() && !emDLYSemParam)
    {
        executaComandoDaSequenciaMM24();
    }
    else if(mm24->paradaTotalSolicitada)
    {
        mm24->paradaTotalSolicitada = false;

        for(char canal = 0; canal < 6; canal++)
        {
            mm24->SetSpeed(canal, uint16_t(velocidadesAnterioresAAParada[int(canal)]));
        }

        ui->chkParadaTotal->setEnabled(true);
        ui->btPararSeqComandos->setEnabled(true);
    }
}

void MainWindow::setouPosicaoAlvoMiniMaestro24(int canal, unsigned int valor)
{
    if(valor > 0)
    {
        lstSpnAlvo[canal]->setValue(int(valor));
        lstChkHab[canal]->setChecked(true);
        lstSlider[canal]->setEnabled(true);
    }
    else
    {
        lstSpnAlvo[canal]->setEnabled(false);
        lstChkHab[canal]->setChecked(false);
        lstSlider[canal]->setEnabled(false);
    }
}

void MainWindow::setouVelocidadeMiniMaestro24(int canal, unsigned int valor)
{
    lstSpnVel[canal]->setValue(int(valor));
}

void MainWindow::setouAceleracaoMiniMaestro24(int canal, unsigned int valor)
{
    lstSpnAcl[canal]->setValue(int(valor));
}

void MainWindow::respostaGetErrorsMiniMaestro24(unsigned char bytesErro[])
{
    uint16_t mascaraErro = 0x0001;
    uint16_t bytesErro16 = bytesErro[0] + 256 * bytesErro[1];
    uint16_t byteBitSetado;

    int idxErro;

    if(bytesErro16 > 0)
    {
        ui->lblCodigoErroMM24->setText(QString("0x%1").arg(bytesErro16, 4, 16, QLatin1Char('0')).toUpper());

        ui->listErrosAtivos->clear();

        for(int i = 0; i < 9; i++)
        {
            byteBitSetado = bytesErro16 & mascaraErro; // Deve resultar em uma potência de 2

            if(byteBitSetado > 0)
            {
                idxErro = int(log2(byteBitSetado));

                QString descricaoErro = descricaoBitErro[idxErro];

                if(!doesContain(ui->listErrosAtivos, descricaoErro))
                {
                    ui->listErrosAtivos->addItem(descricaoErro);
                }
            }

            mascaraErro = mascaraErro * 2;
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
    if(tableWidget->item(idxLinha, idxColuna) == nullptr)
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
            habilitaBotoesExecComandos(true);
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
    filaComandosMovimentacaoAbaComandos.clear();

    filaComandosMovimentacaoAbaComandos.enqueue("[CTZJ0]");
    filaComandosMovimentacaoAbaComandos.enqueue("[CTZJ1]");
    filaComandosMovimentacaoAbaComandos.enqueue("[CTZJ2]");
    filaComandosMovimentacaoAbaComandos.enqueue("[CTZJ3]");
    filaComandosMovimentacaoAbaComandos.enqueue("[CTZJ4]");
    filaComandosMovimentacaoAbaComandos.enqueue("[CTZGR]");

    parser(filaComandosMovimentacaoAbaComandos.dequeue());
}

void MainWindow::desligaServos()
{
    filaComandosMovimentacaoAbaComandos.clear();
    filaComandosMovimentacaoAbaComandos.enqueue("[RPS]");
    filaComandosMovimentacaoAbaComandos.enqueue("[JSTA0000B0000C0000D0000E0000G0000]");

    parser(filaComandosMovimentacaoAbaComandos.dequeue());
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
        abrirGarraMiniMaestro24();
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
        garraSemiabertaMiniMaestro24();
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
        fecharGarraMiniMaestro24();
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
        giroGarraMais90MiniMaestro24();
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
        garraPosNeutraMiniMaestro24();
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
        giroGarraMenos90MiniMaestro24();
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
        posicaoDeRepousoMiniMaestro24();
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
        posicaoNeutraJSTMiniMaestro24();
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
        desligaServosMiniMaestro24();
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
        posicaoNeutraCTZMiniMaestro24();
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
void MainWindow::avisoColisaoBaseFixa(bool posicaoProjetada)
{
    QString strCompl("");

    if(posicaoProjetada)
    {
        strCompl = ",\nmesmo com a posição projetada no plano que corta verticalmente o braço robô";
    }
    QString mensagem = "Posição XYZ alvo faz a garra colidir com a base fixa"+strCompl+".\n"+
                       "Nenhuma alteração será feita na posição alvo.";

    QMessageBox::warning(this,
                         tr("Colisão com base fixa"),
                         mensagem,
                         QMessageBox::Ok,
                         QMessageBox::Ok);
}

void MainWindow::avisoColisaoBaseGiratoria(bool posicaoProjetada)
{
    QString strCompl("");

    if(posicaoProjetada)
    {
        strCompl = ",\nmesmo com a posição projetada no plano que corta verticalmente o braço robô";
    }
    QString mensagem = "Posição XYZ alvo faz a garra colidir com a base giratória"+strCompl+".\n"+
                       "Nenhuma alteração será feita na posição alvo.";

    QMessageBox::warning(this,
                         tr("Colisão com base giratória"),
                         mensagem,
                         QMessageBox::Ok,
                         QMessageBox::Ok);
}

void MainWindow::avisoColisaoSegmentoL1(bool posicaoProjetada)
{
    QString strCompl("");

    if(posicaoProjetada)
    {
        strCompl = ",\nmesmo com a posição projetada no plano que corta verticalmente o braço robô";
    }
    QString mensagem = "Posição XYZ alvo faz a garra colidir com o segmento L1"+strCompl+".\n"+
                       "Nenhuma alteração será feita na posição alvo.";

    QMessageBox::warning(this,
                         tr("Colisão com o segmento L1"),
                         mensagem,
                         QMessageBox::Ok,
                         QMessageBox::Ok);
}

void MainWindow::on_btMover_clicked()
{
    on_btPararSeqComandos_clicked();
    double teta1 = lstSpnAlvoGraus[0]->value();
    double teta2 = lstSpnAlvoGraus[1]->value();
    double teta3 = lstSpnAlvoGraus[2]->value();
    double teta4 = lstSpnAlvoGraus[3]->value();
    double teta5 = lstSpnAlvoGraus[4]->value();

    bool colideComBaseFixa, colideComBaseGir, colideComSegmentoL1;

    double *posGarra = cinematica.posicaoGarra(teta1, teta2, teta3, teta4, teta5,
                                               &colideComBaseFixa,
                                               &colideComBaseGir,
                                               &colideComSegmentoL1);
    delete posGarra;

    if(!(colideComBaseFixa || colideComBaseGir || colideComSegmentoL1))
    {
        if(ui->rdbReadyForPIC->isChecked())
            comandoJST();
        else if(ui->rdbMiniMaestro24->isChecked())
        {            
            uint16_t posicaoAlvo[QTD_SERVOS];

            for(int i = 0; i < QTD_SERVOS; i++)
            {
                posicaoAlvo[i] = static_cast<uint16_t>(lstSpnAlvo[i]->value());
            }
            this->mm24->SetMultipleTargets(QTD_SERVOS, 0, posicaoAlvo);
        }
    }
    else
    {
        if(colideComBaseFixa)
            avisoColisaoBaseFixa();
        else if(colideComBaseGir)
            avisoColisaoBaseGiratoria();
        else if(colideComSegmentoL1)
            avisoColisaoSegmentoL1();

        bool ehNumeroMaiorQueZero;

        for(int idxJunta = 0; idxJunta < 6; idxJunta++)
        {
            int posCorrente = lstEdtAtual[idxJunta]->text().replace(STR_UND_MICROSSEGUNDOS, "").toInt(&ehNumeroMaiorQueZero);
            if(ehNumeroMaiorQueZero)
            {
                lstSpnAlvo[idxJunta]->setValue(posCorrente);
            }
        }
    }
}

void MainWindow::on_btMoverComVelEAcl_clicked()
{    
    on_btPararSeqComandos_clicked();

    double teta1 = lstSpnAlvoGraus[0]->value();
    double teta2 = lstSpnAlvoGraus[1]->value();
    double teta3 = lstSpnAlvoGraus[2]->value();
    double teta4 = lstSpnAlvoGraus[3]->value();
    double teta5 = lstSpnAlvoGraus[4]->value();

    bool colideComBaseFixa, colideComBaseGir, colideComSegmentoL1;

    double *posGarra = cinematica.posicaoGarra(teta1, teta2, teta3, teta4, teta5,
                                               &colideComBaseFixa,
                                               &colideComBaseGir,
                                               &colideComSegmentoL1);
    delete posGarra;

    if(!(colideComBaseFixa || colideComBaseGir || colideComSegmentoL1))
    {
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

                if(lstChkHab[i]->isChecked())
                    valor = lstSpnAlvo[i]->value();
                else
                    valor = 0;

                comandoJST += idJST[i] + QString("%1").arg(valor, 4, 10, QChar('0'));
            }

            comandoJST += "]";

            filaComandosMoverComVelAcl.enqueue(comandoJST);

            enviaComando(filaComandosMoverComVelAcl.dequeue());
        }
        else if(ui->rdbMiniMaestro24->isChecked())
        {
            uint16_t targets[6];

            for(int i = 0; i < QTD_SERVOS; i++)
            {
                  uint16_t speed = static_cast<uint16_t>(lstSpnVel[i]->value());
                  this->mm24->SetSpeed(char(i), speed);

                  uint16_t accel = static_cast<uint16_t>(lstSpnAcl[i]->value());
                  this->mm24->SetAcceleration(char(i), accel);

                  if(lstChkHab[i]->isChecked())
                    targets[i] = static_cast<uint16_t>(lstSpnAlvo[i]->value());
                  else
                    targets[i] = 0;
            }

            this->mm24->SetMultipleTargets(QTD_SERVOS, 0, targets);
        }
    }
    else
    {
        if(colideComBaseFixa)
            avisoColisaoBaseFixa();
        else if(colideComBaseGir)
            avisoColisaoBaseGiratoria();
        else if(colideComSegmentoL1)
            avisoColisaoSegmentoL1();

        bool ehNumeroMaiorQueZero;

        for(int idxJunta = 0; idxJunta < 6; idxJunta++)
        {
            int posCorrente = lstEdtAtual[idxJunta]->text().replace(STR_UND_MICROSSEGUNDOS, "").toInt(&ehNumeroMaiorQueZero);
            if(ehNumeroMaiorQueZero)
            {
                lstSpnAlvo[idxJunta]->setValue(posCorrente);
            }
        }
    }

}

void MainWindow::on_btCalcularXYZAlvo_clicked()
{
    bool colideComBaseFixa, colideComBaseGir, colideComSegmentoL1;

    double teta1 = lstSpnAlvoGraus[0]->value();
    double teta2 = lstSpnAlvoGraus[1]->value();
    double teta3 = lstSpnAlvoGraus[2]->value();
    double teta4 = lstSpnAlvoGraus[3]->value();
    double teta5 = lstSpnAlvoGraus[4]->value();

    double *posGarra = cinematica.posicaoGarra(teta1, teta2, teta3, teta4, teta5,
                                               &colideComBaseFixa,
                                               &colideComBaseGir,
                                               &colideComSegmentoL1);

    if(!(colideComBaseFixa || colideComBaseGir || colideComSegmentoL1))
        preencheCamposXYZAlvo(posGarra);
    else
    {
        if(colideComBaseFixa)
            avisoColisaoBaseFixa();
        else if(colideComBaseGir)
            avisoColisaoBaseGiratoria();
        else if(colideComSegmentoL1)
            avisoColisaoSegmentoL1();
    }
    delete(posGarra);
}

int MainWindow::caixaDialogoPerguntaSimNao(const QString &titulo, const QString &texto)
{
    QMessageBox msgPerguntaPosProjetada;

    msgPerguntaPosProjetada.setParent(this);
    msgPerguntaPosProjetada.setWindowTitle(titulo);
    msgPerguntaPosProjetada.setText(texto);
    QPushButton* btSim = msgPerguntaPosProjetada.addButton(tr("Sim"), QMessageBox::YesRole);
    QPushButton* btNao = msgPerguntaPosProjetada.addButton(tr("Não"), QMessageBox::NoRole);

    msgPerguntaPosProjetada.setDefaultButton(btNao);
    msgPerguntaPosProjetada.setEscapeButton(btNao);
    msgPerguntaPosProjetada.setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
    msgPerguntaPosProjetada.setIcon(QMessageBox::Question);

    msgPerguntaPosProjetada.exec();

    QAbstractButton* btClicado = msgPerguntaPosProjetada.clickedButton();

    if(btClicado == btSim)
    {
        return QMessageBox::Yes;
    }

    return QMessageBox::No;
}

void MainWindow::on_btCalcularAngulosAlvo_clicked()
{
    bool posicaoProjetada;
    bool posicaoAtingivel;
    bool colideComBaseFixa;
    bool colideComBaseGir;
    bool colideComSegmentoL1;
    int respostaPosInatingivel = QMessageBox::No;
    int respostaPosProjetada = QMessageBox::No;

    calculoAngulosAlvoAcionado = true;

    double x = ui->spnPosXAlvo->value();
    double y = ui->spnPosYAlvo->value();
    double z = ui->spnPosZAlvo->value();
    double Rx = ui->spnRxAlvo->value();
    double Ry = ui->spnRyAlvo->value();
    double Rz = ui->spnRzAlvo->value();

    double angulosCorrentes[QTD_SERVOS - 1];

    for(int i = 0; i < QTD_SERVOS - 1; i++)
    {
        angulosCorrentes[i] = lstEdtAtualGraus[i]->text().replace(STR_UND_GRAUS, "").replace(",",".").toDouble();
    }

    double *angulosJuntas = cinematica.angJuntas(&x, &y, &z,
                                                 &Rx, &Ry, &Rz,
                                                 angulosCorrentes,
                                                 angMax, angMin,
                                                 &posicaoProjetada, &posicaoAtingivel,
                                                 &colideComBaseFixa, &colideComBaseGir, &colideComSegmentoL1);

    if(posicaoProjetada && posicaoAtingivel)
    {
        respostaPosProjetada = caixaDialogoPerguntaSimNao(tr("Posição projetada"),
                                                          tr("Posição XYZ foi projetada no plano que corta verticalmente\n"
                                                             "o braço robô.\n"
                                                             "Deseja atualizar a posição XYZ alvo com os valores projetados?\n"
                                                             "Se clicar em não, os ângulos alvo não serão alterados."));

        respostaPosInatingivel = respostaPosProjetada;
    }
    else if(colideComBaseFixa)
    {
        avisoColisaoBaseFixa(posicaoProjetada);

        respostaPosInatingivel = QMessageBox::No;
        respostaPosProjetada = respostaPosInatingivel;
        posicaoAtingivel = false;
    }
    else if(colideComBaseGir)
    {
        avisoColisaoBaseGiratoria(posicaoProjetada);

        respostaPosInatingivel = QMessageBox::No;
        respostaPosProjetada = respostaPosInatingivel;
        posicaoAtingivel = false;
    }
    else if(colideComSegmentoL1)
    {        
        avisoColisaoSegmentoL1(posicaoProjetada);

        respostaPosInatingivel = QMessageBox::No;
        respostaPosProjetada = respostaPosInatingivel;
        posicaoAtingivel = false;
    }
    else if(!posicaoAtingivel)
    {
        QString strCompl("");
        QString strCompl2("");
        if(posicaoProjetada)
        {
            strCompl = ",\nmesmo com a posição projetada no plano que corta verticalmente o braço robô";
            strCompl2 = "as posições XYZ e ";
        }
        QString mensagem = "Posição XYZ não possui ângulos de junta correspondente\n"
                           "que respeite os limites máximos e mínimos das juntas"+strCompl+".\n"+
                           "Deseja adequar o resultado aos máximos e mínimos dos ângulos das juntas?\n"+
                           "Se clicar em não, "+strCompl2+"os ângulos alvo não serão alterados.";


        respostaPosInatingivel = caixaDialogoPerguntaSimNao(tr("Posição inatingível"), mensagem);

        respostaPosProjetada = respostaPosInatingivel;
    }


    if(respostaPosProjetada == QMessageBox::Yes)
    {
        ui->spnPosXAlvo->setValue(x);
        ui->spnPosYAlvo->setValue(y);
        ui->spnPosZAlvo->setValue(z);
        ui->spnRxAlvo->setValue(Rx);
        ui->spnRyAlvo->setValue(Ry);
        ui->spnRzAlvo->setValue(Rz);
    }


    if((!posicaoProjetada && posicaoAtingivel) ||
       (posicaoProjetada && posicaoAtingivel && respostaPosProjetada == QMessageBox::Yes) ||
       (!posicaoAtingivel && respostaPosInatingivel == QMessageBox::Yes))
    {
        for(int i = 0; i < 5; i++)
        {
            lstSpnAlvoGraus[i]->setValue(arredondaPara(angulosJuntas[i], CASAS_DECIMAIS_POSICAO_ANGULAR));
        }
        if(ui->chkEnviaComandoImediato->isChecked())
        {
            if(ui->rdbReadyForPIC->isChecked())
            {
                comandoJST();
            }
            else if (ui->rdbMiniMaestro24->isChecked())
            {
                // Aba Posições das juntas: comando JST para a placa Mini maestro 24, botão Calcular ângulos alvo
            }
        }
    }

    calculoAngulosAlvoAcionado = false;    
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
    if(ui->chkEnviaComandoImediato->isChecked() && !calculoAngulosAlvoAcionado)
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
        else
        {            
            double teta1 = lstSpnAlvoGraus[0]->value();
            double teta2 = lstSpnAlvoGraus[1]->value();
            double teta3 = lstSpnAlvoGraus[2]->value();
            double teta4 = lstSpnAlvoGraus[3]->value();
            double teta5 = lstSpnAlvoGraus[4]->value();

            bool colideComBaseFixa, colideComBaseGir, colideComSegmentoL1;

            double *posGarra = cinematica.posicaoGarra(teta1, teta2, teta3, teta4, teta5,
                                                       &colideComBaseFixa,
                                                       &colideComBaseGir,
                                                       &colideComSegmentoL1);
            delete posGarra;

            if(!(colideComBaseFixa || colideComBaseGir || colideComSegmentoL1))
            {
                if(ui->rdbReadyForPIC->isChecked())
                {
                    comandoEnvioImediato = QString("[JST%1%2]").arg(idJST[idxJunta]).arg(posicaoMicrossegundos, 4, 10, QChar('0'));

                    if(!timerEnvioImediato->isActive())
                    {
                        timerEnvioImediato->start(TEMPO_TIMER_ENVIO_IMEDIATO_MS);
                    }

                }
                else if(ui->rdbMiniMaestro24->isChecked())
                {                    
                    mm24->SetTarget(char(idxJunta), uint16_t(posicaoMicrossegundos));
                }
            }
            else
            {
                bool ehNumeroMaiorQueZero;

                if(colideComBaseFixa)
                    avisoColisaoBaseFixa();
                else if(colideComBaseGir)
                    avisoColisaoBaseGiratoria();
                else if(colideComSegmentoL1)
                    avisoColisaoSegmentoL1();

                ui->chkEnviaComandoImediato->setChecked(false);
                int posCorrente = lstEdtAtual[idxJunta]->text().replace(STR_UND_MICROSSEGUNDOS, "").toInt(&ehNumeroMaiorQueZero);
                if(ehNumeroMaiorQueZero)
                {
                    lstSpnAlvo[idxJunta]->setValue(posCorrente);
                }
            }
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
                    timerEnvioImediato->start(TEMPO_TIMER_ENVIO_IMEDIATO_MS);
                }
            }
            else
            {
                lstSpnVel[idxJunta]->setValue(1);
            }
        }
        else if(ui->rdbMiniMaestro24->isChecked())
        {            
            mm24->SetSpeed(char(idxJunta), uint16_t(velocidadeMicrossegundos));
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
                    timerEnvioImediato->start(TEMPO_TIMER_ENVIO_IMEDIATO_MS);
                }
            }
            else
            {
                lstSpnAcl[idxJunta]->setValue(1);
            }
        }
        else if(ui->rdbMiniMaestro24->isChecked())
        {            
            mm24->SetAcceleration(char(idxJunta), uint16_t(aceleracaoMicrossegundos));
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

void MainWindow::on_tabUnidadePos_currentChanged(int index)
{    
    if(index == 1)
    {
        coeffPontoVerde = coeffPontoVerdePropInv;
        offsetPontoVerde = offsetPontoVerdePropInv;
    }
    else
    {
        coeffPontoVerde = coeffPontoVerdePropDir;
        offsetPontoVerde = offsetPontoVerdePropDir;
    }

    for(int i = 0; i < QTD_SERVOS; i++)
    {
        int valor = lstEdtAtual[i]->text().replace(STR_UND_MICROSSEGUNDOS, "").toInt();

        if(valor > 0)
        {
            setaPosicaoPontoVerde(i, valor);
            lstPontoVerdeSliderPosAtual[i]->setVisible(true);
        }
        else
        {
            lstPontoVerdeSliderPosAtual[i]->setVisible(false);
        }

        bool propInv = (ui->tabelaPosLimitesGraus->item(i,4)->checkState() == Qt::CheckState::Checked);

        lstSlider[i]->setInvertedAppearance(index == 1 && propInv);
    }
}

/* NOTE: ***** Aba Sequência de Comandos ***** */

void MainWindow::on_btNovaSequencia_clicked(bool inicializando)
{
    on_btPararSeqComandos_clicked();

    int resposta = QMessageBox::No;

    if(!inicializando)
    {
        resposta = QMessageBox::question(this,
                                         tr("Nova sequência de comandos"),
                                         tr("Deseja criar uma nova sequência?\n"
                                            "A sequência atual será apagada."),
                                         QMessageBox::Yes,
                                         QMessageBox::No | QMessageBox::Default | QMessageBox::Escape,
                                         QMessageBox::NoButton);
    }
    if (inicializando || resposta == QMessageBox::Yes)
    {
        ui->lblNomeArquivoSequencia->setText("Nova sequência");
        ui->listSequenciaComandos->clear();
    }

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
    montagemDeComandosDialog->setIncrementosAng(this->incrementosAng);
    montagemDeComandosDialog->setIncVelGrausPorSeg(this->incVelGrausPorSeg);
    montagemDeComandosDialog->setIncAclGrausPorSegQuad(this->incAclGrausPorSegQuad);
    montagemDeComandosDialog->setVelocidades(this->lstSpnVel);
    montagemDeComandosDialog->setAceleracoes(this->lstSpnAcl);

    montagemDeComandosDialog->show();
}

void MainWindow::adicionarComandoAASequencia(QString comando)
{
    // TODO: Aba sequência: Permitir comandos serem adicionados no meio da sequência, ao invés de só no final
    // Obs.: ver o insertItem() do QListWidget
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

        comandoJST += idJST[i] + QString("%1").arg(lstEdtAtual[i]->text().replace(STR_UND_MICROSSEGUNDOS, "").toInt(), 4, 10, QChar('0'));
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
        montagemDeComandosDialog->setIncrementosAng(this->incrementosAng);
        montagemDeComandosDialog->setIncVelGrausPorSeg(this->incVelGrausPorSeg);
        montagemDeComandosDialog->setIncAclGrausPorSegQuad(this->incAclGrausPorSegQuad);
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
                                                    tr("Carregar Sequência de Comandos"),
                                                    "",
                                                    tr("Sequência (*.seq);;Arquivo TXT(*.txt)"));

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
                                                    tr("Salvar sequência de comandos"),
                                                    "",
                                                    tr("Sequência (*.seq);;Arquivo TXT(*.txt)"));

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

void MainWindow::habilitaBotoesExecComandos(bool estadoHab)
{
    ui->btExecutarSeqComandos->setEnabled(estadoHab && serial->isOpen());
    ui->btContinuarSeqComandos->setEnabled(estadoHab && serial->isOpen());
    ui->btExecutarLoopSeqComandos->setEnabled(estadoHab && serial->isOpen());
    ui->btContinuarLoopSeqComandos->setEnabled(estadoHab && serial->isOpen());
}

void MainWindow::habilitaBotoesContinuarDLYSemParam()
{
    ui->btContinuarSeqComandos->setEnabled(true);
    ui->btContinuarLoopSeqComandos->setEnabled(true);
}


void MainWindow::on_btExecutarSeqComandos_clicked()
{    
    if(ui->listSequenciaComandos->count() > 0)
    {
        ui->chkEnviaComandoImediato->setChecked(false);
        habilitaBotoesExecComandos(false);
        seqEmExecucao = true;
        emLoop = false;
        ui->listSequenciaComandos->setCurrentRow(0);
        ui->lblStatusSeqComandos->setText(STATUS_SEQCOM_EM_EXECUCAO);
        if(ui->rdbReadyForPIC->isChecked())
        {            
            if(!parser(ui->listSequenciaComandos->currentItem()->text()))
            {
                executaComandoDaSequencia();
            }
        }
        else if(ui->rdbMiniMaestro24->isChecked())
        {            
            if(!parserMM24(ui->listSequenciaComandos->currentItem()->text()))
            {
                executaComandoDaSequenciaMM24();
            }
        }
    }
}

void MainWindow::on_btContinuarSeqComandos_clicked()
{    
    if(ui->listSequenciaComandos->count() > 0)
    {
        ui->chkEnviaComandoImediato->setChecked(false);
        habilitaBotoesExecComandos(false);
        seqEmExecucao = true;
        emLoop = false;

        if(emDLYSemParam)
        {
            continuaExecucaoPartindoDoDLYSemParam();
        }
        else
        {
            int index_selected = ui->listSequenciaComandos->currentRow();
            if(index_selected < 0 || index_selected > ui->listSequenciaComandos->count())
                ui->listSequenciaComandos->setCurrentRow(0);
            ui->lblStatusSeqComandos->setText(STATUS_SEQCOM_CONTINUANDO_EXEC);
            if(ui->rdbReadyForPIC->isChecked())
            {                
                if(!parser(ui->listSequenciaComandos->currentItem()->text()))
                {
                    executaComandoDaSequencia();
                }
            }
            else if(ui->rdbMiniMaestro24->isChecked())
            {                
                if(!parserMM24(ui->listSequenciaComandos->currentItem()->text()))
                {
                    executaComandoDaSequenciaMM24();
                }
            }
        }
    }
}

void MainWindow::on_btExecutarLoopSeqComandos_clicked()
{    
    if(ui->listSequenciaComandos->count() > 0)
    {
        ui->chkEnviaComandoImediato->setChecked(false);
        habilitaBotoesExecComandos(false);
        seqEmExecucao = true;
        emLoop = true;
        ui->listSequenciaComandos->setCurrentRow(0);
        ui->lblStatusSeqComandos->setText(STATUS_SEQCOM_EM_LOOP);
        if(ui->rdbReadyForPIC->isChecked())
        {            
            if(!parser(ui->listSequenciaComandos->currentItem()->text()))
            {
                executaComandoDaSequencia();
            }
        }
        else if(ui->rdbMiniMaestro24->isChecked())
        {            
            if(!parserMM24(ui->listSequenciaComandos->currentItem()->text()))
            {
                executaComandoDaSequenciaMM24();
            }
        }
    }
}

void MainWindow::on_btContinuarLoopSeqComandos_clicked()
{    
    if(ui->listSequenciaComandos->count() > 0)
    {
        ui->chkEnviaComandoImediato->setChecked(false);
        habilitaBotoesExecComandos(false);

        seqEmExecucao = true;
        emLoop = true;

        if(emDLYSemParam)
        {
            continuaExecucaoPartindoDoDLYSemParam();
        }
        else
        {
            int index_selected = ui->listSequenciaComandos->currentRow();
            if(index_selected < 0 || index_selected > ui->listSequenciaComandos->count())
                ui->listSequenciaComandos->setCurrentRow(0);
            ui->lblStatusSeqComandos->setText(STATUS_SEQCOM_CONTINUANDO_LOOP);
            if(ui->rdbReadyForPIC->isChecked())
            {                
                if(!parser(ui->listSequenciaComandos->currentItem()->text()))
                {
                    executaComandoDaSequencia();
                }
            }
            else if(ui->rdbMiniMaestro24->isChecked())
            {                
                if(!parserMM24(ui->listSequenciaComandos->currentItem()->text()))
                {
                    executaComandoDaSequenciaMM24();
                }
            }
        }
    }
}


void MainWindow::on_btPararSeqComandos_clicked()
{
    if(seqEmExecucao)
    {
        seqEmExecucao = false;
        emLoop = false;
        emDLYSemParam = false;

        if(ui->chkParadaTotal->isChecked())
        {
            if(ui->rdbReadyForPIC->isChecked())
            {
                ui->chkParadaTotal->setEnabled(false);
                ui->btPararSeqComandos->setEnabled(false);
                filaComandosParaPararMov.clear();
                for(int i = 0; i < QTD_SERVOS; i++)
                {
                    velocidadesAnterioresAAParada[i] = lstSpnVel[i]->value();
                    filaComandosParaPararMov.enqueue(QString("[VEL%1%2]").arg(junta[i]).arg(1, 4, 10, QChar('0')));
                }
                enviaComando(filaComandosParaPararMov.dequeue());
                paradaDeSequenciaSolicitada = true;
                foiEnviadoJSTParaPararMov = false;
            }
            else if(ui->rdbMiniMaestro24->isChecked())
            {                
                ui->chkParadaTotal->setEnabled(false);
                ui->btPararSeqComandos->setEnabled(false);

                for(int i = 0; i < QTD_SERVOS; i++)
                {
                    velocidadesAnterioresAAParada[i] = lstSpnVel[i]->value();
                    mm24->SetSpeed(char(i), 1);
                }

                mm24->paradaTotalSolicitada = true;
            }
        }
    }

    if(ui->listSequenciaComandos->count() == 0)
    {
        ui->lblStatusSeqComandos->setText(STATUS_SEQCOM_PARADA_VAZIA);
    }
    else
    {
        ui->lblStatusSeqComandos->setText(STATUS_SEQCOM_PARADA_NAO_VAZIA);
        ui->lblStatusSeqComandos->setFocus();
    }
    habilitaBotoesExecComandos(true);
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
    // consequentemente não recebe resposta. Como o timer não é iniciado também,
    // não haverá a execução do estouro do timer, restando apenas o evento
    // que pega a tecla ENTER ou duplo clique.
    timerDLY->stop();
    emDLYSemParam = true;
    posUltimoDLYSemParam = ui->listSequenciaComandos->currentRow();
    ultimoStatusSeqComandos = ui->lblStatusSeqComandos->text();
    ui->listSequenciaComandos->setFocus();
    ui->lblStatusSeqComandos->setText(STATUS_SEQCOM_EM_DLY_SEM_PAR);
    habilitaBotoesContinuarDLYSemParam();
}





void MainWindow::timeoutDLY()
{
    if(!seqEmExecucao)
        return;

    if(ui->rdbReadyForPIC->isChecked())
        executaComandoDaSequencia();
    else if(ui->rdbMiniMaestro24->isChecked())
        executaComandoDaSequenciaMM24();
}

void MainWindow::continuaExecucaoPartindoDoDLYSemParam()
{
    ui->lblStatusSeqComandos->setText(ultimoStatusSeqComandos);

    emDLYSemParam = false;

    if(posUltimoDLYSemParam >= 0)
    {
        ui->listSequenciaComandos->setCurrentRow(posUltimoDLYSemParam);
        posUltimoDLYSemParam = -1;
    }

    if(ui->rdbReadyForPIC->isChecked())
        executaComandoDaSequencia();
    else if(ui->rdbMiniMaestro24->isChecked())
        executaComandoDaSequenciaMM24();
}

void MainWindow::on_listSequenciaComandos_itemActivated(QListWidgetItem *item)
{
    Q_UNUSED(item)

    if(!seqEmExecucao)
    {
        QString comando = item->text();

        if(ui->rdbReadyForPIC->isChecked())
            parser(comando);
        else if(ui->rdbMiniMaestro24->isChecked())
            parserMM24(comando);

        return;
    }

    if(!emDLYSemParam)
    {
        return;
    }

    continuaExecucaoPartindoDoDLYSemParam();
}


/* NOTE: ***** Comandos Mini Maestro 24 ***** */

void MainWindow::abrirGarraMiniMaestro24()
{
    unsigned short valorGarraAberta = ui->tabelaPosLimites->item(5, 0)->text().toUShort();
    this->mm24->SetTarget(5, valorGarraAberta);

}

void MainWindow::garraSemiabertaMiniMaestro24()
{
    unsigned short valorGarraSemiaberta = ui->tabelaPosLimites->item(5, 2)->text().toUShort();
    this->mm24->SetTarget(5, valorGarraSemiaberta);
}

void MainWindow::fecharGarraMiniMaestro24()
{
    unsigned short valorGarraFechada = ui->tabelaPosLimites->item(5, 1)->text().toUShort();
    this->mm24->SetTarget(5, valorGarraFechada);
}

void MainWindow::giroGarraMais90MiniMaestro24()
{
    unsigned short valorPulsoGarraMais90 = ui->tabelaPosLimites->item(4, 0)->text().toUShort();
    this->mm24->SetTarget(4, valorPulsoGarraMais90);
}

void MainWindow::garraPosNeutraMiniMaestro24()
{
    unsigned short valorPulsoGarraPosNeutra = ui->tabelaPosLimites->item(4, 2)->text().toUShort();
    this->mm24->SetTarget(4, valorPulsoGarraPosNeutra);
}

void MainWindow::giroGarraMenos90MiniMaestro24()
{
    unsigned short valorPulsoGarraMenos90 = ui->tabelaPosLimites->item(4, 1)->text().toUShort();
    this->mm24->SetTarget(4, valorPulsoGarraMenos90);
}

void MainWindow::posicaoDeRepousoMiniMaestro24()
{
    uint16_t posicaoRepouso[5];

    this->mm24->filaComAcionamento.clear();

    for(int i = 0; i < 5; i++)
    {
        posicaoRepouso[i] = ui->tabelaPosLimites->item(i, 3)->text().toUShort();
    }

    this->mm24->SetMultipleTargets(5, 0, posicaoRepouso);

    this->mm24->posicaoRepousoAcionada = true;
}

void MainWindow::posicaoNeutraJSTMiniMaestro24()
{
    uint16_t posicaoNeutra[6];

    this->mm24->filaComAcionamento.clear();

    for(int i = 0; i < 6; i++)
    {
        posicaoNeutra[i] = ui->tabelaPosLimites->item(i, 2)->text().toUShort();
    }

    this->mm24->SetMultipleTargets(6, 0, posicaoNeutra);
}

void MainWindow::posicaoNeutraCTZMiniMaestro24()
{
    uint16_t posicaoNeutra, posicaoCorrente;

    this->mm24->filaComAcionamento.clear();

    for(int i = 0; i < 6; i++)
    {
        posicaoNeutra = ui->tabelaPosLimites->item(i, 2)->text().toUShort();
        posicaoCorrente = lstEdtAtual[i]->text().replace(STR_UND_MICROSSEGUNDOS, "").toUShort();

        if(posicaoCorrente != posicaoNeutra)
        {
            comando_mm comandoCTZ;
            comandoCTZ.comando = MM24_SET_TARGET;
            comandoCTZ.canal = static_cast<char>(i);
            comandoCTZ.target[0] = posicaoNeutra;
            comandoCTZ.qtdBytesAReceber = 0;
            this->mm24->filaComAcionamento.enqueue(comandoCTZ);
        }
    }

    comando_mm primeiroCTZ = this->mm24->filaComAcionamento.dequeue();
    this->mm24->SetTarget(primeiroCTZ.canal, primeiroCTZ.target[0]);

}

void MainWindow::desligaServosMiniMaestro24()
{
    this->mm24->filaComAcionamento.clear();

    comando_mm comandoRepouso;
    comandoRepouso.comando = MM24_SET_MULTIPLE_TARGETS;
    comandoRepouso.numTargets = 5;
    comandoRepouso.canal = 0; // primeiro canal
    for(int canal = 0; canal < 5; canal++)
    {
        comandoRepouso.target[canal] = ui->tabelaPosLimites->item(canal, 3)->text().toUShort();
    }

    this->mm24->filaComAcionamento.enqueue(comandoRepouso);

    comando_mm desligaServos;
    desligaServos.comando = MM24_SET_MULTIPLE_TARGETS;
    desligaServos.numTargets = 6;
    desligaServos.canal = 0; // primeiro canal
    for(unsigned int canal = 0; canal < 6; canal++)
    {
        desligaServos.target[canal] = 0;
    }

    this->mm24->filaComAcionamento.enqueue(desligaServos);

    this->mm24->desligaServosAcionado = true;
}

bool MainWindow::parserMM24(QString comando)
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
            else
                return false;

            if(idxJunta != -1)
            {
                lstSpnVel[idxJunta]->setValue(valor);

                mm24->SetSpeed(char(idxJunta), uint16_t(valor));

                return true;
            }
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
            else
                return false;

            if(idxJunta != -1)
            {
                lstSpnAcl[idxJunta]->setValue(valor);

                mm24->SetAcceleration(char(idxJunta), uint16_t(valor));

                return true;
            }
        }
    }
    else if(comando.contains("JST"))
    {
        int tam = comando.length();
        if(tam >= 10 && tam <= 35 && tam % 5 == 0)
        {
            uint16_t posicoesAlvo[QTD_SERVOS];
            int qtJuntas = (tam-5)/5;
            int primeiroCanal = 5;

            for(int i = 0; i < QTD_SERVOS; i++)
            {
                if(lstChkHab[i]->isChecked())
                    posicoesAlvo[i] = uint16_t(lstSpnAlvo[i]->value());
                else
                    posicoesAlvo[i] = 0;
            }

            for(int i = 0; i < qtJuntas; i++)
            {
                char juntaJST = comando.at(5*i+4).toLatin1();

                int idxJunta = -1;
                if(juntaJST >= 'A' && juntaJST <= 'E')
                    idxJunta = juntaJST - 'A';
                else if (juntaJST == 'G')
                    idxJunta = 5;
                else
                    return false;

                if(primeiroCanal > idxJunta) primeiroCanal = idxJunta;

                if(idxJunta != -1)
                {
                    int valor = comando.mid(5*(i+1), 4).toInt();

                    if(valor > 0)
                    {
                        lstSpnAlvo[idxJunta]->setValue(valor);
                    }
                    else
                        lstChkHab[idxJunta]->setChecked(false);

                    posicoesAlvo[idxJunta] = uint16_t(valor);
                }
            }

            mm24->SetMultipleTargets(char(qtJuntas), char(primeiroCanal), posicoesAlvo);

            return true;
        }
    }
    else if(comando.contains("RPS"))
    {
        uint16_t posRepouso[5];
        for (int i = 0; i < 5; i++)
        {
            int valor = ui->tabelaPosLimites->item(i, 3)->text().toInt();
            this->lstSpnAlvo[i]->setValue(valor);
            posRepouso[i] = uint16_t(valor);
        }

        mm24->SetMultipleTargets(5, 0, posRepouso);

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
        else
            return false;

        if(idxJunta != -1)
        {
            int valor = ui->tabelaPosLimites->item(idxJunta, 2)->text().toInt();
            lstSpnAlvo[idxJunta]->setValue(valor);
            mm24->SetTarget(char(idxJunta), uint16_t(valor));
        }

        return true;
    }
    else if(comando.contains("GA"))
    {
        int valor = ui->tabelaPosLimites->item(5, 0)->text().toInt();
        ui->spnGRAlvo->setValue(valor);

        mm24->SetTarget(5, uint16_t(valor));

        return true;
    }
    else if(comando.contains("GF"))
    {
        int valor = ui->tabelaPosLimites->item(5, 1)->text().toInt();
        ui->spnGRAlvo->setValue(valor);

        mm24->SetTarget(5, uint16_t(valor));
        return true;
    }
    // Não possui comando LED. Este será ignorado para a Mini Maestro 24
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

void MainWindow::executaComandoDaSequenciaMM24()
{
    // BUG: Mini Maestro 24: Com a sequência em execução, se clicar em uma linha diferente do comando que está sendo executado, a linha clicada é executada.
    int index_selected = ui->listSequenciaComandos->currentRow();

    if(index_selected + 1 < ui->listSequenciaComandos->count())
    {
        ui->listSequenciaComandos->setCurrentRow(++index_selected);

        while (!parserMM24(ui->listSequenciaComandos->currentItem()->text()) &&
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

            while (!parserMM24(ui->listSequenciaComandos->currentItem()->text()) &&
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
            habilitaBotoesExecComandos(true);
        }
    }
}



/*NOTE: ***** Aba configurações ***** */

void MainWindow::on_rdbReadyForPIC_clicked()
{
    ui->rdbMiniMaestro24->setChecked(false);    
    mostrarAbaTerminal(ui->rdbReadyForPIC);
}

void MainWindow::on_rdbMiniMaestro24_clicked()
{
    ui->rdbReadyForPIC->setChecked(false);    
    mostrarAbaTerminal(ui->rdbMiniMaestro24);
}

void MainWindow::on_btObterPosLimites_clicked()
{
    on_btPararSeqComandos_clicked();

    ui->chkEnviaComandoImediato->setChecked(false);

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

    ui->chkEnviaComandoImediato->setChecked(false);

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
    ui->chkEnviaComandoImediato->setChecked(false);
    configurarConversaoEntreMicrossegundosEAngulos();
}

void MainWindow::on_rdbSemFeedback_clicked(bool checked)
{    
    if(checked)
    {
        on_btPararSeqComandos_clicked();
        ui->chkEnviaComandoImediato->setChecked(false);
        enviaComando("[FRS0]");
    };
}

void MainWindow::on_rdbPosicoesCorrentesDasJuntas_clicked(bool checked)
{
    if(checked)
    {
        on_btPararSeqComandos_clicked();
        ui->chkEnviaComandoImediato->setChecked(false);
        enviaComando("[FRS1]");
    }
}

void MainWindow::on_rdbSinalDeMovimento_clicked(bool checked)
{    
    if(checked)
    {
        on_btPararSeqComandos_clicked();
        ui->chkEnviaComandoImediato->setChecked(false);
        enviaComando("[FRS2]");
    }
}

void MainWindow::on_chkComandosBloqueantesDeMovimento_clicked(bool checked)
{
    on_btPararSeqComandos_clicked();
    ui->chkEnviaComandoImediato->setChecked(false);

    if(checked)
        enviaComando("[CSB1]");
    else
        enviaComando("[CSB0]");
}

void MainWindow::on_btResetarPlacaControle_clicked()
{
    on_btPararSeqComandos_clicked();
    ui->chkEnviaComandoImediato->setChecked(false);

    int resposta = QMessageBox::question(this,
                                         tr("Resetar placa de controle"),
                                         tr("Deseja reiniciar a placa Ready For PIC?\n"
                                            "A placa Mini Maestro 24 poderá ser reiniciada \n"
                                            "também caso seu pino de reset esteja ligado."),
                                         QMessageBox::Yes,
                                         QMessageBox::No | QMessageBox::Default | QMessageBox::Escape,
                                         QMessageBox::NoButton);

    if (resposta == QMessageBox::Yes)
        enviaComando("[RST]");
}

void MainWindow::on_btResetarPlacaServos_clicked()
{
    on_btPararSeqComandos_clicked();
    ui->chkEnviaComandoImediato->setChecked(false);

    int resposta = QMessageBox::question(this,
                                         tr("Resetar placa dos servos"),
                                         tr("Deseja reiniciar a placa Mini Maestro 24?"),
                                         QMessageBox::Yes,
                                         QMessageBox::No | QMessageBox::Default | QMessageBox::Escape,
                                         QMessageBox::NoButton);

    if (resposta == QMessageBox::Yes)
        enviaComando("[RSTM]");
}


/* NOTE: ***** Aba Terminal ***** */

void MainWindow::on_chkEcoLocal_clicked(bool checked)
{
    console->setLocalEchoEnabled(checked);
}

void MainWindow::on_btLimparErrosMM24_clicked()
{
    mm24->bytesDeErro[0] = 0x00;
    mm24->bytesDeErro[1] = 0x00;

    ui->lblCodigoErroMM24->setText("0x0000");
    ui->listErrosAtivos->clear();
}

/* NOTE: ***** Funções para Cinemática direta/inversa ***** */

double *MainWindow::preencheCamposXYZAtual(double *posicoesAtuaisGraus)
{
    double *posGarra = cinematica.posicaoGarra(posicoesAtuaisGraus[0], posicoesAtuaisGraus[1], posicoesAtuaisGraus[2],
                                    posicoesAtuaisGraus[3], posicoesAtuaisGraus[4]);

    ui->edtPosXAtual->setText(QString("%L1%2").arg(posGarra[0], 0, 'f', CASAS_DECIMAIS_POSICAO_XYZ).arg(STR_UND_CM));
    ui->edtPosYAtual->setText(QString("%L1%2").arg(posGarra[1], 0, 'f', CASAS_DECIMAIS_POSICAO_XYZ).arg(STR_UND_CM));
    ui->edtPosZAtual->setText(QString("%L1%2").arg(posGarra[2], 0, 'f', CASAS_DECIMAIS_POSICAO_XYZ).arg(STR_UND_CM));
    ui->edtRxAtual->setText(QString("%L1%2").arg(posGarra[3], 0, 'f', CASAS_DECIMAIS_POSICAO_ANGULAR).arg(STR_UND_GRAUS));
    ui->edtRyAtual->setText(QString("%L1%2").arg(posGarra[4], 0, 'f', CASAS_DECIMAIS_POSICAO_ANGULAR).arg(STR_UND_GRAUS));
    ui->edtRzAtual->setText(QString("%L1%2").arg(posGarra[5], 0, 'f', CASAS_DECIMAIS_POSICAO_ANGULAR).arg(STR_UND_GRAUS));

    return posGarra;
}

void MainWindow::preencheCamposXYZAlvo(double *posGarra)
{
    ui->spnPosXAlvo->setValue(posGarra[0]);
    ui->spnPosYAlvo->setValue(posGarra[1]);
    ui->spnPosZAlvo->setValue(posGarra[2]);
    ui->spnRxAlvo->setValue(posGarra[3]);
    ui->spnRyAlvo->setValue(posGarra[4]);
    ui->spnRzAlvo->setValue(posGarra[5]);
}
