#pragma once

#include "MVC/ViewBase.h"
#include "RemoteServices/Controllers/ClientController.h"
#include "UI/ui_ClientView.h"

#include <QWidget>

namespace Scanner3DClient::GUI
{
    class ClientView final : public MVC::ViewBase<MVC::IView, RemoteServices::IClientController>, public QWidget, public Ui_ClientView
    {
    public:
        ClientView(QWidget* parent);
        virtual ~ClientView() override final = default;

        void OnServerConnected(const RemoteServices::IServicesControllerSharedPtr& servicesController);
        void OnServerDisconnected(const RemoteServices::IServicesControllerSharedPtr& servicesController);

    protected:
        virtual void OnControllerChanged() override final;
        virtual MVC::IListenerUniquePtr CreateListener() override final;

    private:
        std::map<RemoteServices::IServicesControllerSharedPtr, ServicesViewUniquePtr> m_servicesViews;
    };
}