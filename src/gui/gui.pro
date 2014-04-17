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
          $$PLATE_GATEWAY_SANDBOX_SRC/engine \
          $$PLATE_GATEWAY_SANDBOX_SRC/helper \
          $$PLATE_GATEWAY_THIRDPARTY_OPENCV_INC \
          $$PLATE_GATEWAY_THIRDPARTY_OPENCV_INC/opencv \
          $$PLATE_GATEWAY_THIRDPARTY_OPENCV_INC/opencv2 \
          $$PLATE_GATEWAY_THIRDPARTY_LEPTONICA_INC/leptonica\
          $$PLATE_GATEWAY_THIRDPARTY_TESSERACT_INC/tesseract \
          #

LIBS += \
     -L$$PLATE_GATEWAY_SANDBOX_LIB \
     -L$$PLATE_GATEWAY_THIRDPARTY_TESSERACT_LIB \
     -L$$PLATE_GATEWAY_THIRDPARTY_LEPTONICA_LIB \
     -lhelper \
     -lengine \
     -ltesseract \
     -llept \
     -lpthread \
     $$PLATE_GATEWAY_THIRDPARTY_OPENCV_LIB/libopencv_calib3d.so \
     $$PLATE_GATEWAY_THIRDPARTY_OPENCV_LIB/libopencv_contrib.so \
     $$PLATE_GATEWAY_THIRDPARTY_OPENCV_LIB/libopencv_core.so \
     $$PLATE_GATEWAY_THIRDPARTY_OPENCV_LIB/libopencv_features2d.so \
     $$PLATE_GATEWAY_THIRDPARTY_OPENCV_LIB/libopencv_flann.so \
     $$PLATE_GATEWAY_THIRDPARTY_OPENCV_LIB/libopencv_gpu.so \
     $$PLATE_GATEWAY_THIRDPARTY_OPENCV_LIB/libopencv_highgui.so \
     $$PLATE_GATEWAY_THIRDPARTY_OPENCV_LIB/libopencv_imgproc.so \
     $$PLATE_GATEWAY_THIRDPARTY_OPENCV_LIB/libopencv_legacy.so \
     $$PLATE_GATEWAY_THIRDPARTY_OPENCV_LIB/libopencv_ml.so \
     $$PLATE_GATEWAY_THIRDPARTY_OPENCV_LIB/libopencv_nonfree.so \
     $$PLATE_GATEWAY_THIRDPARTY_OPENCV_LIB/libopencv_objdetect.so \
     $$PLATE_GATEWAY_THIRDPARTY_OPENCV_LIB/libopencv_ocl.so \
     $$PLATE_GATEWAY_THIRDPARTY_OPENCV_LIB/libopencv_photo.so \
     $$PLATE_GATEWAY_THIRDPARTY_OPENCV_LIB/libopencv_stitching.so \
     $$PLATE_GATEWAY_THIRDPARTY_OPENCV_LIB/libopencv_superres.so \
     $$PLATE_GATEWAY_THIRDPARTY_OPENCV_LIB/libopencv_ts.a \
     $$PLATE_GATEWAY_THIRDPARTY_OPENCV_LIB/libopencv_video.so \
     $$PLATE_GATEWAY_THIRDPARTY_OPENCV_LIB/libopencv_videostab.so \
     -lrt \
     -lpthread \
     -lm \
     -ldl \ 
     #

SOURCES += \
    main.cpp \
    PlategatewayDialog.cpp \
    RenderVideo.cpp \
    RenderHistogram.cpp \
    #

HEADERS += \
    PlategatewayDialog.h \
    RenderVideo.h \
    RenderHistogram.h \
    #

FORMS += \
    PlategatewayDialogUi.ui \
    #
