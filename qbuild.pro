# Copyright (c) 2011 Radek Polak

TEMPLATE=app

# The binary name
TARGET=qtmoko-chess

# This app uses Qtopia
CONFIG+=qtopia

DEFINES+=QTOPIA

QT += svg

SOURCES += main.cpp\
    mainwindow.cpp \
    boardwidget.cpp

HEADERS  += mainwindow.h \
    boardwidget.h
    
RESOURCES = qresources.qrc

pkg [
    name=qtmoko-chess
    desc="chess"
    version=$$QTOPIA_VERSION
    license=$$QTOPIA_LICENSE
    maintainer=$$QTOPIA_MAINTAINER
]

target [
    hint=sxe
    domain=trusted
]

desktop [
    hint=desktop
    files=qtmoko-chess.desktop
    path=/apps/Games
]

pics [
    hint=pics
    files=chess.svg
    path=/pics/qtmoko-chess
]
