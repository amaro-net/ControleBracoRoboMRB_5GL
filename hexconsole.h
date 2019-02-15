#ifndef HEXCONSOLE_H
#define HEXCONSOLE_H

#include <QPlainTextEdit>

class HexConsole : public QPlainTextEdit
{
        Q_OBJECT

    signals:
        void getData(const QByteArray &data);

    public:
        explicit HexConsole(QWidget *parent = nullptr);
        void putData(const QByteArray &data, bool ehEnvioDeCaracteres = false);
        void setLocalEchoEnabled(bool set);

        bool getMonitoramentoSerialHabilitado() const;
        void setMonitoramentoSerialHabilitado(bool value);

    protected:
        void keyPressEvent(QKeyEvent *e) Q_DECL_OVERRIDE;
        void mousePressEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
        void mouseDoubleClickEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
        void contextMenuEvent(QContextMenuEvent *e) Q_DECL_OVERRIDE;

    private:
        bool localEchoEnabled;
        bool monitoramentoSerialHabilitado = false;
        int idxCharDigitado = 0;
        QByteArray caracteresDigitados;
        char valorHex;
        int tamanhoMaximoLinha;
        int qtdCaracteresDigitadosLinha = 0;
};

#endif // HEXCONSOLE_H
