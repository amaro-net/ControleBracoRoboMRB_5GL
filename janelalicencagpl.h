#ifndef JANELALICENCAGPL_H
#define JANELALICENCAGPL_H

#include <QMainWindow>

namespace Ui {
class JanelaLicencaGPL;
}

class JanelaLicencaGPL : public QMainWindow
{
    Q_OBJECT

public:
    explicit JanelaLicencaGPL(QWidget *parent = nullptr);
    ~JanelaLicencaGPL();

private:
    Ui::JanelaLicencaGPL *ui;
};

#endif // JANELALICENCAGPL_H
