
include ( ../PlateGateway.pri )

QT += network

TEMPLATE = lib

INCLUDEPATH = \
          . \
          $$PLATE_GATEWAY_SANDBOX_INC \
          $$PLATE_GATEWAY_SANDBOX_SRC/engine \
          #

CONFIG += staticlib

TARGET = helper
DESTDIR = $$PLATE_GATEWAY_SANDBOX_LIB

MOC_DIR = ./moc_cpp

SOURCES += \
        PlateValidationHelper.cpp \
        RenderVideoHelper.cpp \
        RenderHistogramHelper.cpp \
        #

HEADERS += \
        PlateValidationHelper.h \
        RenderVideoHelper.h \
        RenderHistogramHelper.h \
        #
