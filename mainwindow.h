#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "montagemdecomandosdialog.h"
#include "console.h"
#include "constantes.h"

#include <QMainWindow>

#include <QtSerialPort/QSerialPort>

#include <QtWidgets/QSpinBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QSlider>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QLabel>
#include <QListWidgetItem>
#include <qqueue.h>
#include <QTimer>
#include <QMatrix4x4>
#include <QVector3D>
#include <QVector4D>

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

        /* Funções de conversão de valores de posição, velocidade e aceleração */
        double converteMicrossegundosParaGraus(int idxJunta, int posicaoMicrossegundos);
        int converteGrausParaMicrossegundos(int idxJunta, double posicaoGraus);
        double converteVelTmpPulsoParaGrausPorSeg(int idxJunta, int velTmpPulso);
        int converteVelGrausPorSegParaTmpPulso(int idxJunta, double velGrausPorSeg);
        double converteAclTmpPulsoParaGrausPorSegQuad(int idxJunta, int aclTmpPulso);
        int converteAclGrausPorSegQuadParaTmpPulso(int idxJunta, double aclGrausPorSegQuad);

    private slots:

        void abrirPortaSerial();
        void fecharPortaSerial();
        void writeData(const QByteArray &data);
        void readData();
        void handleError(QSerialPort::SerialPortError error);
        void timeoutDLY();
        void timeoutConfig();
        void timeoutEnvioImediato();

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

        void on_btNovaSequencia_clicked(bool inicializando = false);

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

        void on_spnJ0Alvo_valueChanged(int posicaoMicrossegundos);

        void on_spnJ1Alvo_valueChanged(int posicaoMicrossegundos);

        void on_spnJ2Alvo_valueChanged(int posicaoMicrossegundos);

        void on_spnJ3Alvo_valueChanged(int posicaoMicrossegundos);

        void on_spnJ4Alvo_valueChanged(int posicaoMicrossegundos);

        void on_spnGRAlvo_valueChanged(int posicaoMicrossegundos);

        void on_spnJ0Vel_valueChanged(int velTmpPulso);

        void on_spnJ1Vel_valueChanged(int velTmpPulso);

        void on_spnJ2Vel_valueChanged(int velTmpPulso);

        void on_spnJ3Vel_valueChanged(int velTmpPulso);

        void on_spnJ4Vel_valueChanged(int velTmpPulso);

        void on_spnGRVel_valueChanged(int velTmpPulso);

        void on_spnJ0Acl_valueChanged(int aclTmpPulso);

        void on_spnJ1Acl_valueChanged(int aclTmpPulso);

        void on_spnJ2Acl_valueChanged(int aclTmpPulso);

        void on_spnJ3Acl_valueChanged(int aclTmpPulso);

        void on_spnJ4Acl_valueChanged(int aclTmpPulso);

        void on_spnGRAcl_valueChanged(int aclTmpPulso);

        void on_spnJ0AlvoGraus_valueChanged(double posicaoGraus);

        void on_spnJ1AlvoGraus_valueChanged(double posicaoGraus);

        void on_spnJ2AlvoGraus_valueChanged(double posicaoGraus);

        void on_spnJ3AlvoGraus_valueChanged(double posicaoGraus);

        void on_spnJ4AlvoGraus_valueChanged(double posicaoGraus);

        void on_spnGRAlvoGraus_valueChanged(double posicaoGraus);

        void on_spnJ0VelGrausPorSeg_valueChanged(double velGrausPorSeg);

        void on_spnJ1VelGrausPorSeg_valueChanged(double velGrausPorSeg);

        void on_spnJ2VelGrausPorSeg_valueChanged(double velGrausPorSeg);

        void on_spnJ3VelGrausPorSeg_valueChanged(double velGrausPorSeg);

        void on_spnJ4VelGrausPorSeg_valueChanged(double velGrausPorSeg);

        void on_spnGRVelGrausPorSeg_valueChanged(double velGrausPorSeg);

        void on_spnJ0AclGrausPorSegQuad_valueChanged(double aclGrausPorSegQuad);

        void on_spnJ1AclGrausPorSegQuad_valueChanged(double aclGrausPorSegQuad);

        void on_spnJ2AclGrausPorSegQuad_valueChanged(double aclGrausPorSegQuad);

        void on_spnJ3AclGrausPorSegQuad_valueChanged(double aclGrausPorSegQuad);

        void on_spnJ4AclGrausPorSegQuad_valueChanged(double aclGrausPorSegQuad);

        void on_spnGRAclGrausPorSegQuad_valueChanged(double aclGrausPorSegQuad);

        void on_chkJ0_clicked(bool checked);

        void on_chkJ1_clicked(bool checked);

        void on_chkJ2_clicked(bool checked);

        void on_chkJ3_clicked(bool checked);

        void on_chkJ4_clicked(bool checked);

        void on_chkGR_clicked(bool checked);

        void on_chkEnviaComandoImediato_toggled(bool checked);

        void on_sliderJ0_valueChanged(int value);

        void on_sliderJ1_valueChanged(int value);

        void on_sliderJ2_valueChanged(int value);

        void on_sliderJ3_valueChanged(int value);

        void on_sliderJ4_valueChanged(int value);

        void on_sliderGR_valueChanged(int value);

        void on_tabUnidadePos_currentChanged(int index);

    private:
        void preencheCombosPortaSerial();
        void showStatusMessage(const QString &message);
        void conectarComponentes();
        void alimentarListasDeComponentes();
        void alimentarFilaDeComandosDeInicializacao();

        void configuracoesIniciais();

        void configurarConversaoEntreMicrossegundosEAngulos(bool valoresDefault = false);

        void habilitarComponentesConn(bool estadoHab);
        void habilitarComponentes(bool estadoHab);
        void habilitarComponentesReadyForPIC(bool estadoHab);
        void HabilitarComponentesComServosLigados();

        void enviaComando(QString comando);
        void executaComandoDaSequencia();

        void recebeCaracteresDeResposta(QByteArray data);
        void setaPosicaoPontoVerde(int idxJunta, int posicao);
        void montaJSTParaPararMov1Junta();
        void decodificaResposta();

        void setarValorPosLimiteResposta(QString resposta);
        void setaValorItemTabela(QTableWidget *tableWidget, int idxLinha, int idxColuna, QString strValor);
        void setarVelOuAclResposta(QString resposta, QList<QSpinBox *> listaSpinBox);

        void converteSpnAlvoParaGraus(int idxJunta, int posicaoAlvo);
        void converteSpnVelParaGrausPorSeg(int idxJunta, int velocidade);
        void converteSpnAclParaGrausPorSegQuad(int idxJunta, int aceleracao);

        void converteSpnAlvoGrausParaTmpPulso(int idxJunta, double posicaoGraus);
        void converteSpnVelGrausPorSegParaTmpPulso(int idxJunta, double velGrausPorSeg);
        void converteSpnAclGrausPorSegQuadParaTmpPulso(int idxJunta, double aclGrausPorSegQuad);

        void habilitaJunta(int idxJunta, bool checked);

        void habilitaCamposAbaPosicaoAlvo(int posicaoAba, bool estadoHab);
        void habilitaCamposAbaPosicaoAlvoJunta(int posicaoAba, int idxJunta, bool estadoHab);
        void enviaPosicaoAlvoAssimQueMudar(int idxJunta, int posicaoMicrossegundos);
        void enviaVelocidadeAssimQueMudar(int idxJunta, int velocidadeMicrossegundos);
        void enviaAceleracaoAssimQueMudar(int idxJunta, int aceleracaoMicrossegundos);

        void sliderValueChanged(int idxJunta, int value);

        void habilitaBotoesExecComandos(bool estadoHab);
        void habilitaBotoesContinuarDLYSemParam();
        void continuaExecucaoPartindoDoDLYSemParam();

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


        /* Funções para cinemática direta/inversa */
        QMatrix4x4 cinematicaDireta(double teta1graus, double teta2graus, double teta3graus, double teta4graus, double teta5graus);
        QMatrix4x4 matrizPosGarra(double teta1graus, double teta2graus, double teta3graus, double teta4graus, double teta5graus);
        double *posicaoGarra(double teta1graus, double teta2graus, double teta3graus, double teta4graus, double teta5graus);
        double *preencheCamposXYZAtual(double *posicoesAtuaisGraus);
        void preencheCamposXYZAlvo(double* posGarra);
        double *angJuntas(double *x, double *y, double *z,
                          double *gamaGraus, double *betaGraus, double *alfaGraus,
                          double *angulosMaxGraus, double *angulosMinGraus,
                          bool *posicaoProjetada = NULL, bool *posicaoAtingivel = NULL);


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

        QList<QSlider *> lstSlider;

        QList<QCheckBox *> lstChkLeds;

        QList<QLabel *> lstPontoVerdeSliderPosAtual;

        QQueue<QString> filaBufferEntrada;
        bool bufferPronto = false;
        bool flagProtocolo = false;
        QString bufferProto;
        bool inicializando = false;
        bool ecoEch0 = false;
        bool ecoCaracteresAtivado = false;

        QQueue<QString> filaComandosInicializacao;
        QQueue<QString> filaComandosMovimentacaoAbaComandos;
        QQueue<QString> filaComandosMoverComVelAcl;
        QQueue<QString> filaConfigPosLimites;
        QQueue<QString> filaComandosParaPararMov;

        bool seqEmExecucao = false;
        bool emLoop = false;
        bool itemClicado = false;
        bool paradaDeSequenciaSolicitada = false;
        int velocidadesAnterioresAAParada[QTD_SERVOS];
        bool foiEnviadoJSTParaPararMov = false;
        QString comandoParaPararMovEnviado;
        QString ultimoVELcomVelocidadeAnterior = "";

        QTimer *timer;
        int tentativaConfig = 0;

        QTimer *timerDLY;
        bool emDLYSemParam = false;
        int posUltimoDLYSemParam = -1;
        QString ultimoStatusSeqComandos = "";
        int countAbaPosicoesValueChanged = -1;

        QTimer *timerEnvioImediato;
        QString comandoEnvioImediato;

        QString comandoJSTParaPararMov;

        bool calculoAngulosAlvoAcionado = false;

        /**** Variáveis para conversão entre ângulos em graus e posições em microssegundos ****/
        double coeffAng[QTD_SERVOS];
        double offsetAng[QTD_SERVOS];
        int tempoPulsoMax[QTD_SERVOS];
        int tempoPulsoMin[QTD_SERVOS];
        double angMax[QTD_SERVOS];
        double angMin[QTD_SERVOS];
        int qtdPosicoesTmpPulso[QTD_SERVOS];
        double incrementosAng[QTD_SERVOS];
        double incVelGrausPorSeg[QTD_SERVOS];
        double incAclGrausPorSegQuad[QTD_SERVOS];

        double *coeffPontoVerde;
        double *offsetPontoVerde;
        double coeffPontoVerdePropInv[QTD_SERVOS];
        double offsetPontoVerdePropInv[QTD_SERVOS];
        double coeffPontoVerdePropDir[QTD_SERVOS];
        double offsetPontoVerdePropDir[QTD_SERVOS];
};

#endif // MAINWINDOW_H
