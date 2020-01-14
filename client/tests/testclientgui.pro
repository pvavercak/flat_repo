QT       += widgets testlib network

TARGET = TestClientGUI
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += \
                                ../

LIBS += \
		-L../clientgui \
		-lWidgetsLib

SOURCES += \
        testclientgui.cpp 

DEFINES += SRCDIR=\\\"$$PWD/\\\"
