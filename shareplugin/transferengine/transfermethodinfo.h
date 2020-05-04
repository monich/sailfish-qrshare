#ifndef TRANSFERMETHODINFO_H
#define TRANSFERMETHODINFO_H

#include <QString>
#include <QStringList>

#if TRANSFER_METHOD_INFO_VERSION == 1
#  define TransferMethodInfo TransferMethodInfo1
#elif TRANSFER_METHOD_INFO_VERSION == 2
#  include <QVariantHash>
#  define TransferMethodInfo TransferMethodInfo2
#else
#  error "Need to define TRANSFER_METHOD_INFO_VERSION"
#endif

class TransferMethodInfo {
public:
    inline TransferMethodInfo() : accountId(0) {}
    inline TransferMethodInfo &operator=(const TransferMethodInfo &other) {
        displayName = other.displayName;
        userName = other.userName;
        methodId = other.methodId;
        shareUIPath = other.shareUIPath;
        capabilitities = other.capabilitities;
        accountId = other.accountId;
#if TRANSFER_METHOD_INFO_VERSION > 1
        accountIcon = other.accountIcon;
        hints = other.hints;
#endif
        return *this;
    }
    inline TransferMethodInfo(const TransferMethodInfo &other) :
        displayName(other.displayName),
        userName(other.userName),
        methodId(other.methodId),
        shareUIPath(other.shareUIPath),
        capabilitities(other.capabilitities),
        accountId(other.accountId) {
#if TRANSFER_METHOD_INFO_VERSION > 1
        accountIcon = other.accountIcon;
        hints = other.hints;
#endif
    }
    inline ~TransferMethodInfo() {}

    QString displayName;
    QString userName;
    QString methodId;
    QString shareUIPath;
    QStringList capabilitities;
    quint32 accountId;
#if TRANSFER_METHOD_INFO_VERSION > 1
    QString accountIcon;
    QVariantHash hints;
#endif
};

#endif // TRANSFERMETHODINFO_H
