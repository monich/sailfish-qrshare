TEMPLATE = lib
TARGET = qrshareplugin
CONFIG += plugin

NAME = sailfish-qrshare
QRSHARE_UI_DIR = /usr/share/$$NAME
QRSHARE_TRANSLATIONS_DIR = $${QRSHARE_UI_DIR}/translations

QMAKE_CXXFLAGS += -Wno-unused-parameter

INCLUDEPATH += \
    transferengine

DEFINES += \
    QRSHARE_UI_DIR=\\\"$$QRSHARE_UI_DIR\\\" \
    QRSHARE_TRANSLATIONS_FILE=\\\"$$NAME\\\" \
    QRSHARE_TRANSLATIONS_DIR=\\\"$$QRSHARE_TRANSLATIONS_DIR\\\"

CONFIG(debug, debug|release) {
    DEFINES += DEBUG HARBOUR_DEBUG
}

OTHER_FILES += \
    transferengine/mediatransferinterface.h \
    transferengine/transfermethodinfo1.h \
    transferengine/transfermethodinfo2.h \
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

OTHER_FILES += \
    qrshare.svg \
    qml/QRShare.qml

target.path = /usr/lib/nemo-transferengine/plugins
INSTALLS += target

shareui.files = qml/*.qml *svg
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

LANGUAGES = es fr pt ru sv zh_CN

addTrFile($$NAME)
for(l, LANGUAGES) {
    addTrFile($${NAME}-$$l)
}

qm.path = $$QRSHARE_TRANSLATIONS_DIR
qm.CONFIG += no_check_exist
INSTALLS += qm
