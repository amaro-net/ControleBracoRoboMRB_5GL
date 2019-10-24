/*
    Controle do Braço Robô MRB-5GL - controls and configures MRB-5GL, a 5 DOF Robot Arm prototype

    Copyright (C) 2019  Amaro Duarte de Paula Neto

    This file is part of Controle do Braço Robô MRB-5GL.

    Controle do Braço Robô MRB-5GL is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Controle do Braço Robô MRB-5GL is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Controle do Braço Robô MRB-5GL.  If not, see <https://www.gnu.org/licenses/>.

    contact e-mail: amaro.net80@gmail.com


    Este arquivo é parte do programa Controle do Braço Robô MRB-5GL

    Controle do Braço Robô MRB-5GL é um software livre; você pode redistribuí-lo e/ou
    modificá-lo dentro dos termos da Licença Pública Geral GNU como
    publicada pela Free Software Foundation (FSF); na versão 3 da
    Licença, ou (a seu critério) qualquer versão posterior.

    Controle do Braço Robô MRB-5GL é distribuído na esperança de que possa ser útil,
    mas SEM NENHUMA GARANTIA; sem uma garantia implícita de ADEQUAÇÃO
    a qualquer MERCADO ou APLICAÇÃO EM PARTICULAR. Veja a
    Licença Pública Geral GNU para maiores detalhes.

    Você deve ter recebido uma cópia da Licença Pública Geral GNU junto
    com este programa, Se não, veja <http://www.gnu.org/licenses/>.
*/
#ifndef MONTAGEMDECOMANDOSDIALOG_H
#define MONTAGEMDECOMANDOSDIALOG_H

#include "mainwindow.h"
#include "constantes.h"


#include <QDialog>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QSlider>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QRadioButton>


namespace Ui {
    class MontagemDeComandosDialog;
}

class MainWindow;

class MontagemDeComandosDialog : public QDialog
{
        Q_OBJECT

    public:
        explicit MontagemDeComandosDialog(QWidget *parent = 0);
        ~MontagemDeComandosDialog();

        void setMainWindow(MainWindow *mainUi);

        void setEmEdicao(bool emEdicao);
        void setTextEdtComando(QString comando);
        void setTabelaPosLimites(QTableWidget* tabelaPosLimites);
        void setTabelaPosLimitesGraus(QTableWidget* tabelaPosLimitesGraus);

        void setIncrementosAng(double incrementosAng[]);
        void setIncVelGrausPorSeg(double incVelGrausPorSeg[]);
        void setIncAclGrausPorSegQuad(double incAclGrausPorSegQuad[]);

        void setPosicoesAlvo(QList<QSpinBox *> lstSpnPosAlvo);
        void setVelocidades(QList<QSpinBox *> lstSpnVel);
        void setAceleracoes(QList<QSpinBox *> lstSpnAcl);

    private slots:
        void on_btAdicionarOuModificar_clicked();

        void on_btSair_clicked();

        void on_btFecharGarra_clicked();

        void on_btAbrirGarra_clicked();

        void on_btRepouso_clicked();

        void on_btCTZ_clicked();

        void on_rdbJunta0_clicked();

        void on_rdbJunta1_clicked();

        void on_rdbJunta2_clicked();

        void on_rdbJunta3_clicked();

        void on_rdbJunta4_clicked();

        void on_rdbGarra_clicked();

        void on_btJST_clicked();

        void on_chkJunta0_clicked(bool checked);

        void on_chkJunta1_clicked(bool checked);

        void on_chkJunta2_clicked(bool checked);

        void on_chkJunta3_clicked(bool checked);

        void on_chkJunta4_clicked(bool checked);

        void on_chkGarra_clicked(bool checked);

        void on_spnJ0Alvo_valueChanged(int posicaoMicrossegundos);

        void on_spnJ1Alvo_valueChanged(int posicaoMicrossegundos);

        void on_spnJ2Alvo_valueChanged(int posicaoMicrossegundos);

