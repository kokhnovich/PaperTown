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
    gameobjectpickermodel.cpp \
    core/gameobjects.cpp \
    core/gamelist.cpp \
    core/gamemap.cpp \
    core/gamefield.cpp \
    core/gameobjectfactory.cpp \
    core/eventscheduler.cpp \
    objects/gameobjectrepository.cpp \
    objects/gameobjectproperties.cpp \
    scene/gamescene.cpp \
    scene/gametextures.cpp \
    scene/gamefieldviews.cpp \
    scene/gamescenegeometry.cpp \
    util/coordinates.cpp \
    util/misc.cpp

HEADERS += \
    mainwindow.h \
    gameobjectpickermodel.h \
    core/gameobjects.h \
    core/gamelist.h \
    core/gamemap.h \
    core/gamefield.h \
    core/gameobjectfactory.h \
    core/eventscheduler.h \
    objects/gameobjectrepository.h \
    objects/gameobjectproperties.h \
    scene/gamescene.h \
    scene/gametextures.h \
    scene/gamefieldviews.h \
    scene/gamescenegeometry.h \
    util/coordinates.h \
    util/misc.h

FORMS += \
    mainwindow.ui

SUBDIRS += \
    PaperTown.pros
