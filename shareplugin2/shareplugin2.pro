TEMPLATE = lib
TARGET = qrshareplugin2
CONFIG += plugin link_pkgconfig

include(../config.pri)

HARBOUR_LIB_DIR = ../harbour-lib
HARBOUR_LIB_INCLUDE = $${HARBOUR_LIB_DIR}/include
HARBOUR_LIB_SRC = $${HARBOUR_LIB_DIR}/src

QMAKE_CXXFLAGS += -Wno-unused-parameter -fvisibility=hidden
QMAKE_CFLAGS += -fvisibility=hidden
QMAKE_LFLAGS += -fvisibility=hidden

INCLUDEPATH += \
    ../shareplugin \
    transferengine \
    $${HARBOUR_LIB_INCLUDE}

DEFINES += \
    QRSHARE_UI_DIR=\\\"$$QRSHARE_UI_DIR\\\" \
    QRSHARE_TRANSLATIONS_FILE=\\\"$$QRSHARE_TRANSLATIONS_FILE\\\" \
    QRSHARE_TRANSLATIONS_DIR=\\\"$$QRSHARE_TRANSLATIONS_DIR\\\"

CONFIG(debug, debug|release) {
    DEFINES += DEBUG HARBOUR_DEBUG
}

OTHER_FILES += \
    transferengine/sharingmethodinfo.h \
    transferengine/sharingplugininfo.h \
    transferengine/sharingplugininterface.h

SOURCES += \
    src/qrshareplugin2.cpp

target.path = $$[QT_INSTALL_LIBS]/nemo-transferengine/plugins/sharing
INSTALLS += target
