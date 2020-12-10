TEMPLATE = lib
TARGET = qrshareplugin
CONFIG += plugin link_pkgconfig
PKGCONFIG += nemotransferengine-qt5

include(../config.pri)

QMAKE_CXXFLAGS += -Wno-unused-parameter -fvisibility=hidden
QMAKE_CFLAGS += -fvisibility=hidden
QMAKE_LFLAGS += -fvisibility=hidden

INCLUDEPATH += \
    transferengine

DEFINES += \
    QRSHARE_UI_DIR=\\\"$$QRSHARE_UI_DIR\\\" \
    QRSHARE_TRANSLATIONS_FILE=\\\"$$QRSHARE_TRANSLATIONS_FILE\\\" \
    QRSHARE_TRANSLATIONS_DIR=\\\"$$QRSHARE_TRANSLATIONS_DIR\\\"

CONFIG(debug, debug|release) {
    DEFINES += DEBUG HARBOUR_DEBUG
}

OTHER_FILES += \
    transferengine/mediatransferinterface.h \
    transferengine/transfermethodinfo.h \
    transferengine/transferplugininfo.h \
    transferengine/transferplugininterface.h

HEADERS += \
    src/qrshare.h \
    src/qrshareplugin.h \
    src/qrshareplugininfo.h \
    src/qrsharetransfer.h

SOURCES += \
    src/librpm.c \
    src/qrshareplugin.cpp \
    src/qrshareplugininfo1.cpp \
    src/qrshareplugininfo2.cpp \
    src/qrsharetransfer.cpp

QRSHARE_UI_FILES = \
    qrshare.svg \
    qml/unhappy.svg \
    qml/QRShare.qml

OTHER_FILES += $$QRSHARE_UI_FILES

target.path = $$[QT_INSTALL_LIBS]/nemo-transferengine/plugins
INSTALLS += target

shareui.files = $$QRSHARE_UI_FILES
shareui.path = $$QRSHARE_UI_DIR
INSTALLS += shareui

OTHER_FILES += \
    translations/*

# Translations
TRANSLATION_SOURCES = \
  $${_PRO_FILE_PWD_}/qml \
  $${_PRO_FILE_PWD_}/src

defineTest(addTrFile) {
    in = $${_PRO_FILE_PWD_}/translations/$$1
    out = $${OUT_PWD}/translations/$$1

    s = $$replace(1,-,_)
    lupdate_target = lupdate_$$s
    lrelease_target = lrelease_$$s

    $${lupdate_target}.commands = lupdate -noobsolete -locations none $${TRANSLATION_SOURCES} -ts \"$${in}.ts\" && \
        mkdir -p \"$${OUT_PWD}/translations\" &&  [ \"$${in}.ts\" != \"$${out}.ts\" ] && \
        cp -af \"$${in}.ts\" \"$${out}.ts\" || :

    $${lrelease_target}.target = $${out}.qm
    $${lrelease_target}.depends = $${lupdate_target}
    $${lrelease_target}.commands = lrelease -idbased \"$${out}.ts\"

    QMAKE_EXTRA_TARGETS += $${lrelease_target} $${lupdate_target}
    PRE_TARGETDEPS += $${out}.qm
    qm.files += $${out}.qm

    export($${lupdate_target}.commands)
    export($${lrelease_target}.target)
    export($${lrelease_target}.depends)
    export($${lrelease_target}.commands)
    export(QMAKE_EXTRA_TARGETS)
    export(PRE_TARGETDEPS)
    export(qm.files)
}

LANGUAGES = cs es fr pl pt ru sv zh_CN hu

addTrFile($$QRSHARE_TRANSLATIONS_FILE)
for(l, LANGUAGES) {
    addTrFile($${QRSHARE_TRANSLATIONS_FILE}-$$l)
}

qm.path = $$QRSHARE_TRANSLATIONS_DIR
qm.CONFIG += no_check_exist
INSTALLS += qm
