#include "pch.h"
#include "Networking/Client/WebSocketClient.h"
#include "RemoteServices/DataModels/Services.h"
#include "RemoteServices/DataModels/Client.h"
#include "RemoteServices/Controllers/ClientController.h"
#include "Services/ClientServicesBuilder.h"
#include <iostream>

using namespace Scanner3DClient;

int main()
{
    const auto fail = [](const auto returnCode, const auto& message)
    {
        std::cerr << message;
        return returnCode;
    };

    if(!Networking::Initialize())
        return fail(-1, "Cannot initialize networking.");

    auto websocketClient = Networking::IWebSocketClient::CreateEasyWs();
    if (!websocketClient)
        return fail(-2, "Cannot create web socket client.");

    const auto client = RemoteServices::IClientSharedPtr{ RemoteServices::IClient::Create(std::move(websocketClient)) };
    if (!client)
        return fail(-3, "Cannot create client data model.");

    const auto clientController = RemoteServices::IClientController::Create();
    if (!clientController)
        return fail(-4, "Cannot create client controller.");

    if (!clientController->SetDataModel(client))
        return fail(-5, "Client controller has not accepters client data model.");

    auto servicesBuilder = std::make_unique<ClientServicesBuilder>();
    if (!clientController->RegisterListener(std::move(servicesBuilder)))
        return fail(-6, "Cannot register services builder.");

    if (!client->Initialize())
        return fail(-7, "Cannot initialize server data model.");

    if (!client->ConnectToServer("192.168.130.60", 50485, 5000u))
        return fail(-8, "Cannot connect to server.");

    while (true)
        client->Update();

    client->Finalize();
    Networking::Finalize();

    return 0;
}