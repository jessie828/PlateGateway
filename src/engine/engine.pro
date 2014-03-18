
include ( ../PlateGateway.pri )

QT += network

TEMPLATE = lib

INCLUDEPATH = \
          . \
          $$PLATE_GATEWAY_SANDBOX_INC \
          #

CONFIG += staticlib

TARGET = engine
DESTDIR = $$PLATE_GATEWAY_SANDBOX_LIB

MOC_DIR = ./moc_cpp

SOURCES += \
        PlateGateway.cpp \
        #

HEADERS += \
        PlateGateway.h \
        Muin.h \
        #
