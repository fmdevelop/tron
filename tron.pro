#-------------------------------------------------
#
# Project created by QtCreator 2015-12-12T17:35:32
#
#-------------------------------------------------

QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = tron
TEMPLATE = app


SOURCES += src/main.cpp\
    src/gameplay/game.cpp \
    src/gameplay/initialgamestategenerator.cpp \
    src/constants.cpp \
    src/rendering/openglwindow.cpp \
    src/mainwindow.cpp \
    src/rendering/trongamewindow.cpp \
    src/rendering/trongamewidget.cpp

HEADERS  += \
    src/data/data.h \
    src/data/field.h \
    src/data/border.h \
    src/data/explosion.h \
    src/data/snake.h \
    src/data/gamestate.h \
    src/constants.h \
    src/gameplay/game.h \
    src/data/initobject.h \
    src/gameplay/initialgamestategenerator.h \
    src/rendering/openglwindow.h \
    src/mainwindow.h \
    src/rendering/trongamewindow.h \
    src/rendering/trongamewidget.h

FORMS    += \
    src/mainwindow.ui
