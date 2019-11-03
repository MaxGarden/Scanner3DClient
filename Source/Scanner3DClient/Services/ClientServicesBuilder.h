#pragma once

#include "RemoteServices/Controllers/ClientControllerListenerBase.h"

namespace Scanner3DClient
{
    class ClientServicesBuilder final : public RemoteServices::ClientControllerListenerBase
    {
    public:
        ClientServicesBuilder() = default;
        ~ClientServicesBuilder() override final = default;

        void OnServerConnected(const RemoteServices::IServicesControllerSharedPtr& servicesController) override final;

    private:
        void RegisterServicesFactories(const RemoteServices::IServicesControllerSharedPtr& servicesController);
    };
}