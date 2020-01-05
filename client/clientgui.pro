QT       += core gui network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = clientgui
TEMPLATE = app
DEFINES += QT_DEPRECATED_WARNINGS
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
SOURCES += \
        main.cpp \
        clientgui.cpp \
        client.cpp \
        fphandler.cpp
HEADERS += \
        clientgui.h \
        client.h \
        fphandler.h
FORMS += \
        clientgui.ui
unix:!macx: LIBS += -L$$PWD/suprema_sdk/bin/x64/ -lUFScanner
INCLUDEPATH += $$PWD/suprema_sdk/include
DEPENDPATH += $$PWD/suprema_sdk/include