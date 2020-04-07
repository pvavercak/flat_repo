QT       += core gui network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = client
TEMPLATE = app
DEFINES += QT_DEPRECATED_WARNINGS CPU_ONLY BOOST_SYSTEM_NO_DEPRECATED BOOST_NO_CXX11_HDR_SYSTEM_ERROR
unix: PKGCONFIG += opencv
CONFIG += link_pkgconfig c++17 pthread
SOURCES += \
        main.cpp \
        clientgui.cpp \
        client.cpp \
        user.cpp
HEADERS += \
        clientgui.h \
        user.hpp \
        client.hpp
FORMS += \
        clientgui.ui
unix:!macx: LIBS += -L$$PWD/suprema_sdk/bin/x64/ -lUFScanner
INCLUDEPATH += $$PWD/suprema_sdk/include
DEPENDPATH += $$PWD/suprema_sdk/include
