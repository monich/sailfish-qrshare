/*
 * Copyright (C) 2024 Slava Monich <slava@monich.com>
 *
 * You may use this file under the terms of the BSD license as follows:
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer
 *     in the documentation and/or other materials provided with the
 *     distribution.
 *
 *  3. Neither the names of the copyright holders nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation
 * are those of the authors and should not be interpreted as representing
 * any official policies, either expressed or implied.
 */

#include "qrshare.h"
#include "sharingplugininterface.h"

#include "HarbourDebug.h"

#include <QCoreApplication>
#include <QTranslator>
#include <QLocale>

class Q_DECL_EXPORT QRSharePlugin2 :
    public QObject,
    public SharingPluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.monich.qrshare.sharing.plugin")
    Q_INTERFACES(SharingPluginInterface)
    class PluginInfo;

public:
    QRSharePlugin2();
    SharingPluginInfo* infoObject() Q_DECL_OVERRIDE;
    QString pluginId() const Q_DECL_OVERRIDE;
};

//===========================================================================
// QRSharePlugin2::PluginInfo
//===========================================================================

class QRSharePlugin2::PluginInfo :
    public SharingPluginInfo
{
public:
    PluginInfo();

    QList<SharingMethodInfo> info() const Q_DECL_OVERRIDE;
    void query() Q_DECL_OVERRIDE;

private:
    QList<SharingMethodInfo> iInfoList;
};

QRSharePlugin2::PluginInfo::PluginInfo()
{
    SharingMethodInfo info;

    //: Plugin display name
    //% "QR Share"
    info.setDisplayName(qtTrId("qrshare-display_name"));
    info.setMethodId(QRSHARE_PLUGIN_ID);
    info.setMethodIcon(QRSHARE_ICON_SVG);
    info.setShareUIPath(QRSHARE_ITEM_PATH);
    info.setCapabilities(QStringList("text/*"));
    iInfoList.append(info);
}

QList<SharingMethodInfo>
QRSharePlugin2::PluginInfo::info() const
{
    return iInfoList;
}

void
QRSharePlugin2::PluginInfo::query()
{
    Q_EMIT infoReady();
}

//===========================================================================
// QRSharePlugin2
//===========================================================================

QRSharePlugin2::QRSharePlugin2()
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
        HWARN("Failed to load QR Share translations "
            QRSHARE_TRANSLATIONS_DIR "/" QRSHARE_TRANSLATIONS_FILE "*.qm");
        delete tr;
    }
}

SharingPluginInfo*
QRSharePlugin2::infoObject()
{
    return new PluginInfo;
}

QString
QRSharePlugin2::pluginId() const
{
    return QRSHARE_PLUGIN_ID;
}

#include "qrshareplugin2.moc"
