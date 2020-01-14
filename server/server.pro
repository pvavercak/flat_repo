QT       += core gui network sql
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
TARGET = server
TEMPLATE = app
DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
        main.cpp \
        sslserver.cpp \
        server.cpp \
    connection.cpp

HEADERS += \
        sslserver.h \
    server.h \
    connection.h

FORMS += \
        sslserver.ui
