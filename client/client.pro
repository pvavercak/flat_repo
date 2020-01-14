QT       += core gui network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = client
TEMPLATE = app
DEFINES += QT_DEPRECATED_WARNINGS
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

unix:!macx: LIBS += -L$$PWD/../build-Extraction-Desktop-Debug/ -lExtraction

INCLUDEPATH += $$PWD/../extractor
DEPENDPATH += $$PWD/../extractor
