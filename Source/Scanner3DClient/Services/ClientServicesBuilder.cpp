#include "pch.h"
#include "ClientServicesBuilder.h"
#include "RemoteServices/Services/ServiceFactory.h"

#include "Config/ConfigService.h"
#include "Camera/CameraService.h"
#include "Scanner/ScannerService.h"

using namespace Scanner3DClient;
using namespace Scanner3DClient::Services;

void ClientServicesBuilder::OnServerConnected(const RemoteServices::IServicesControllerSharedPtr& servicesController)
{
    RegisterServicesFactories(servicesController);
}

void ClientServicesBuilder::RegisterServicesFactories(const RemoteServices::IServicesControllerSharedPtr& servicesController)
{
    if (!servicesController)
        return;

    servicesController->RegisterServiceFactory(RemoteServices::ServiceFactory<ConfigService>::CreateFactory("config"));
    servicesController->RegisterServiceFactory(RemoteServices::ServiceFactory<CameraService>::CreateFactory("camera"));
    servicesController->RegisterServiceFactory(RemoteServices::ServiceFactory<ScannerService>::CreateFactory("scanner"));
}