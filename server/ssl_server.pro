QT       += core gui network sql
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
TARGET = sslserver
TEMPLATE = app
DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
        main.cpp \
        sslserver.cpp \
        server.cpp \

HEADERS += \
        sslserver.h \
    server.h

FORMS += \
        sslserver.ui
