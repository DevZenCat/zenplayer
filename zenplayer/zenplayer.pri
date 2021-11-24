QT += multimedia
CONFIG += c++lz

HEADERS += \
    $$PWD/zenplayer.h \
    $$PWD/zenplayerengine.h \
    $$PWD/zenplayerpresenter.h \


SOURCES += \
    $$PWD/zenplayerengine.cpp \
    $$PWD/zenplayerpresenter.cpp \


isQuick = $$find(QT, "quick")

count(isQuick, 1) {
    HEADERS += $$PWD/zenplayer.h $$PWD/zenplayerqmlcomponentvideo.h
    SOURCES += $$PWD/zenplayer.cpp $$PWD/zenplayerqmlcomponentvideo.cpp
}

isWidget = $$find(QT, "widgets")

count(isWidget, 1) {
    HEADERS += $$PWD/zenplayerwidget.h
    SOURCES += $$PWD/zenplayerwidget.cpp
}

INCLUDEPATH += $$PWD/3rdparty/ffmpeg/include/

LIBS += -lavcodec -lavformat -lavutil -lswscale -lswresample

win32 {
    LIBS += -L$$PWD/3rdparty/ffmpeg/lib/win/
    LIBS += -lGdi32 -lUser32 -lD3D11
}

mac {
    LIBS += -L$$PWD/3rdparty/ffmpeg/lib/mac/
    LIBS += -framework CoreFoundation -framework CoreGraphics -framework AudioToolbox -framework VideoToolbox -framework CoreMedia -framework CoreVideo -framework CoreImage -framework AVFoundation -framework IOSurface
    LIBS += -lbz2 -lz -lavdevice -lavfilter -lxslt -lzmq -lx264 -lx265 -lwebp -lwebpdecoder -lwebpdemux -lwebpmux -lxml2 -logg -lxvidcore -lvpx -lvorbis -lvorbisenc -ltheoraenc -lsrt -lspeex -lsoxr -lrav1e -lopenjp2
    LIBS += -lopus -ltiff -ltiffxx -lmp3lame -llzma -ltheoradec -lsodium -lsnappy -liconv -lgnutls -ldav1d -lbluray -laom -levent_openssl -lssl -lcrypto -lopencore-amrwb -lopencore-amrnb -lfreetype -lpng -lfontconfig -lexpat
}
