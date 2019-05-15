#ifndef TRANSFERPLUGININTERFACE_H
#define TRANSFERPLUGININTERFACE_H

#include <QtPlugin>

class MediaTransferInterface;
class TransferPluginInfo;

class TransferPluginInterface {
public:
    virtual MediaTransferInterface * transferObject() = 0;
    virtual TransferPluginInfo *infoObject() = 0;
    virtual QString pluginId() const = 0;
    virtual bool enabled() const = 0;
};

Q_DECLARE_INTERFACE(TransferPluginInterface, "org.nemo.TransferPluginInterface/1.0")

#endif // TRANSFERPLUGININTERFACE_H
