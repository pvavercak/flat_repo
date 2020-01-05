QT       += core gui network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = servergui
TEMPLATE = app
DEFINES += QT_DEPRECATED_WARNINGS
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000
SOURCES += \
        main.cpp \
        servergui.cpp
HEADERS += \
        servergui.h
FORMS += \
        servergui.ui
