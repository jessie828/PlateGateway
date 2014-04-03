
include ( ../PlateGateway.pri )

QT += network

TEMPLATE = lib

INCLUDEPATH = \
          . \
          $$PLATE_GATEWAY_SANDBOX_INC \
          $$PLATE_GATEWAY_THIRDPARTY_OPENCV_INC \
          $$PLATE_GATEWAY_THIRDPARTY_OPENCV_INC/opencv \
          $$PLATE_GATEWAY_THIRDPARTY_OPENCV_INC/opencv2 \
          $$PLATE_GATEWAY_THIRDPARTY_LEPTONICA_INC/leptonica\
          $$PLATE_GATEWAY_THIRDPARTY_TESSERACT_INC/tesseract \
          #

CONFIG += staticlib

TARGET = engine
DESTDIR = $$PLATE_GATEWAY_SANDBOX_LIB

SOURCES += \
        PlateGateway.cpp \
        PlateDetector.cpp \
        #

HEADERS += \
        PlateGateway.h \
        Muin.h \
        PlateDetector.h \
        #
