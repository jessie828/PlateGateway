
include ( ../PlateGateway.pri )

QT += network

TEMPLATE = lib

INCLUDEPATH = \
          . \
          $$PLATE_GATEWAY_SANDBOX_INC \
          #

CONFIG += staticlib

TARGET = util
DESTDIR = $$PLATE_GATEWAY_SANDBOX_LIB

SOURCES += \
        Utils.cpp \
        Wavelet.cpp \
        #

HEADERS += \
        Utils.h \
        Wavelet.h \
        #
