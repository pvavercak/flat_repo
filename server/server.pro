QT       += core gui network sql
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
TARGET = server
TEMPLATE = app
DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
        main.cpp \
        server.cpp \
    connection.cpp \
    servergui.cpp

HEADERS += \
    server.h \
    connection.h \
    servergui.h

FORMS += \
        sslserver.ui
