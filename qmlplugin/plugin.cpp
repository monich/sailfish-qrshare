/*
 * Copyright (C) 2019-2020 Jolla Ltd.
 * Copyright (C) 2019-2020 Slava Monich <slava@monich.com>
 *
 * You may use this file under the terms of BSD license as follows:
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *   1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer
 *      in the documentation and/or other materials provided with the
 *      distribution.
 *   3. Neither the names of the copyright holders nor the names of its
 *      contributors may be used to endorse or promote products derived
 *      from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "HarbourAztecCodeGenerator.h"
#include "HarbourBase32.h"
#include "HarbourQrCodeGenerator.h"
#include "HarbourQrCodeImageProvider.h"
#include "HarbourDebug.h"

#include <QImage>
#include <QPainter>
#include <QQmlExtensionPlugin>
#include <QCoreApplication>
#include <QTranslator>
#include <QLocale>
#include <QDebug>

// ==========================================================================
// QRCodeUtils
// ==========================================================================

class QRCodeUtils : public QObject {
    Q_OBJECT

public:
    QRCodeUtils(QObject* aParent);

    Q_INVOKABLE QString saveToGallery(QString aBase32, QString aSubDir,
        QString aBaseName, int aScale);

    // Callback for qmlRegisterSingletonType<QRCodeUtils>
    static QObject* createSingleton(QQmlEngine* aEngine, QJSEngine* aScript);
};

QRCodeUtils::QRCodeUtils(QObject* aParent) :
    QObject(aParent)
{
}

QObject* QRCodeUtils::createSingleton(QQmlEngine* aEngine, QJSEngine*)
{
    return new QRCodeUtils(aEngine);
}

QString QRCodeUtils::saveToGallery(QString aBase32, QString aSubDir, QString aBaseName,
    int aScale)
{
    const QByteArray bits(HarbourBase32::fromBase32(aBase32.toLocal8Bit()));
    HDEBUG(aBase32 << "=>" << bits.size() << "bytes");
    QImage qrcode(HarbourQrCodeImageProvider::createImage(bits));
    if (!qrcode.isNull()) {
        // Draw one-pixel white square around QR code
        const int w = qrcode.width() + 2;
        const int h = qrcode.height() + 2;
        QImage img(w, h, QImage::Format_ARGB32);
        QPainter painter(&img);
        painter.fillRect(0, 0, w, h, Qt::white);
        painter.drawImage(1, 1, qrcode);
        painter.end();

        if (aScale > 1) {
            HDEBUG(w << "x" << h << "=>" <<  (w * aScale) << "x" << (h * aScale));
            img = img.scaledToHeight(h * aScale);
        }

        // Write the file
        QString destDir = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
        if (!destDir.isEmpty()) {
            if (!aSubDir.isEmpty()) {
                destDir += QDir::separator() + aSubDir;
            }
            if (aBaseName.isEmpty()) aBaseName = QLatin1String("qrcode");
            if (QFile::exists(destDir) || QDir(destDir).mkpath(destDir)) {
                static const QString suffix(".png");
                const QString prefix(destDir + QDir::separator() + aBaseName);
                QString destFile = prefix + suffix;
                for (int i = 1; QFile::exists(destFile); i++) {
                    destFile = prefix + QString().sprintf("-%03d", i) + suffix;
                }
                if (img.save(destFile)) {
                    HDEBUG(destFile);
                    return destFile;
                } else {
                    HWARN("Cannot save" << qPrintable(destFile));
                }
            } else {
                HWARN("Cannot create directory" << qPrintable(destDir));
            }
        }
    }
    return QString();
}

// ==========================================================================
// QRShareQmlExtensionPlugin
// ==========================================================================

class Q_DECL_EXPORT QRShareQmlExtensionPlugin : public QQmlExtensionPlugin {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QQmlExtensionInterface_iid)

public:
    void registerTypes(const char* aUri) Q_DECL_OVERRIDE;
    void initializeEngine(QQmlEngine *engine, const char *uri) Q_DECL_OVERRIDE;
};

void QRShareQmlExtensionPlugin::registerTypes(const char* aUri)
{
    HDEBUG(aUri);
    const int v1 = 1;
    const int v2 = 0;
    qmlRegisterSingletonType<QRCodeUtils>(aUri, v1, v2, "QRCodeUtils", QRCodeUtils::createSingleton);
    qmlRegisterType<HarbourQrCodeGenerator>(aUri, v1, v2, "QrCodeGenerator");
    qmlRegisterType<HarbourAztecCodeGenerator>(aUri, v1, v2, "AztecCodeGenerator");
}

void QRShareQmlExtensionPlugin::initializeEngine(QQmlEngine* aEngine, const char *aUri)
{
    HDEBUG(aUri);
    aEngine->addImageProvider("qrcode", new HarbourQrCodeImageProvider);

    // Load translations for share UI
    QTranslator* tr = new QTranslator(qApp);
    const QString filename(QRSHARE_TRANSLATIONS_FILE);
    const QString directory(QRSHARE_TRANSLATIONS_DIR);
    const QString prefix("-");
    const QString suffix(".qm");
    if (tr->load(QLocale(), filename, prefix, directory, suffix) ||
        tr->load(QLocale("en"), filename, prefix, directory, suffix)) {
        qApp->installTranslator(tr);
    } else {
        HWARN("Failed to load QR Share translations");
        delete tr;
    }
}

#include "plugin.moc"
