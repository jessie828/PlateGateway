# -------------------------------------------------
# Project created by skxawng828
# -------------------------------------------------

include( ../PlateGateway.pri )

TEMPLATE = app

DESTDIR = $$PLATE_GATEWAY_SANDBOX_BIN
MOC_DIR = ./moc_cpp
UI_SOURCES_DIR = ./uic_cpp
UI_HEADERS_DIR = ./uic_headers
TARGET = PlateGateway.exe

INCLUDEPATH = \
         . \
         ./uic_headers \
         $$PLATE_GATEWAY_SANDBOX_INC \
         $$PLATE_GATEWAY_SANDBOX_SRC/util \
         $$PLATE_GATEWAY_SANDBOX_SRC/engine \
         $$PLATE_GATEWAY_SANDBOX_SRC/helper \
         #

LIBS += \
     -L$$PLATE_GATEWAY_SANDBOX_LIB \
     -L$$PLATE_GATEWAY_SYSTEM_LIB \
     -lhelper \
     -lengine \
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

SOURCES += \
    main.cpp \
    SetupDialog.cpp \
    PlategatewayDialog.cpp \
    Plates.cpp \
    RenderVideo.cpp \
    RenderHistogram.cpp \
    #

HEADERS += \
    SetupDialog.h \
    PlategatewayDialog.h \
    Plates.h \
    RenderVideo.h \
    RenderHistogram.h \
    #

FORMS += \
    SetupDialogUi.ui \
    PlategatewayDialogUi.ui \
    PlatesUi.ui \
    #
