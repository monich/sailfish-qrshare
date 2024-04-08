#ifndef SHARINGPLUGININTERFACE_H
#define SHARINGPLUGININTERFACE_H

#include <QtPlugin>

#include "sharingplugininfo.h"

class SharingPluginInterface
{
public:
    virtual SharingPluginInfo *infoObject() = 0;
    virtual QString pluginId() const = 0;
};

Q_DECLARE_INTERFACE(SharingPluginInterface, "org.sailfishos.SharingPluginInterface/1.0")

#endif // SHARINGPLUGININTERFACE_H
