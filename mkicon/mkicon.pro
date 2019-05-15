TEMPLATE = app
TARGET = mkicon
QT += gui svg

silica {
    CONFIG += link_pkgconfig
    PKGCONFIG += sailfishsilica
    DEFINES += HAVE_SILICA
} else {
    LIBS += -ldl
}

SOURCES += mkicon.cpp

target.path = $$[QT_INSTALL_QML]/com/monich/qrshare

INSTALLS += target
