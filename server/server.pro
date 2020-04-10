QT       += core gui network sql
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
TARGET = server
TEMPLATE = app
QMAKE_CXXFLAGS += -Wl,--stack,100000000
QMAKE_CXXFLAGS += -Wl,--heap,100000000
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

unix:!macx: LIBS += -L$$PWD/../dep/libs -lExtraction \
                    -L$$PWD/../dep/libs -lPreprocessing \
                    -L$$PWD/../dep/libs -lMatcher \
                    -L/usr/local/lib -lafcpu

INCLUDEPATH += $$PWD/../dep/includes

DEFINES += "SRC_DIR=\\\"$$_PRO_FILE_PWD_\\\""
