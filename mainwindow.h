#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#define QTD_SERVOS 6

#include "montagemdecomandosdialog.h"
#include "console.h"

#include <QMainWindow>

#include <QtSerialPort/QSerialPort>

#include <QtWidgets/QSpinBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QSlider>
#include <QListWidgetItem>
#include <qqueue.h>
#include <QTimer>


namespace Ui {
class MainWindow;
}

class MontagemDeComandosDialog;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void adicionarComandoAASequencia(QString comando);
    void editarComandoNaSequencia(QString comando);

private slots:    

    void abrirPortaSerial();
    void fecharPortaSerial();
    void writeData(const QByteArray &data);
    void readData();    
    void handleError(QSerialPort::SerialPortError error);
    void timeoutDLY();
    void timeoutConfig();

    void on_rdbReadyForPIC_clicked();

    void on_rdbMiniMaestro24_clicked();

    void on_chkAtivarLeds_clicked(bool checked);

    void on_btAtualizarLeds_clicked();

    void on_btAbrirGarra_clicked();

    void on_btGarraSemiaberta_clicked();

    void on_btFecharGarra_clicked();

    void on_btGiroGarraMais90_clicked();

    void on_btGarraPosNeutra_clicked();

    void on_btGiroGarraMenos90_clicked();

    void on_btPosicaoRepouso_clicked();

    void on_btPosNeutraJST_clicked();

    void on_btDesligaServos_clicked();

    void on_btPosNeutraCTZ_clicked();

    void on_btMover_clicked();

    void on_btMoverComVelEAcl_clicked();

    void on_btNovaSequencia_clicked();

    void on_btAdicionarComando_clicked();

    void on_btAdicionarPosCorrente_clicked();

    void on_btComandoUp_clicked();

    void on_btComandoDown_clicked();

    void on_btEditarComando_clicked();

    void on_btRemoverComando_clicked();

    void on_btCarregarSeqComandos_clicked();

    void on_btSalvarSeqComandos_clicked();

    void on_btExecutarSeqComandos_clicked();

    void on_btContinuarSeqComandos_clicked();

    void on_btExecutarLoopSeqComandos_clicked();

    void on_btContinuarLoopSeqComandos_clicked();

    void on_btPararSeqComandos_clicked();

    void on_btAdicionarComandoAASeqComandos_clicked();

    void on_btAdicionarLedsSeqComandos_clicked();

    void on_btObterPosLimites_clicked();

    void on_btConfigurarPosLimites_clicked();

    void on_btConfigurarPosLimitesGraus_clicked();

    void on_rdbSemFeedback_clicked(bool checked);

    void on_rdbPosicoesCorrentesDasJuntas_clicked(bool checked);

    void on_rdbSinalDeMovimento_clicked(bool checked);

    void on_chkComandosBloqueantesDeMovimento_clicked(bool checked);

    void on_btResetarPlacaControle_clicked();

    void on_btResetarPlacaServos_clicked();

    void on_btCalcularXYZAlvo_clicked();

    void on_btCalcularAngulosAlvo_clicked();

    void on_chkLEDPi_clicked();

    void on_listSequenciaComandos_itemActivated(QListWidgetItem *item);

    void on_chkEcoLocal_clicked(bool checked);

private:
    void preencheCombosPortaSerial();
    void showStatusMessage(const QString &message);
    void conectarComponentes();
    void alimentarListasDeComponentes();
    void alimentarFilaDeComandosDeInicializacao();

    void configuracoesIniciais();
    void habilitarComponentesConn(bool estadoHab);
    void habilitarComponentes(bool estadoHab);
    void habilitarComponentesReadyForPIC(bool estadoHab);
    void HabilitarComponentesComServosLigados();

    void enviaComando(QString comando);
    void executaComandoDaSequencia();

    void recebeCaracteresDeResposta(QByteArray data);
    void decodificaResposta();

    void setarValorPosLimiteResposta(QString resposta);
    void setarVelOuAclResposta(QString resposta, QList<QSpinBox *> listaSpinBox);


    /* Comandos */
    void abrirGarra();
    void garraSemiaberta();
    void fecharGarra();
    void giroGarraMais90();
    void garraPosNeutra();
    void giroGarraMenos90();
    void posicaoDeRepouso();
    void posicaoNeutraJST();
    void posicaoNeutraCTZ();
    void desligaServos();

    void comandoJST();
    void comandoLED();
    void comandoLEDSemParametros();

    bool parser(QString comando);

    void iniciaDLYSemParametro();


    Ui::MainWindow *ui;
    MontagemDeComandosDialog *montagemDeComandosDialog;

    Console *console;
    QSerialPort *serial;

    QList<QSpinBox *> lstSpnAlvo;
    QList<QLineEdit *> lstEdtAtual;
    QList<QSpinBox *> lstSpnVel;
    QList<QSpinBox *> lstSpnAcl;

    QList<QDoubleSpinBox *> lstSpnAlvoGraus;
    QList<QLineEdit *> lstEdtAtualGraus;
    QList<QDoubleSpinBox *> lstSpnVelGrausPorSeg;
    QList<QDoubleSpinBox *> lstSpnAclGrausPorSegQuad;

    QList<QCheckBox *> lstChkHab;
    QList<QCheckBox *> lstChkHabGraus;

    QList<QSlider *> lstSlider;
    QList<QSlider *> lstSliderGraus;

    QList<QCheckBox *> lstChkLeds;

    QQueue<QString> filaBufferEntrada;
    bool bufferPronto = false;
    bool flagProtocolo = false;
    QString bufferProto;
    bool inicializando = false;
    bool ecoEch0 = false;
    bool ecoCaracteresAtivado = false;

    QQueue<QString> filaComandosInicializacao;
    QQueue<QString> filaComandosPosicaoNeutraCTZ;
    QQueue<QString> filaComandosMoverComVelAcl;
    QQueue<QString> filaConfigPosLimites;

    bool seqEmExecucao = false;
    bool emLoop = false;
    int linhaCorrenteSequencia = -1;
    bool itemClicado = false;

    QTimer *timer;
    int tentativaConfig = 0;

    QTimer *timerDLY;
    bool emDLYSemParam = false;
    int posUltimoDLYSemParam = -1;
    QString ultimoStatusSeqComandos = "";
};

//QString junta[QTD_SERVOS] = {"J0", "J1", "J2", "J3", "J4", "GR"};
//QString idJST[QTD_SERVOS] = {"A", "B", "C", "D", "E", "G"};

#endif // MAINWINDOW_H
