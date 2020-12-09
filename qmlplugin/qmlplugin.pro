TEMPLATE = lib
TARGET = qrshareqmlplugin
CONFIG += plugin link_pkgconfig
QT += qml quick
PKGCONFIG += glib-2.0

QMAKE_CXXFLAGS += -Wno-unused-parameter -fvisibility=hidden
QMAKE_CFLAGS += -std=c99 -Wno-sign-compare -fvisibility=hidden
QMAKE_LFLAGS += -fvisibility=hidden

CONFIG(debug, debug|release) {
    DEFINES += DEBUG HARBOUR_DEBUG
}

# Directories
HARBOUR_LIB_DIR = $${_PRO_FILE_PWD_}/harbour-lib
HARBOUR_LIB_INCLUDE = $${HARBOUR_LIB_DIR}/include
HARBOUR_LIB_SRC = $${HARBOUR_LIB_DIR}/src
LIBAZTEC_LIB_DIR = $${_PRO_FILE_PWD_}/libaztec
LIBAZTEC_LIB_INCLUDE = $${LIBAZTEC_LIB_DIR}/include
LIBAZTEC_LIB_SRC = $${LIBAZTEC_LIB_DIR}/src
LIBQRENCODE_DIR = $${_PRO_FILE_PWD_}/libqrencode

# Libraries
LIBS += libqrencode.a

INCLUDEPATH += \
  $${HARBOUR_LIB_INCLUDE} \
  $${LIBAZTEC_LIB_INCLUDE} \
  $${LIBQRENCODE_DIR}

HEADERS += \
    $${HARBOUR_LIB_INCLUDE}/HarbourAztecCodeGenerator.h \
    $${HARBOUR_LIB_INCLUDE}/HarbourBase32.h \
    $${HARBOUR_LIB_INCLUDE}/HarbourDebug.h \
    $${HARBOUR_LIB_INCLUDE}/HarbourImageProvider.h \
    $${HARBOUR_LIB_INCLUDE}/HarbourQrCodeGenerator.h \
    $${HARBOUR_LIB_INCLUDE}/HarbourQrCodeImageProvider.h \
    $${HARBOUR_LIB_INCLUDE}/HarbourSystem.h \
    $${HARBOUR_LIB_INCLUDE}/HarbourTask.h \
    $${HARBOUR_LIB_INCLUDE}/HarbourTheme.h

SOURCES += \
    $${HARBOUR_LIB_SRC}/HarbourAztecCodeGenerator.cpp \
    $${HARBOUR_LIB_SRC}/HarbourBase32.cpp \
    $${HARBOUR_LIB_SRC}/HarbourImageProvider.cpp \
    $${HARBOUR_LIB_SRC}/HarbourQrCodeGenerator.cpp \
    $${HARBOUR_LIB_SRC}/HarbourQrCodeImageProvider.cpp \
    $${HARBOUR_LIB_SRC}/HarbourSystem.cpp \
    $${HARBOUR_LIB_SRC}/HarbourTask.cpp \
    $${HARBOUR_LIB_SRC}/HarbourTheme.cpp

SOURCES += \
    $${LIBAZTEC_LIB_SRC}/aztec_bits.c \
    $${LIBAZTEC_LIB_SRC}/aztec_encode.c \
    $${LIBAZTEC_LIB_SRC}/aztec_rs.c

SOURCES += \
    plugin.cpp

OTHER_FILES += \
    qmldir

TARGET_PATH = $$[QT_INSTALL_QML]/com/monich/qrshare

target.path = $${TARGET_PATH}
INSTALLS += target

qmldir.path = $${TARGET_PATH}
qmldir.files += qmldir
INSTALLS += qmldir
