#-------------------------------------------------
#
# Project created by QtCreator 2018-01-28T14:25:01
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ControleBracoRobo
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
    janelalicencagpl.cpp \
    janelasobre.cpp \
        main.cpp \
        mainwindow.cpp \
    console.cpp \
    montagemdecomandosdialog.cpp \
    solucaocinematicainversa.cpp \
    cinematica.cpp \
    utils.cpp \
    plano3d.cpp \
    minimaestro24.cpp \
    hexconsole.cpp

HEADERS += \
    janelalicencagpl.h \
    janelasobre.h \
        mainwindow.h \
    console.h \
    montagemdecomandosdialog.h \
    constantes.h \
    solucaocinematicainversa.h \
    cinematica.h \
    utils.h \
    plano3d.h \
    minimaestro24.h \
    hexconsole.h

FORMS += \
    janelalicencagpl.ui \
    janelasobre.ui \
    janelasobre_linux.ui \
        mainwindow.ui \
    mainwindow_linux.ui \
    montagemdecomandosdialog.ui

QT += serialport

RESOURCES += \
    controlebracoroboresorcefile.qrc

DISTFILES += \
    COPYING

RC_ICONS += icone_mainwindow_J1J_2.ico
