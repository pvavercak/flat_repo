QT       += core gui network sql
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
TARGET = server
TEMPLATE = app
DEFINES += QT_DEPRECATED_WARNINGS CPU_ONLY BOOST_SYSTEM_NO_DEPRECATED BOOST_NO_CXX11_HDR_SYSTEM_ERROR
unix: PKGCONFIG += opencv
CONFIG += link_pkgconfig c++17 pthread
SOURCES += main.cpp \
           server.cpp \
           servergui.cpp \
           databaseconnection.cpp

HEADERS += server.h \
           servergui.h \
           databaseconnection.h

FORMS += server.ui

unix:!macx: LIBS += -L$$PWD/../depends/lib -lExtraction \
                    -L$$PWD/../depends/lib -lPreprocessing \
                    -L/usr/local/lib -lafcpu
INCLUDEPATH += $$PWD/../depends/includes
INCLUDEPATH += $$PWD/../preprocessor

DEFINES += "SRC_DIR=\\\"$$_PRO_FILE_PWD_\\\""
