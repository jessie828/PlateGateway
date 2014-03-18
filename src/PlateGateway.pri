CONFIG += release warn_on
CONFIG += ordered

OBJECTS_DIR = $$(PLATE_GATEWAY_BUILD)/obj

PLATE_GATEWAY_SANDBOX     = $$(PLATE_GATEWAY_BUILD)
PLATE_GATEWAY_SANDBOX_BIN = $$PLATE_GATEWAY_SANDBOX/bin
PLATE_GATEWAY_SANDBOX_LIB = $$PLATE_GATEWAY_SANDBOX/lib
PLATE_GATEWAY_SANDBOX_INC = $$PLATE_GATEWAY_SANDBOX/include
PLATE_GATEWAY_SANDBOX_SRC = $$PLATE_GATEWAY_SANDBOX/src
PLATE_GATEWAY_SANDBOX_MOC = ./moc_cpp

PLATE_GATEWAY_SYSTEM_LIB = $$(PLATE_GATEWAY_SYSTEM_LIB)

QMAKE_CXXFLAGS += -fPIC -fmessage-length=0
QMAKE_CFLAGS   += -fPIC -fmessage-length=0 

# Additional optimizations for release build
QMAKE_CXXFLAGS += -finline-functions
QMAKE_CFLAGS   += -finline-functions

QMAKE_LIBDIR_X11 += /usr/X11R6/lib64