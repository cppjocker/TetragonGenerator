#-------------------------------------------------
#
# Project created by QtCreator 2012-02-12T15:04:34
#
#-------------------------------------------------

QT       += core gui

TARGET = FIT9201KLIMOV_puzzle
TEMPLATE = app


SOURCES += main.cpp \
    tetragonwindow.cpp \
    tetragongenerator.cpp \
    geometricutils.cpp \
    convextetragon.cpp \
    noconvextetragon.cpp \
    tetragon.cpp

HEADERS  += \
    tetragonwindow.h \
    tetragongenerator.h \
    geometricutils.h \
    convextetragon.h \
    tetragon.h \
    noconvextetragon.h

FORMS    += mainwindow.ui

RESOURCES += \
    application.qrc
