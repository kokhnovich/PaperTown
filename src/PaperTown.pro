#-------------------------------------------------
#
# Project created by QtCreator 2019-02-26T17:11:24
#
#-------------------------------------------------

QT       += core gui

RESOURCES += img.qrc


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PaperTown
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

CONFIG += c++14

SOURCES += \
        main.cpp \
        mainwindow.cpp \
        core/gameobjects.cpp \
        core/gamelist.cpp \
    core/gamemap.cpp \
    core/gamemapbase.cpp \
    core/gamemultimap.cpp

HEADERS += \
        mainwindow.h \
        core/gameobjects.h \
        core/gamelist.h \
    core/gamemap.h \
    core/gamemapbase.h \
    core/gamemultimap.h

FORMS += \
    mainwindow.ui

# DISTFILES += \
#     cell3.png

DISTFILES += \
    img/cell1.jpg \
    img/cell2.jpg \
    img/cell.png \
    img/cell3.png

SUBDIRS += \
    PaperTown.pros