        void on_spnJ3Alvo_valueChanged(int posicaoMicrossegundos);

        void on_spnJ4Alvo_valueChanged(int posicaoMicrossegundos);

        void on_spnGRAlvo_valueChanged(int posicaoMicrossegundos);

        void on_spnJ0AlvoGraus_valueChanged(double posicaoGraus);

        void on_spnJ1AlvoGraus_valueChanged(double posicaoGraus);

        void on_spnJ2AlvoGraus_valueChanged(double posicaoGraus);

        void on_spnJ3AlvoGraus_valueChanged(double posicaoGraus);

        void on_spnJ4AlvoGraus_valueChanged(double posicaoGraus);

        void on_spnGRAlvoGraus_valueChanged(double posicaoGraus);

        void on_btVel_clicked();

        void on_rdbJunta0Vel_clicked();

        void on_rdbJunta1Vel_clicked();

        void on_rdbJunta2Vel_clicked();

        void on_rdbJunta3Vel_clicked();

        void on_rdbJunta4Vel_clicked();

        void on_rdbGarraVel_clicked();

        void on_spnVel_valueChanged(int velTmpPulso);

        void on_spnVelGrausPorSeg_valueChanged(double velGrausPorSeg);

        void on_btAcl_clicked();

        void on_rdbJunta0Acl_clicked();

        void on_rdbJunta1Acl_clicked();

        void on_rdbJunta2Acl_clicked();

        void on_rdbJunta3Acl_clicked();

        void on_rdbJunta4Acl_clicked();

        void on_rdbGarraAcl_clicked();

        void on_chkLEDPi_clicked();

        void on_edtComando_textChanged(const QString &newText);

        void on_spnAcl_valueChanged(int aclTmpPulso);

        void on_spnAclGrausPorSegQuad_valueChanged(double aclGrausPorSegQuad);

        void on_btLED_clicked();

        void on_btDLY_clicked();

        void on_spnTempoMs_valueChanged(int arg1);

        void on_btDesliga_clicked();

    private:
        Ui::MontagemDeComandosDialog *ui;

        MainWindow *mainWindow;

        int tabelaPosLimites[QTD_SERVOS][4];
        double tabelaPosLimitesGraus[QTD_SERVOS][4];
        double incVelGrausPorSeg[QTD_SERVOS];
        double incAclGrausPorSegQuad[QTD_SERVOS];

        int tempoPulsoMax[QTD_SERVOS];
        int tempoPulsoMin[QTD_SERVOS];

        double angMax[QTD_SERVOS];
        double angMin[QTD_SERVOS];

        int posAlvo[QTD_SERVOS];
        int velocidade[QTD_SERVOS];
        int aceleracao[QTD_SERVOS];
        bool emEdicao = false;

        QList<QRadioButton *> lstRdbJuntaCTZ;
        QList<QCheckBox *> lstChkJuntaJST;
        QList<QSpinBox *> lstSpnPosAlvo;
        QList<QDoubleSpinBox *> lstSpnPosAlvoGraus;
        QList<QRadioButton *> lstRdbJuntaVEL;
        QList<QRadioButton *> lstRdbJuntaACL;
        QList<QCheckBox *> lstChkLEDPi;

        void alimentarListasDeComponentes();
        void setarVelOuAcl(QString comando, QSpinBox *spinBox);
        void onRdbJuntaCTZClicked(int idxJunta);
        void onChkJuntaJSTClicked(int idxJunta, bool checked);
        void onSpnAlvo_valueChanged(int idxJunta, int posicaoMicrossegundos);
        void onSpnJuntaAlvoGraus_valueChanged(int idxJunta, double posicaoGraus);
        void onRdbJuntaVelClicked(int idxJunta);
        void onRdbJuntaAclClicked(int idxJunta);

        int pegaJuntaMarcadaVel();
        int pegaJuntaMarcadaAcl();
};

#endif // MONTAGEMDECOMANDOSDIALOG_H
