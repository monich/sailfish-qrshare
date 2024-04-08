#ifndef SHARINGPLUGININFO_H
#define SHARINGPLUGININFO_H

#include <QList>
#include <QObject>
#include <QString>

#include "sharingmethodinfo.h"

class SharingPluginInfo:
    public QObject
{
    Q_OBJECT

public:
    virtual QList<SharingMethodInfo> info() const = 0;
    virtual void query() = 0;

Q_SIGNALS:
    void infoReady();
    void infoError(const QString&);
};

#endif // SHARINGPLUGININFO_H
