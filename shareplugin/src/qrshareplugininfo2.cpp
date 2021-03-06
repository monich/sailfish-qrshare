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

#define TRANSFER_METHOD_INFO_VERSION 2
#include "transfermethodinfo.h"
#include "transferplugininfo.h"
#include "qrshareplugininfo.h"
#include "qrshare.h"

#include <QFileInfo>
#include <QUrl>

class QRSharePluginInfo2 : public TransferPluginInfo {
public:
    QRSharePluginInfo2(const char* aQmlPath);

    QList<TransferMethodInfo> info() const Q_DECL_OVERRIDE;
    void query() Q_DECL_OVERRIDE;
    bool ready() const Q_DECL_OVERRIDE;

private:
    QList<TransferMethodInfo> iInfoList;
};

QRSharePluginInfo2::QRSharePluginInfo2(const char* aQmlPath)
{
    TransferMethodInfo info;
    QFileInfo png(QRSHARE_ICON_PNG);

    //: Plugin display name
    //% "QR Share"
    info.displayName = qtTrId("qrshare-display_name");
    info.methodId = QRSHARE_PLUGIN_ID;
    info.shareUIPath = aQmlPath;
    info.capabilitities.append("text/*");
    info.accountIcon = QUrl::fromLocalFile(png.exists() ?
        png.absoluteFilePath() : QString(QRSHARE_ICON_SVG)).toString();

    iInfoList.append(info);
}

QList<TransferMethodInfo> QRSharePluginInfo2::info() const
{
    return iInfoList;
}

void QRSharePluginInfo2::query()
{
    Q_EMIT infoReady();
}

bool QRSharePluginInfo2::ready() const
{
    return true;
}

TransferPluginInfo* QRShareCreatePluginInfo2InProcess()
{
    // In-process sharing
    return new QRSharePluginInfo2(QRSHARE_PAGE_PATH);
}

TransferPluginInfo* QRShareCreatePluginInfo2OutOfProcess()
{
    // Out-of-process sharing (Sailfish OS >= 4.2.0)
    return new QRSharePluginInfo2(QRSHARE_ITEM_PATH);
}
