#pragma once

#include "MVC/ViewBase.h"
#include "RemoteServices/Controllers/ServicesController.h"

#include <QWidget>

namespace Scanner3DClient::GUI
{
    class ServicesView final : public MVC::ViewBase<MVC::IView, RemoteServices::IServicesController>, public QWidget
    {
    public:
        ServicesView(QWidget* parent);
        virtual ~ServicesView() override final = default;

        void OnServicePaired(RemoteServices::IService& service);
        void OnServiceUnparied(RemoteServices::IService& service);

    protected:
        virtual MVC::IListenerUniquePtr CreateListener() override final;
    };
}