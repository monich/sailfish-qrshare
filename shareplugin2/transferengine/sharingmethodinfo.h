#ifndef SHARINGMETHODINFO_H
#define SHARINGMETHODINFO_H

#include <QStringList>

class SharingMethodInfoPrivate;
class SharingMethodInfo
{
public:
    SharingMethodInfo();
    SharingMethodInfo &operator=(const SharingMethodInfo&);
    SharingMethodInfo(const SharingMethodInfo &other);
    ~SharingMethodInfo();

    void setDisplayName(const QString&);
    void setMethodId(const QString&);
    void setMethodIcon(const QString&);
    void setShareUIPath(const QString&);
    void setCapabilities(const QStringList&);

private:
    SharingMethodInfoPrivate* d_ptr;
    Q_DECLARE_PRIVATE(SharingMethodInfo);
};

#endif // SHARINGMETHODINFO_H
