#ifndef MEDIATRANSFERINTERFACE_H
#define MEDIATRANSFERINTERFACE_H

#include <QObject>
#include <QString>
#include <QUrl>

class MediaTransferInterface: public QObject {
    Q_OBJECT
public:
    explicit MediaTransferInterface(QObject *parent = 0);
    virtual ~MediaTransferInterface();

    virtual QString displayName() const = 0;
    virtual QUrl serviceIcon() const = 0;
    virtual bool cancelEnabled() const = 0;
    virtual bool restartEnabled() const = 0;
    virtual void start() = 0;
    virtual void cancel() = 0;

private:
    void *d_ptr;
};

#endif // MEDIATRANSFERINTERFACE_H
