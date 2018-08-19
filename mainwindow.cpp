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
#include "solucaocinematicainversa.h"

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
                setaValorItemTabela(ui->tabelaPosLimites, i, j, QString("%1").arg(tabelaPosLimitesDefault[i][j]));
                setaValorItemTabela(ui->tabelaPosLimitesGraus, i, j, QString("%1").arg(tabelaPosLimitesGrausDefault[i][j]));
            }

            if(tabelaPosLimitesGrausDefault[i][4] == 1)
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
        // TODO: Cinemática direta/inversa: alterar a tabela de limites de ângulos para considerar a junta 2 com ângulos sempre positivos
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
    ui->chkBtPararEnviaJST->setEnabled(estadoHab);
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
        configuracoesIniciais();
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

double MainWindow::arredondaPara(double num, int casasDecimais)
{
    //char charArrayNumArred[50];

    // TODO: Arredondamentos: verificar, se possível, o porquê de alguns arredondamentos não funcionarem como esperado.
    double divisor = pow(10, casasDecimais);

    double parteInteira = trunc(num);
    double parteDecimal = num - parteInteira;
    parteDecimal = round(parteDecimal * divisor) / divisor;

    double numArred = parteInteira + parteDecimal;

    //double numArred = lround(num * divisor) / divisor;

    /*double numArred = num * divisor;
    numArred = lround(numArred);
    numArred = numArred / divisor;*/

    //sprintf(charArrayNumArred, "%.*f\n", casasDecimais, numArredAux);
    //QString strNumArred(charArrayNumArred);
    //double numArred = strNumArred.toDouble();

    return numArred;
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
    float xf;

    if(coeffPontoVerde != NULL && offsetPontoVerde != NULL)
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
        double *posGarra;

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
            double *posGarra;
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
                posicoesAtuaisGraus[k] = lstEdtAtualGraus[k]->text().replace(STR_UND_GRAUS, "").toDouble();
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
                    }
                }
                else
                {
                    lstChkHab[i]->setChecked(false);
                    lstSpnAlvo[i]->setEnabled(false);
                    lstSpnAlvoGraus[i]->setEnabled(false);
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

        if(paradaDeSequenciaSolicitada && ui->chkBtPararEnviaJST->isChecked())
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
                // TODO: Reimplementar a parada total com o novo comando de parada total implementado na placa de controle
                ui->chkBtPararEnviaJST->setEnabled(true);
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
    double teta1 = lstSpnAlvoGraus[0]->value();
    double teta2 = lstSpnAlvoGraus[1]->value();
    double teta3 = lstSpnAlvoGraus[2]->value();
    double teta4 = lstSpnAlvoGraus[3]->value();
    double teta5 = lstSpnAlvoGraus[4]->value();

    double *posGarra = posicaoGarra(teta1, teta2, teta3, teta4, teta5);

    preencheCamposXYZAlvo(posGarra);
    delete(posGarra);
}

