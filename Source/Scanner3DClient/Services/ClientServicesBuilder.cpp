#include "pch.h"
#include "ClientServicesBuilder.h"

using namespace Scanner3DClient;

void ClientServicesBuilder::OnServerConnected(const RemoteServices::IServicesControllerSharedPtr& servicesController)
{
    RegisterServicesFactories(servicesController);
}

void ClientServicesBuilder::RegisterServicesFactories(const RemoteServices::IServicesControllerSharedPtr& servicesController)
{
    //TODO
}