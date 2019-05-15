/*
 * Copyright (C) 2019 Jolla Ltd.
 * Copyright (C) 2019 Slava Monich <slava@monich.com>
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
#include <QDebug>

#include <rpm/rpmlib.h>
#include <rpm/header.h>
#include <rpm/rpmts.h>
#include <rpm/rpmdb.h>

QRSharePlugin::QRSharePlugin() :
    iCreatePluginInfoFunc(QRShareCreatePluginInfo2)
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
        const char* name = "nemo-transferengine-qt5";
        rpmts ts = rpmtsCreate();
        if (ts) {
            rpmdbMatchIterator mi = rpmtsInitIterator(ts, RPMDBI_NAME, name, 0);
            if (mi) {
                Header h = rpmdbNextIterator(mi);
                if (h) {
                    const char* v = headerGetString(h, RPMTAG_VERSION);
                    if (v) {
                        qDebug() << name << v;
                        // API break at 0.2.0
                        if (v[0] == '0' && v[1] == '.' && v[2] < '2') {
                            iCreatePluginInfoFunc = QRShareCreatePluginInfo1;
                        }
                    }
                }
                rpmdbFreeIterator(mi);
            }
        }
        rpmtsFree(ts);
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
