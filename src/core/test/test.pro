QT += testlib
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

SOURCES +=  tst_testgamefield.cpp \
    ../eventscheduler.cpp \
    ../gamefield.cpp \
    ../gamelist.cpp \
    ../gamemap.cpp \
    ../gameobjects.cpp

HEADERS += \
    ../eventscheduler.h \
    ../gamefield.h \
    ../gamelist.h \
    ../gamemap.h \
    ../gameobjects.h