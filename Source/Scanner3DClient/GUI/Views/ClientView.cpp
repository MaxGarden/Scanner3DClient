#include "pch.h"
#include "ClientView.h"
#include "ServicesView.h"
#include "RemoteServices/Controllers/ClientControllerListenerBase.h"

using namespace Scanner3DClient::GUI;

class ClientViewListener final : public RemoteServices::ClientControllerListenerBase
{
public:
    ClientViewListener(ClientView& clientView);
    virtual ~ClientViewListener() override final = default;

    virtual void OnServerConnected(const RemoteServices::IServicesControllerSharedPtr& servicesController) override final;
    virtual void OnServerDisconnected(const RemoteServices::IServicesControllerSharedPtr& servicesController) override final;

private:
    ClientView& m_clientView;
};

ClientViewListener::ClientViewListener(ClientView& clientView) :
    m_clientView{ clientView }
{
}

void ClientViewListener::OnServerConnected(const RemoteServices::IServicesControllerSharedPtr& servicesController)
{
    m_clientView.OnServerConnected(servicesController);
}

void ClientViewListener::OnServerDisconnected(const RemoteServices::IServicesControllerSharedPtr& servicesController)
{
    m_clientView.OnServerDisconnected(servicesController);
}

ClientView::ClientView(QWidget* parent) :
    QWidget{ parent }
{
    setupUi(this);
}

void ClientView::OnServerConnected(const RemoteServices::IServicesControllerSharedPtr& servicesController)
{
    auto servicesView = std::make_unique<ServicesView>(this);
    if (!servicesView->SetController(servicesController))
    {
        CLIENT_ASSERT(false);
        return;
    }

    m_servicesViews.emplace(servicesController, std::move(servicesView));
}

void ClientView::OnServerDisconnected(const RemoteServices::IServicesControllerSharedPtr& servicesController)
{
    const auto iterator = m_servicesViews.find(servicesController);
    CLIENT_ASSERT(iterator != m_servicesViews.cend());
    if (iterator == m_servicesViews.cend())
        return;

    m_servicesViews.erase(iterator);
}

void ClientView::OnControllerChanged()
{
    while (!m_servicesViews.empty())
        OnServerDisconnected(m_servicesViews.begin()->first);

    const auto& controller = GetController();
    if (!controller)
        return;

    controller->VisitServers([this](const auto& servicesController)
    {
        OnServerConnected(servicesController);
    });
}

MVC::IListenerUniquePtr ClientView::CreateListener()
{
    return std::make_unique<ClientViewListener>(*this);
}