void MainWindow::on_btCalcularAngulosAlvo_clicked()
{
    bool posicaoProjetada;
    bool posicaoAtingivel;
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
        angulosCorrentes[i] = lstEdtAtualGraus[i]->text().replace(STR_UND_GRAUS, "").toDouble();
    }

    double *angulosJuntas = angJuntas(&x, &y, &z, &Rx, &Ry, &Rz, angulosCorrentes, angMax, angMin, &posicaoProjetada, &posicaoAtingivel);

    if(posicaoProjetada && posicaoAtingivel)
    {
        respostaPosProjetada = QMessageBox::question(this,
                                                       tr("Posição projetada"),
                                                       tr("Posição XYZ foi projetada no plano que corta verticalmente\n"
                                                          "o braço robô.\n"
                                                          "Deseja atualizar a posição XYZ alvo com os valores projetados?\n"
                                                          "Se clicar em não, os ângulos alvo não serão alterados."),
                                                       QMessageBox::Yes,
                                                       QMessageBox::No | QMessageBox::Default | QMessageBox::Escape,
                                                       QMessageBox::NoButton);

        respostaPosInatingivel = respostaPosProjetada;
    }
    else if(!posicaoAtingivel)
    {
        QString strCompl("");
        QString strCompl2("");
        if(posicaoProjetada)
        {
            strCompl = ",\nmesmo com a posição projetada no plano que corta verticalmente o braço robô";
            strCompl2 = "as posições XYZ e";
        }
        QString mensagem = "Posição XYZ não possui ângulos de junta correspondente\n"
                           "que respeite os limites máximos e mínimos das juntas"+strCompl+".\n"+
                           "Deseja adequar o resultado aos máximos e mínimos dos ângulos das juntas?\n"+
                           "Se clicar em não, "+strCompl2+" os ângulos alvo não serão alterados.";

        respostaPosInatingivel = QMessageBox::question(this,
                                         tr("Posição inatingível"),
                                         mensagem,
                                         QMessageBox::Yes,
                                         QMessageBox::No | QMessageBox::Default | QMessageBox::Escape,
                                         QMessageBox::NoButton);

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
        else if(ui->rdbReadyForPIC->isChecked())
        {
            comandoEnvioImediato = QString("[JST%1%2]").arg(idJST[idxJunta]).arg(posicaoMicrossegundos, 4, 10, QChar('0'));

            if(!timerEnvioImediato->isActive())
            {
                timerEnvioImediato->start(TEMPO_TIMER_ENVIO_IMEDIATO_MS);
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
            // TODO: Aba sequência: parser para a Mini Maestro 24
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
                // TODO: Aba sequência: parser para a Mini Maestro 24
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
            // TODO: Aba sequência: parser para a Mini Maestro 24
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
                // TODO: Aba sequência: parser para a Mini Maestro 24
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

        if(ui->chkBtPararEnviaJST->isChecked())
        {
            ui->chkBtPararEnviaJST->setEnabled(false);
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
    // consequente não recebe resposta. Como o timer não é iniciado também,
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

    executaComandoDaSequencia();
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

    executaComandoDaSequencia();
}

void MainWindow::on_listSequenciaComandos_itemActivated(QListWidgetItem *item)
{
    Q_UNUSED(item)

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

    continuaExecucaoPartindoDoDLYSemParam();
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



/* NOTE: ***** Funções para Cinemática direta/inversa ***** */

/**
 * @brief MainWindow::cinematicaDireta
 * Função que retorna uma matriz 4 x 4 que faz a transformação
 * de coordenadas do referencial do pulso da garra para um referencial
 * que coincide com um ponto na superfície da base do braço robô, que
 * cruza com uma linha vertical que passa rente ao eixo da junta 0.
 * As 3 primeiras linhas e 3 primeiras colunas contém uma matriz de
 * rotação do referencial do pulso da garra, enquanto as 3 primeiras
 * linhas da quarta coluna possui as translações em x, y e z do referencial
 * do pulso da garra.
 *
 * @param teta1graus ângulo em graus da junta 0
 * @param teta2graus ângulo em graus da junta 1
 * @param teta3graus ângulo em graus da junta 2
 * @param teta4graus ângulo em graus da junta 3
 * @param teta5graus ângulo em graus da junta 4 (pulso da garra)
 * @return a matriz da cinemática direta, composta pela matriz de rotação e pela translação do referencial do pulso da garra.
 */
QMatrix4x4 MainWindow::cinematicaDireta(double teta1graus, double teta2graus, double teta3graus, double teta4graus, double teta5graus)
{
    // TODO: Cinemática direta: Alterar, mesmo que provisoriamente, a cinemática direta para considerar o teta3 sempre positivo.
    double teta1 = teta1graus * M_PI / 180;
    double teta2 = teta2graus * M_PI / 180;
    double teta3 = teta3graus * M_PI / 180;
    double teta4 = teta4graus * M_PI / 180;
    double teta5 = teta5graus * M_PI / 180;

    double s1 = sin(teta1);
    double s2 = sin(teta2);
    double s3 = sin(teta3);
    double s4 = sin(teta4);
    double s5 = sin(teta5);

    double c1 = cos(teta1);
    double c2 = cos(teta2);
    double c3 = cos(teta3);
    double c4 = cos(teta4);
    double c5 = cos(teta5);

    double s23 = c2*s3 + c3*s2;
    double c23 = c2*c3 - s2*s3;
    double c234 = c23*c4 - s23*s4;
    double s234 = s23*c4 + c23*s4;

    double r11 = c1*c234*c5 + s1*s5;
    double r21 = s1*c234*c5 - c1*s5;
    double r31 = s234*c5;
    double r12 =  s1*c5 - c1*c234*s5;
    double r22 = -c1*c5 - s1*c234*s5;
    double r32 = -s234*s5;
    double r13 = c1*s234;
    double r23 = s1*s234;
    double r33 = -c234;

    double f = a1 + a2*c2 + a3*c23 + a4*c234 + d5*s234;
    const double d234 = d2 + d3 + d4;

    double px = s1 * d234 + c1 * f;
    double py = s1 * f - c1 * d234;
    double pz = d1 + a2*s2 + a3*s23 + a4*s234 - d5*c234;

    return QMatrix4x4(r11, r12, r13, px,
                      r21, r22, r23, py,
                      r31, r32, r33, pz,
                        0,   0,   0,  1);
}

/**
 * @brief MainWindow::matrizPosGarra
 * Matriz de posicionamento da garra. Esta matriz faz a translação
 * do referencial da garra para o pulso, do pulso para o cruzamento
 * entre os eixos das juntas 1 e 0 (com a função de cinemática direta),
 * e do cruzamento de J1 e J0 para a base do braço robô.
 *
 * @param teta1graus ângulo em graus da junta 0
 * @param teta2graus ângulo em graus da junta 1
 * @param teta3graus ângulo em graus da junta 2
 * @param teta4graus ângulo em graus da junta 3
 * @param teta5graus ângulo em graus da junta 4 (pulso da garra)
 * @return Matriz de posicionamento da garra
 */
QMatrix4x4 MainWindow::matrizPosGarra(double teta1graus, double teta2graus, double teta3graus, double teta4graus, double teta5graus)
{
    QMatrix4x4 matrizGarraParaPulso(1, 0, 0, 0,
                                    0, 1, 0, 0,
                                    0, 0, 1, LgL3,
                                    0, 0, 0, 1);

    /*
    cinDir = ! r11 r12 r13   px !
             ! r21 r22 r23   py !
             ! r31 r32 r33   pz !
             !   0   0   0   1  !

    cinDir * mGP = ! r11 r12 r13   r13 * LgL3 + px !
                   ! r21 r22 r23   r23 * LgL3 + py !
                   ! r31 r32 r33   r33 * LgL3 + pz !
                   !   0   0   0          1        !

                   x = px + LgL3 * r13
                   y = py + LgL3 * r23
                   z = pz + LgL3 * r33

                   px = x - LgL3 * r13
                   py = y - LgL3 * r23
                   pz = z - LgL3 * r33
    */
    return cinematicaDireta(teta1graus, teta2graus, teta3graus, teta4graus, teta5graus)
           * matrizGarraParaPulso;
}

/**
 * @brief MainWindow::posicaoGarra
 * Função que retorna as coordenadas x, y, z, Rx, Ry e Rz da garra em relação à base do braço robô,
 * sendo Rx, Ry e Rz as rotações, em graus, em torno dos eixos X, Y e Z da base da garra.
 * Lembrar de desalocar o ponteiro que receber o resultado desta função quando não for mais usado.
 * @param teta1graus ângulo em graus da junta 0
 * @param teta2graus ângulo em graus da junta 1
 * @param teta3graus ângulo em graus da junta 2
 * @param teta4graus ângulo em graus da junta 3
 * @param teta5graus ângulo em graus da junta 4 (pulso da garra)
 * @return vetor que contém nos 3 primeiros elementos as coordenadas X, Y e Z da garra em cm, e no restante os ângulos Rx, Ry e Rz em graus.
 */
double *MainWindow::posicaoGarra(double teta1graus, double teta2graus, double teta3graus, double teta4graus, double teta5graus)
{    
    QMatrix4x4 MGarra = matrizPosGarra(teta1graus, teta2graus, teta3graus, teta4graus, teta5graus);

    double R[3][3];

    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            R[i][j] = arredondaPara(MGarra(i,j), 3);

    double x = arredondaPara(MGarra(0, 3), CASAS_DECIMAIS_POSICAO_XYZ);
    double y = arredondaPara(MGarra(1, 3), CASAS_DECIMAIS_POSICAO_XYZ);
    double z = arredondaPara(MGarra(2, 3), CASAS_DECIMAIS_POSICAO_XYZ);

    double beta = atan2(-R[2][0], sqrt(pow(R[0][0], 2) + pow(R[1][0], 2))) * 180 / M_PI;

    double alfa, gama;

    if(beta == 90)
    {
        alfa = 0;
        gama = atan2(R[0][1], R[1][1]) * 180 / M_PI;
    }
    else if(beta == -90)
    {
        alfa = 0;
        gama = -atan2(R[0][1], R[1][1]) * 180 / M_PI;
    }
    else
    {
        double cbeta = cos(beta * M_PI / 180);
        alfa = atan2(R[1][0]/cbeta, R[0][0]/cbeta) * 180 / M_PI;
        gama = atan2(R[2][1]/cbeta, R[2][2]/cbeta) * 180 / M_PI;
    }

    gama = arredondaPara(gama, CASAS_DECIMAIS_ROTACOES_XYZ);
    beta = arredondaPara(beta, CASAS_DECIMAIS_ROTACOES_XYZ);
    alfa = arredondaPara(alfa, CASAS_DECIMAIS_ROTACOES_XYZ);

    return new double[6]{x, y, z, gama, beta, alfa};
}

double *MainWindow::preencheCamposXYZAtual(double *posicoesAtuaisGraus)
{
    double *posGarra = posicaoGarra(posicoesAtuaisGraus[0], posicoesAtuaisGraus[1], posicoesAtuaisGraus[2],
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

/**
 * @brief MainWindow::angJuntas
 *
 * Cinemática inversa
 *
 * @param x coordenada x da garra em relação à base da garra. Esta coordenada poderá ser recalculada, caso ela não seja atingível.
 * @param y coordenada y da garra em relação à base da garra. Esta coordenada poderá ser recalculada, caso ela não seja atingível.
 * @param z coordenada z da garra em relação à base da garra. Esta coordenada poderá ser recalculada, caso ela não seja atingível.
 * @param gamaGraus
 * @param betaGraus
 * @param alfaGraus
 * @param angulosMaxGraus vetor contendo os ângulos máximo das juntas.
 * @param angulosMinGraus vetor contendo os ângulos mínimo das juntas.
 * @param posicaoProjetada true se a posição desejada foi projetada no plano vertical do braço robô, e qualquer das coordenadas (XYZ ou rotações) foi alterada. false caso contrário.
 * @param posicaoAtingivel true se a posição (projetada ou não) for atingível. false se qualquer das juntas assumir um ângulo impossível.
 * @return ângulos das juntas em graus, se a posição de alguma forma for atingível.
 */
double *MainWindow::angJuntas(double *x, double *y, double *z,
                              double *gamaGraus, double *betaGraus, double *alfaGraus,
                              double *angulosCorrentesJuntas,
                              double *angulosMaxGraus, double *angulosMinGraus,
                              bool *posicaoProjetada, bool *posicaoAtingivel)
{
    if(posicaoAtingivel != NULL)
        *posicaoAtingivel = true;

    double gama = *gamaGraus * M_PI /180;
    double beta = *betaGraus * M_PI /180;
    double alfa = *alfaGraus * M_PI /180;

    double sgama = arredondaPara(sin(gama), CASAS_DECIMAIS_SENOS_COSSENOS);
    double sbeta = arredondaPara(sin(beta), CASAS_DECIMAIS_SENOS_COSSENOS);
    double salfa = arredondaPara(sin(alfa), CASAS_DECIMAIS_SENOS_COSSENOS);

    double cgama = arredondaPara(cos(gama), CASAS_DECIMAIS_SENOS_COSSENOS);
    double cbeta = arredondaPara(cos(beta), CASAS_DECIMAIS_SENOS_COSSENOS);
    double calfa = arredondaPara(cos(alfa), CASAS_DECIMAIS_SENOS_COSSENOS);

    double r11;
    double r21;
    double r31;
    double r12 = calfa * sbeta * sgama - salfa * cgama;
    double r22 = salfa * sbeta * sgama + calfa * cgama;
    double r32 = cbeta * sgama;
    double r13 = calfa * sbeta * cgama + salfa * sgama;
    double r23 = salfa * sbeta * cgama - calfa * sgama;
    double r33 = cbeta * cgama;

    double px = *x - LgL3 * r13;
    double py = *y - LgL3 * r23;
    double pz = *z - LgL3 * r33;

    px = arredondaPara(px, CASAS_DECIMAIS_POSICAO_XYZ);
    py = arredondaPara(py, CASAS_DECIMAIS_POSICAO_XYZ);
    pz = arredondaPara(pz, CASAS_DECIMAIS_POSICAO_XYZ);

    double px2py2 = pow(px,2) + pow(py,2);
    double sqrtpx2py2 = sqrt(px2py2);

    // Projetando o ponto desejado no plano do braço. Esta projeção será a que a
    // garra poderá realmente assumir (ver cap. 4 do craig - The Yasukawa
    // Motoman L-3 página 121)

    // WARNING: Ponto da garra e origem do referencial da base: Notar que o ponto da garra (e o ponto do pulso) não está rente com a origem do referencial da base fixa.
    // WARNING: Vetor M e plano do braço robô: O plano cuja normal é M passa pelo pulso da garra e pela origem do referencial da base (não pega o ponto da garra).
    //          Ou seja, esse mesmo plano é oblíquo ao plano que pega a coordenada da garra e do pulso
    //          e também, a um plano paralelo que pega a origem da base fixa.
    // WARNING: Ver warnings do arquivo constantes.h referentes aos parâmetros d2, d3, d4 e d5

    QVector3D M = QVector3D(-py/sqrtpx2py2, px/sqrtpx2py2, 0);
    QVector3D Zt(r13, r23, r33);
    QVector3D Yt(r12, r22, r32);

    QVector3D K = QVector3D::crossProduct(M, Zt);

    QVector3D Ztl = QVector3D::crossProduct(K, M);

    /* WARNING: Vetor Ztl:  Se forem respeitados os parâmetros exatos de Denavit-Hatenberg obtidos para o braço robô (mais
     * especificamente d2, d3 e d4), Ztl sempre apontará para uma direção diferente do Zt, mesmo que a posição
     * da garra seja uma posição possível, por conta do vetor M calculado (ver warning referente ao vetor M e ao plano ao
     * qual ele é perpendicular).
     */
    double cteta = QVector3D::dotProduct(Zt, Ztl);
    double steta = QVector3D::dotProduct(QVector3D::crossProduct(Zt, Ztl), K);

    steta = arredondaPara(steta, CASAS_DECIMAIS_SENOS_COSSENOS);
    cteta = arredondaPara(cteta, CASAS_DECIMAIS_SENOS_COSSENOS);

    QVector3D Ytl = cteta * Yt
                 + steta * QVector3D::crossProduct(K, Yt)
                 + ((1 - cteta) * QVector3D::dotProduct(K, Yt)) * K;

    QVector3D Xtl = QVector3D::crossProduct(Ytl, Ztl);

    for(int i = 0; i < 3; i++)
    {
        // TODO: Cinemática inversa: Verificar se estes arredondamentos fazem sentido
        Xtl[i] = arredondaPara(Xtl[i], CASAS_DECIMAIS_SENOS_COSSENOS);
        Ytl[i] = arredondaPara(Ytl[i], CASAS_DECIMAIS_SENOS_COSSENOS);
        Ztl[i] = arredondaPara(Ztl[i], CASAS_DECIMAIS_SENOS_COSSENOS);
    }

    r11 = Xtl[0];
    r21 = Xtl[1];
    r31 = Xtl[2];

    r12 = Ytl[0];
    r22 = Ytl[1];
    r32 = Ytl[2];

    r13 = Ztl[0];
    r23 = Ztl[1];
    r33 = Ztl[2];

    // Recalculando x, y e z da garra    
    double xProj = px + LgL3 * r13;
    double yProj = py + LgL3 * r23;
    double zProj = pz + LgL3 * r33;

    //recalculando gama, beta e alfa
    double betaProj = atan2(-r31, sqrt(pow(r11,2)+pow(r21,2)));
    double alfaProj, gamaProj;

    if(betaProj == M_PI_2)
    {
        alfaProj = 0;
        gamaProj = atan2(r12, r22);
    }
    else if (betaProj == -M_PI_2)
    {
        alfaProj = 0;
        gamaProj = -atan2(r12, r22);
    }
    else
    {
        double cbeta = cos(betaProj);
        cbeta = arredondaPara(cbeta, CASAS_DECIMAIS_SENOS_COSSENOS);
        alfaProj = atan2(r21/cbeta, r11/cbeta);
        gamaProj = atan2(r32/cbeta, r33/cbeta);
    }

    xProj = arredondaPara(xProj, CASAS_DECIMAIS_POSICAO_XYZ);
    yProj = arredondaPara(yProj, CASAS_DECIMAIS_POSICAO_XYZ);
    zProj = arredondaPara(zProj, CASAS_DECIMAIS_POSICAO_XYZ);

    double alfaGrausProj = alfaProj * 180 / M_PI;
    double betaGrausProj = betaProj * 180 / M_PI;
    double gamaGrausProj = gamaProj * 180 / M_PI;

    alfaGrausProj = arredondaPara(alfaGrausProj, CASAS_DECIMAIS_ROTACOES_XYZ);
    betaGrausProj = arredondaPara(betaGrausProj, CASAS_DECIMAIS_ROTACOES_XYZ);
    gamaGrausProj = arredondaPara(gamaGrausProj, CASAS_DECIMAIS_ROTACOES_XYZ);

    if(posicaoProjetada != NULL)
    {
        *posicaoProjetada = xProj != *x || yProj != *y || zProj != *z ||
                            alfaGrausProj != *alfaGraus ||
                            betaGrausProj != *betaGraus ||
                            gamaGrausProj != *gamaGraus;
    }

    *x = xProj;
    *y = yProj;
    *z = zProj;
    *gamaGraus = gamaGrausProj;
    *betaGraus = betaGrausProj;
    *alfaGraus = alfaGrausProj;

    // Início da cinemática inversa propriamente dita
    SolucaoCinematicaInversa *solucao;
    QList<SolucaoCinematicaInversa *> solucoes;
    QList<double> solucaoTeta1;    

    const double d234 = d2 + d3 + d4;
    const double d234quad = d234 * d234;

    double atan2pypx = atan2(-px, py);
    double atan2sqrt = atan2(sqrt(px2py2 - d234quad),-d234);

    double teta1_1 = atan2pypx + atan2sqrt;
    double teta1_2 = atan2pypx - atan2sqrt;
    double teta1_3 = atan2(r23, r13);
    double teta1_4 = atan2(r23, r13);

    double teta1min = angulosMinGraus[0] * M_PI / 180;
    double teta1max = angulosMaxGraus[0] * M_PI / 180;

    double teta2min = angulosMinGraus[1] * M_PI / 180;
    double teta2max = angulosMaxGraus[1] * M_PI / 180;

    double teta3min = angulosMinGraus[2] * M_PI / 180;
    double teta3max = angulosMaxGraus[2] * M_PI / 180;

    double teta4min = angulosMinGraus[3] * M_PI / 180;
    double teta4max = angulosMaxGraus[3] * M_PI / 180;

    double teta5min = angulosMinGraus[4] * M_PI / 180;
    double teta5max = angulosMaxGraus[4] * M_PI / 180;


    double teta1, teta2, teta3, teta4, teta5;

    if((teta1_1 >= teta1min) && (teta1_1 <= teta1max))
        solucaoTeta1.append(teta1_1);

    if((teta1_2 >= teta1min) && (teta1_2 <= teta1max))
    {
        if(!solucaoTeta1.contains(teta1_2))
            solucaoTeta1.append(teta1_2);
    }

    if((teta1_3 >= teta1min) && (teta1_3 <= teta1max))
    {
        if(!solucaoTeta1.contains(teta1_3))
            solucaoTeta1.append(teta1_3);
    }

    if((teta1_4 >= teta1min) && (teta1_4 <= teta1max))
    {
        if(!solucaoTeta1.contains(teta1_3))
            solucaoTeta1.append(teta1_4);
    }

    if(solucaoTeta1.isEmpty())
    {
        if (teta1_1 < teta1min || teta1_2 < teta1min || teta1_3 < teta1min || teta1_4 < teta1min)
        {
            teta1 = teta1min;

        }
        else if (teta1_1 > teta1max || teta1_2 > teta1max || teta1_3 > teta1max || teta1_4 > teta1max)
        {
            teta1 = teta1max;            
        }        

        if(posicaoAtingivel != NULL)
            *posicaoAtingivel = false;

        solucao = new SolucaoCinematicaInversa;

        solucao->teta1 = teta1;

        solucao->s1 = arredondaPara(sin(teta1), CASAS_DECIMAIS_SENOS_COSSENOS);
        solucao->c1 = arredondaPara(cos(teta1), CASAS_DECIMAIS_SENOS_COSSENOS);
        solucao->possivel = false;
        solucao->teta1possivel = false;

        solucoes.append(solucao);
    }
    else
    {
        for(int i = 0; i < solucaoTeta1.count(); i++)
        {
            solucao = new SolucaoCinematicaInversa;

            teta1 = solucaoTeta1.at(i);

            solucao->teta1 = teta1;
            solucao->s1 = arredondaPara(sin(teta1), CASAS_DECIMAIS_SENOS_COSSENOS);
            solucao->c1 = arredondaPara(cos(teta1), CASAS_DECIMAIS_SENOS_COSSENOS);
            solucao->possivel = true;
            solucao->teta1possivel = true;

            solucoes.append(solucao);
        }
    }

    double pzd1 = pz - d1;

    for(int i = 0; i < solucoes.count(); i++)
    {
        solucao = solucoes.at(i);

        teta1 = solucoes.at(i)->teta1;
        double s1 = solucoes.at(i)->s1;
        double c1 = solucoes.at(i)->c1;

        double s5 = s1 * r11 - c1 * r21;
        double c5 = s1 * r12 - c1 * r22;

        teta5 = atan2(s5, c5);

        if(teta5 >= teta5min && teta5 <= teta5max)
        {
            solucao->teta5 = teta5;
            solucao->s5 = s5;
            solucao->c5 = c5;
            solucao->teta5possivel = true;
        }
        else
        {
            if(teta5 < teta5min)
                teta5 = teta5min;
            else if(teta5 > teta5max)
                teta5 = teta5max;

            s5 = arredondaPara(sin(teta5), CASAS_DECIMAIS_SENOS_COSSENOS);
            c5 = arredondaPara(cos(teta5), CASAS_DECIMAIS_SENOS_COSSENOS);

            solucao->teta5 = teta5;
            solucao->s5 = s5;
            solucao->c5 = c5;
            solucao->possivel = false;
            solucao->teta5possivel = false;
        }

        double c234, s234, teta234;

        s234 = c1 * r13 + s1 * r23;
        c234 = -r33;

        teta234 = atan2(s234, c234);

        double px2py2pzdl2 = px2py2 + pow(pzd1, 2);
        // TODO: Cinemática inversa: Realizar testes com a cinemática inversa assumindo teta3 com ângulos sempre positivos
        double c3 = (px2py2pzdl2 - pow(a2, 2) - pow(a3, 2))/(2 * a2 * a3);
        double s3 = -sqrt(1 - pow(c3, 2)); // teta3 sempre será negativo

        teta3 = atan2(s3, c3);

        if(teta3 >= teta3min && teta3 <= teta3max)
        {
            solucao->teta3 = teta3;
            solucao->s3 = s3;
            solucao->c3 = c3;
            solucao->teta3possivel = true;
        }
        else
        {            
            if(teta3 < teta3min)
                teta3 = teta3min;
            else if(teta3 > teta3max)
                teta3 = teta3max;

            s3 = arredondaPara(sin(teta3), CASAS_DECIMAIS_SENOS_COSSENOS);
            c3 = arredondaPara(cos(teta3), CASAS_DECIMAIS_SENOS_COSSENOS);

            solucao->teta3 = teta3;
            solucao->s3 = s3;
            solucao->c3 = c3;
            solucao->possivel = false;
            solucao->teta3possivel = false;
        }

        // TODO: Cinemática inversa: rever as fórmulas para o teta2 e o teta4.
        double beta2 = atan2(pzd1, px2py2);

        //double cksi = (px2py2pzdl2 - pow(a2, 2) - pow(a3, 2))/(2 * a2 * sqrt(px2py2pzdl2));
        //double ksi = atan2(sqrt(1 - pow(cksi, 2)), cksi);

        double ksi = atan2(a3 * s3, a2 + a3 * c3);

        teta2 = beta2 - ksi;

        teta4 = teta234 - teta2 - teta3;

        double beta2graus = beta2 * 180 / M_PI;
        double ksigraus = ksi * 180 / M_PI;

        double teta234graus = teta234 * 180 / M_PI;
        double teta2graus = teta2 * 180 / M_PI;
        double teta3graus = teta3 * 180 / M_PI;
        double teta4graus = teta4 * 180 / M_PI;

        if(teta2 >= teta2min && teta2 <= teta2max)
        {
            solucao->teta2 = teta2;
            solucao->teta2possivel = true;
        }
        else
        {
            if(teta2 < teta2min)
                teta2 = teta2min;
            else if(teta2 > teta2max)
                teta2 = teta2max;

            solucao->teta2 = teta2;
            solucao->possivel = false;
            solucao->teta3possivel = false;
        }

        if(teta4 >= teta4min && teta4 <= teta4max)
        {
            solucao->teta4 = teta4;
            solucao->teta4possivel = true;
        }
        else
        {
            if(teta4 < teta4min)
                teta4 = teta4min;
            else if(teta4 > teta4max)
                teta4 = teta4max;

            solucao->teta4 = teta4;
            solucao->possivel = false;
            solucao->teta4possivel = false;
        }

    }



    // Avaliação final das soluções
    if(solucoes.count() == 1)
    {
        solucao = solucoes[0];

        if(posicaoAtingivel != NULL)
        {
            *posicaoAtingivel = solucao->possivel;
        }        

        if(solucao->subSolucao != NULL)
        {
            // Unificação da solução com a subsolução
            SubSolucaoCinematicaInversa *subSolucao = solucao->subSolucao;

            if(!(solucao->possivel
                 || (subSolucao->teta2possivel
                     && subSolucao->teta3possivel
                     && subSolucao->teta4possivel))
               )
            {
                // Substitui os ângulos da solução pelos que constam na subsolução
                // quando a solução em si não é possível. Esta é uma forma de
                // fazer a solução ficar mais próxima do possível
                solucao->teta2 = subSolucao->teta2;
                solucao->teta3 = subSolucao->teta3;
                solucao->teta4 = subSolucao->teta4;
            }
            else
            {
                // Pega a subsolução de maior peso.
                double difTeta2 = abs(angulosCorrentesJuntas[1] - subSolucao->teta2);
                double difTeta3 = abs(angulosCorrentesJuntas[2] - subSolucao->teta3);
                double difTeta4 = abs(angulosCorrentesJuntas[3] - subSolucao->teta4);

                subSolucao->peso = (pesoTeta2 * difTeta2 + pesoTeta3 * difTeta3 + pesoTeta4 * difTeta4)/(pesoTeta2 + pesoTeta3 + pesoTeta4);

                difTeta2 = abs(angulosCorrentesJuntas[1] - solucao->teta2);
                difTeta3 = abs(angulosCorrentesJuntas[2] - solucao->teta3);
                difTeta4 = abs(angulosCorrentesJuntas[3] - solucao->teta4);

                double pesoSolucao = (pesoTeta2 * difTeta2 + pesoTeta3 * difTeta3 + pesoTeta4 * difTeta4)/(pesoTeta2 + pesoTeta3 + pesoTeta4);

                if(subSolucao->peso >= pesoSolucao)
                {
                    solucao->teta2 = subSolucao->teta2;
                    solucao->teta3 = subSolucao->teta3;
                    solucao->teta4 = subSolucao->teta4;
                }
            }

            delete subSolucao;
        }        
    }
    else
    {        
        // Ver no livro do Craig páginas 104 e 105 sobre pesos das juntas e solução de menor peso.
        QList<SolucaoCinematicaInversa *> solucoesValidas;

        for(int i = 0; i < solucoes.count(); i++)
        {
            solucao = solucoes.at(i);

            if(solucao->subSolucao != NULL)
            {
                // Unifica a subsolução com base nos pesos
                SubSolucaoCinematicaInversa *subSolucao = solucao->subSolucao;

                double difTeta2 = abs(angulosCorrentesJuntas[1] - subSolucao->teta2);
                double difTeta3 = abs(angulosCorrentesJuntas[2] - subSolucao->teta3);
                double difTeta4 = abs(angulosCorrentesJuntas[3] - subSolucao->teta4);

                subSolucao->peso = (pesoTeta2 * difTeta2 + pesoTeta3 * difTeta3 + pesoTeta4 * difTeta4)/(pesoTeta2 + pesoTeta3 + pesoTeta4);

                difTeta2 = abs(angulosCorrentesJuntas[1] - solucao->teta2);
                difTeta3 = abs(angulosCorrentesJuntas[2] - solucao->teta3);
                difTeta4 = abs(angulosCorrentesJuntas[3] - solucao->teta4);

                double pesoSolucao = (pesoTeta2 * difTeta2 + pesoTeta3 * difTeta3 + pesoTeta4 * difTeta4)/(pesoTeta2 + pesoTeta3 + pesoTeta4);

                if(subSolucao->peso >= pesoSolucao)
                {
                    solucao->teta2 = subSolucao->teta2;
                    solucao->teta3 = subSolucao->teta3;
                    solucao->teta4 = subSolucao->teta4;
                }

                delete subSolucao;
            }

            // Inclui a solução na lista de soluções válidas
            if(solucao->possivel)
            {
                solucoesValidas.append(solucao);
            }
        }

        // Se não existir solução válida
        if(solucoesValidas.isEmpty())
        {
            int maiorQtdPosicoesPossiveis = 0;
            int idxSolMaiorQtdAngPossivel = 0;

            // Pega a solução inválida com maior quantidade de ângulos válidos
            for(int i = 0; i < solucoes.count(); i++)
            {
                int qtd = 0;

                if(solucoes.at(i)->teta1possivel)
                    qtd++;

                if(solucoes.at(i)->teta2possivel)
                    qtd++;

                if(solucoes.at(i)->teta3possivel)
                    qtd++;

                if(solucoes.at(i)->teta4possivel)
                    qtd++;

                if(solucoes.at(i)->teta5possivel)
                    qtd++;

                if(qtd > maiorQtdPosicoesPossiveis)
                {
                    maiorQtdPosicoesPossiveis = qtd;
                    idxSolMaiorQtdAngPossivel = i;
                }
            }

            solucao = solucoes.at(idxSolMaiorQtdAngPossivel);
        }
        else if(solucoesValidas.count() == 1)
        {
            solucao = solucoesValidas.at(0);

            if(posicaoAtingivel != NULL)
            {
                *posicaoAtingivel = solucao->possivel;
            }
        }
        else
        {
            // Obtém a solução válida de maior peso, ou a primeira, em caso de pesos iguais
            double difTeta[QTD_SERVOS - 1];
            double angTeta[QTD_SERVOS - 1];
            double pesosTeta[QTD_SERVOS - 1];

            pesosTeta[0] = pesoTeta1;
            pesosTeta[1] = pesoTeta2;
            pesosTeta[2] = pesoTeta3;
            pesosTeta[3] = pesoTeta4;
            pesosTeta[4] = pesoTeta5;

            for(int i = 0; i < solucoes.count(); i++)
            {
                solucao = solucoes.at(i);

                angTeta[0] = solucao->teta1;
                angTeta[1] = solucao->teta2;
                angTeta[2] = solucao->teta3;
                angTeta[3] = solucao->teta4;
                angTeta[4] = solucao->teta5;

                for(int j = 0; j < QTD_SERVOS - 1; j++)
                    difTeta[j] = abs(angulosCorrentesJuntas[j] - angTeta[j]);

                double somatorio = 0, somaPesos = 0;

                for(int j = 0; j < QTD_SERVOS - 1; j++)
                {
                    somatorio += pesosTeta[j] * difTeta[j];
                    somaPesos = pesosTeta[j];
                }

                solucao->peso = somatorio/somaPesos;
            }

            int idxSolucaoPesoMaior = 0;
            double maiorPeso = solucoes.at(0)->peso;

            for(int i = 1; i < solucoes.count(); i++)
            {
                if(solucoes.at(i)->peso > maiorPeso)
                {
                    maiorPeso = solucoes.at(i)->peso;
                    idxSolucaoPesoMaior = i;
                }
            }

            solucao = solucoes.at(idxSolucaoPesoMaior);            
        }
    }

    if(posicaoAtingivel != NULL)
    {
        *posicaoAtingivel = solucao->possivel;
    }

    teta1 = solucao->teta1;
    teta2 = solucao->teta2;
    teta3 = solucao->teta3;
    teta4 = solucao->teta4;
    teta5 = solucao->teta5;

    while(!solucoes.isEmpty())
    {
        delete solucoes.takeFirst();
    }

    teta1 = teta1 * 180 / M_PI;
    teta2 = teta2 * 180 / M_PI;
    teta3 = teta3 * 180 / M_PI;
    teta4 = teta4 * 180 / M_PI;
    teta5 = teta5 * 180 / M_PI;

    teta1 = arredondaPara(teta1, CASAS_DECIMAIS_POSICAO_ANGULAR);
    teta2 = arredondaPara(teta2, CASAS_DECIMAIS_POSICAO_ANGULAR);
    teta3 = arredondaPara(teta3, CASAS_DECIMAIS_POSICAO_ANGULAR);
    teta4 = arredondaPara(teta4, CASAS_DECIMAIS_POSICAO_ANGULAR);
    teta5 = arredondaPara(teta5, CASAS_DECIMAIS_POSICAO_ANGULAR);

    return new double[5]{teta1, teta2, teta3, teta4, teta5};
}


