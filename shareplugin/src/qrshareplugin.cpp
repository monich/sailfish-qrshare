/*
 * Copyright (C) 2019-2021 Jolla Ltd.
 * Copyright (C) 2019-2021 Slava Monich <slava@monich.com>
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

#include "qrshare.h"
#include "qrshareplugin.h"
#include "qrsharetransfer.h"
#include "qrshareplugininfo.h"

#include <QCoreApplication>
#include <QTranslator>
#include <QLocale>
#include <QDir>
#include <QDebug>

#include <rpm/rpmlib.h>
#include <rpm/header.h>
#include <rpm/rpmts.h>
#include <rpm/rpmdb.h>

static QVector<uint> getPackageVersion(rpmts aRpmTs, const char* aPackage)
{
    QVector<uint> version;
    rpmdbMatchIterator mi = rpmtsInitIterator(aRpmTs, RPMDBI_NAME, aPackage, 0);
    if (mi) {
        Header h = rpmdbNextIterator(mi);
        if (h) {
            const char* v = headerGetString(h, RPMTAG_VERSION);
            if (v) {
                qDebug() << aPackage << v;
                const QStringList parts(QString(v).split('.', QString::SkipEmptyParts));
                const int n = qMin(parts.count(),3);
                for (int i = 0; i < n; i++) {
                    const QString part(parts.at(i));
                    bool ok = false;
                    int val = part.toUInt(&ok);
                    if (ok) {
                        version.append(val);
                    } else {
                        break;
                    }
                }
            }
        }
        rpmdbFreeIterator(mi);
    }
    return version;
}

QRSharePlugin::QRSharePlugin() :
    iCreatePluginInfoFunc(QRShareCreatePluginInfo2OutOfProcess)
{
    // Load translations
    QTranslator* tr = new QTranslator(this);
    const QString filename(QRSHARE_TRANSLATIONS_FILE);
    const QString directory(QRSHARE_TRANSLATIONS_DIR);
    const QString prefix("-");
    const QString suffix(".qm");
    if (tr->load(QLocale(), filename, prefix, directory, suffix) ||
        tr->load(QLocale("en"), filename, prefix, directory, suffix)) {
        qApp->installTranslator(tr);
    } else {
        qWarning() << "Failed to load QR Share translations "
            QRSHARE_TRANSLATIONS_DIR "/" QRSHARE_TRANSLATIONS_FILE "*.qm";
        delete tr;
    }

    // Figure out which version of plugin API is expected from us
    if (rpmReadConfigFiles(NULL, NULL) == 0) {
        rpmts ts = rpmtsCreate();
        if (ts) {
            // Detect native transferengine plugin API break
            QVector<uint> v = getPackageVersion(ts, "nemo-transferengine-qt5");
            // API break at 0.2.0
            if (v.count() >= 2 && v.at(0) == 0 && v.at(1) < 2) {
                iCreatePluginInfoFunc = QRShareCreatePluginInfo1;
            } else {
                // Detect QML API break
                v = getPackageVersion(ts, "declarative-transferengine-qt5");
                // API break at 0.4.0
                if (v.count() >= 2 && v.at(0) == 0 && v.at(1) < 4) {
                    iCreatePluginInfoFunc = QRShareCreatePluginInfo2InProcess;
                }
            }
            rpmtsFree(ts);
        }
    } else {
        // Check what's the latest librpm.so.x in /usr/lib
        QDir dir("/usr/lib");
        const QString prefix("librpm.so.");
        QFileInfoList list = dir.entryInfoList(QDir::Files |
            QDir::NoDotAndDotDot, QDir::NoSort);

        int i, x = -1;
        const int n = list.count();
        for (i = 0; i < n; i++) {
            const QFileInfo& info = list.at(i);
            if (info.isFile()) {
                const QString name(info.fileName());
                if (name.startsWith(prefix)) {
                    QString suffix(name.mid(prefix.length()));
                    const int dot = suffix.indexOf('.');
                    if (dot > 0) suffix.truncate(dot);
                    bool isNumber = false;
                    const int x2 = suffix.toInt(&isNumber);
                    if (isNumber && x2 >= x) {
                        x = x2;
                    }
                }
            }
        }
        if (x >= 0 && x < 8) {
            qDebug() << "Using API v1";
            iCreatePluginInfoFunc = QRShareCreatePluginInfo1;
        }
    }
}

QRSharePlugin::~QRSharePlugin()
{
}

MediaTransferInterface* QRSharePlugin::transferObject()
{
    return new QRShareTransfer;
}

TransferPluginInfo* QRSharePlugin::infoObject()
{
    return iCreatePluginInfoFunc();
}

QString QRSharePlugin::pluginId() const
{
    return QRSHARE_PLUGIN_ID;
}

bool QRSharePlugin::enabled() const
{
    return true;
}
