#ifndef TRANSFERPLUGININFO_H
#define TRANSFERPLUGININFO_H

#include <QObject>
#include <QString>
#include <QList>

class TransferPluginInfo: public QObject {
    Q_OBJECT
public:
    virtual QList<TransferMethodInfo> info() const = 0;
    virtual void query() = 0;
    virtual bool ready() const = 0;

Q_SIGNALS:
    void infoReady();
    void infoError(const QString &msg);
};

#endif // TRANSFERPLUGINSTATUS_H
