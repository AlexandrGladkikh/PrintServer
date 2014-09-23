#-------------------------------------------------
#
# Project created by QtCreator 2014-09-05T20:07:00
#
#-------------------------------------------------

QT       += core
QT       += widgets
QT       += gui
QT       += printsupport

TARGET = PrintQT
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    lib/wrap_func.cpp \
    lib/sock.cpp \
    lib/error.cpp

HEADERS += \
    lib/unp.h \
    lib/sock.h \
    lib/error.h \
    lib/wrap_func.h
