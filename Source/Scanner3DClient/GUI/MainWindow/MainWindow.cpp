#include "pch.h"
#include "MainWindow.h"
#include "Networking/Client/WebSocketClient.h"
#include "Services/ClientServicesBuilder.h"
#include "GUI/ConnectDialog/ConnectDialog.h"

#include <QTimer>
#include <QMessageBox>

using namespace Scanner3DClient::GUI;

MainWindow::MainWindow() : 
    m_updateTimer{ std::make_unique<QTimer>(this) }
{
    setupUi(this);
}

MainWindow::~MainWindow()
{
    if (m_initialized)
        Finalize();
}

bool MainWindow::Initialize()
{
    CLIENT_ASSERT(!m_initialized);
    if (m_initialized)
        return false;

    const auto fail = [this](const auto& message)
    {
        QMessageBox::critical(this, tr("Fatal error"), tr(message));
        return false;
    };

    if(!Networking::Initialize())
        return fail("Cannot initialize networking.");

    auto websocketClient = Networking::IWebSocketClient::CreateEasyWs();
    if (!websocketClient)
        return fail("Cannot create web socket client.");

    m_client = RemoteServices::IClientSharedPtr{ RemoteServices::IClient::Create(std::move(websocketClient)) };
    if (!m_client)
        return fail("Cannot create client data model.");

    m_clientController = RemoteServices::IClientController::Create();
    if (!m_clientController)
        return fail("Cannot create client controller.");

    if (!m_clientController->SetDataModel(m_client))
        return fail("Client controller has not accepted client data model.");

    auto servicesBuilder = std::make_unique<ClientServicesBuilder>();
    if (!m_clientController->RegisterListener(std::move(servicesBuilder)))
        return fail("Cannot register services builder.");

    if (!m_clientView->SetController(m_clientController))
        return fail("Client view has not accepted client controller.");

    if (!m_client->Initialize())
        return fail("Cannot initialize server data model.");

    if (!connect(m_updateTimer.get(), SIGNAL(timeout()), this, SLOT(OnUpdateTimer())))
        return fail("Cannot connect update timer.");

    m_updateTimer->start(16);

    return (m_initialized = true);
}

void MainWindow::Finalize()
{
    CLIENT_ASSERT(m_initialized);
    if (!m_initialized)
        return;

    disconnect(m_updateTimer.get(), SIGNAL(timeout()), this, SLOT(OnUpdateTimer()));

    m_client->Finalize();
    Networking::Finalize();

    m_clientController.reset();
    m_client.reset();
}

void MainWindow::OnUpdateTimer()
{
    if (!m_initialized)
        return;

    if (m_client)
        m_client->Update();
}

void MainWindow::OnConnectToScannerTrigger()
{
    const auto settingsDialog = new ConnectDialog{ this, *m_client };

    settingsDialog->setAttribute(Qt::WA_DeleteOnClose);
    settingsDialog->open();
}