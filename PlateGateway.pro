# -------------------------------------------------
# Project created by QtCreator 2010-10-15T08:04:30
# -------------------------------------------------

INCLUDEPATH = \
        . \
        /usr/include/libavcodec \
        /usr/include/opencv \
        # 

LIBS += \
     -L/usr/local/lib \
     -lcxcore \
     -lcv \
     -lhighgui \
     -lcvaux \
     -lml \
     -lavformat \
     -lavutil \
     -lavcodec \
     -Wl \
     -ltesseract \
     #

TARGET = PlateGatewayQt
TEMPLATE = app
SOURCES += main.cpp \
    setupdialog.cpp \
    plategateway.cpp \
    plategatewayqt.cpp \
    plates.cpp
HEADERS += muin.h \
    setupdialog.h \
    plategateway.h \
    plategatewayqt.h \
    plates.h
FORMS += setupdialog.ui \
    plategatewayqt.ui \
    plates.ui

QMAKE_CXXFLAGS+=-DFF_API_CODEC_ID
QMAKE_CXXFLAGS+=-DFF_API_GET_BUFFER
QMAKE_CXXFLAGS+=-DFF_API_DESTRUCT_PACKET
QMAKE_CXXFLAGS+=-DFF_API_REQUEST_CHANNELS
QMAKE_CXXFLAGS+=-DFF_API_GET_BUFFER
QMAKE_CXXFLAGS+=-DFF_API_LOWRES
QMAKE_CXXFLAGS+=-DFF_API_ALLOC_CONTEXT
QMAKE_CXXFLAGS+=-DFF_API_AVCODEC_OPEN
QMAKE_CXXFLAGS+=-DFF_API_DESTRUCT_PACKET
QMAKE_CXXFLAGS+=-DFF_API_GET_BUFFER
QMAKE_CXXFLAGS+=-DFF_API_OLD_DECODE_AUDIO
QMAKE_CXXFLAGS+=-DFF_API_OLD_ENCODE_AUDIO
QMAKE_CXXFLAGS+=-DFF_API_OLD_ENCODE_VIDEO
QMAKE_CXXFLAGS+=-DFF_API_AVCODEC_RESAMPLE
QMAKE_CXXFLAGS+=-DFF_API_DEINTERLACE
QMAKE_CXXFLAGS+=-DAV_HAVE_INCOMPATIBLE_LIBAV_ABI
QMAKE_CXXFLAGS+=-DFF_API_MISSING_SAMPLE

#QMAKE_CXXFLAGS+=opencv

#QMAKE_CFLAGS+=opencv

