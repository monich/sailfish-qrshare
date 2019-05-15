TEMPLATE = subdirs
SUBDIRS = qmlplugin shareplugin qrencode mkicon

qmlplugin.depends = qrencode-target

qrencode.file = qmlplugin/qrencode.pro
qrencode.target = qrencode-target

OTHER_FILES += \
    rpm/*
