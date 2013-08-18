#-------------------------------------------------
#
# Project created by QtCreator 2013-08-01T23:17:13
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += oauth

TARGET = qu1
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    cubuntuoneservice.cpp \
    auth.cpp \
    cstorage.cpp \
    ctreeitem.cpp \
    ctreemodel.cpp \
    croottreeitem.cpp \
    ctableproxymodel.cpp \
    cuploaddialog.cpp \
    cubuntuonetask.cpp \
    ccheckfilesdialog.cpp

HEADERS  += mainwindow.h \
    cubuntuoneservice.h \
    auth.h \
    cglobals.h \
    cstorage.h \
    ctreeitem.h \
    ctreemodel.h \
    croottreeitem.h \
    ctableproxymodel.h \
    cuploaddialog.h \
    cubuntuonetask.h \
    ccheckfilesdialog.h

FORMS    += mainwindow.ui \
    cuploaddialog.ui \
    ccheckfilesdialog.ui

include(oauth/oauth.pri)
