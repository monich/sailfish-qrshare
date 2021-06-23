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
#include <QDebug>

#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

static QVector<uint> getPackageVersion(const char* aPackage)
{
    QVector<uint> version;
    int fds[2];
    if (pipe(fds) == 0) {
        pid_t pid = fork();
        if (!pid) {
            const char* argv[6];
            argv[0] = "rpm";
            argv[1] = "-q";
            argv[2] = "--qf";
            argv[3] = "%{version}";
            argv[4] = aPackage;
            argv[5] = NULL;
            while ((dup2(fds[1], STDOUT_FILENO) == -1) && (errno == EINTR));
            execvp(argv[0], (char**)argv);
            exit(1);
        }
        close(fds[1]);

        // There shouldn't be much output
        QByteArray out;
        const int chunk = 16;
        ssize_t n = 0;
        do {
            const int size = out.size();
            out.resize(size + chunk);
            while ((n = read(fds[0], out.data() + size, chunk)) == -1 && (errno == EINTR));
            out.resize(size + qMax(n, (ssize_t)0));
        } while (n > 0);

        // Parse the version
        if (out.size() > 0) {
            const char* v = out.constData();
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
        waitpid(pid, NULL, 0);
        close(fds[0]);
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
    // Detect native transferengine plugin API break
    QVector<uint> v = getPackageVersion("nemo-transferengine-qt5");
    // API break at 0.2.0
    if (v.count() >= 2 && v.at(0) == 0 && v.at(1) < 2) {
        iCreatePluginInfoFunc = QRShareCreatePluginInfo1;
    } else {
        // Detect QML API break
        v = getPackageVersion("declarative-transferengine-qt5");
        // API break at 0.4.0
        if (v.count() >= 2 && v.at(0) == 0 && v.at(1) < 4) {
            iCreatePluginInfoFunc = QRShareCreatePluginInfo2InProcess;
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
