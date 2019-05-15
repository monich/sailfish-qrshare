#ifndef TRANSFERMETHODINFO_H
#define TRANSFERMETHODINFO_H

#include <QString>
#include <QStringList>

// Version < 0.2.0
class TransferMethodInfo {
public:
    TransferMethodInfo();
    TransferMethodInfo &operator=(const TransferMethodInfo &other);
    TransferMethodInfo(const TransferMethodInfo &other);
    ~TransferMethodInfo();

    QString displayName;
    QString userName;
    QString methodId;
    QString shareUIPath;
    QStringList capabilitities;
    quint32 accountId;
};

#endif // TRANSFERMETHODINFO_H
