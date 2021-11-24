QT       += core widgets

CONFIG += c++11

DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    widget.cpp

HEADERS += widget.h

FORMS += widget.ui

include('../../zenplayer/zenplayer.pri')
INCLUDEPATH += "../../zenplayer/"

DESTDIR = ../../bin
