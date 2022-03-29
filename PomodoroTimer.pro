#-------------------------------------------------
#
# Project created by QtCreator 2016-02-15T12:15:16
#
#-------------------------------------------------

QT       += core gui multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

equals(QMAKE_CXX, g++) { QMAKE_LFLAGS += -no-pie }

TARGET = PomodoroTimer
TEMPLATE = app


SOURCES += main.cpp\
    pomodorotimerapplication.cpp

HEADERS  += \
    pomodorotimerapplication.h

FORMS    +=

RESOURCES += \
    resources.qrc

RC_ICONS = resources/Tomato-48.ico
