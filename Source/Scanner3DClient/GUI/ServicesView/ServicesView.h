#pragma once

#include "MVC/ViewBase.h"
#include "RemoteServices/Controllers/ServicesController.h"
#include "UI/ui_ServicesView.h"
#include "Services/Camera/CameraService.h"

#include <QWidget>

namespace Scanner3DClient::GUI
{
    class ServicesView final : public QWidget, public MVC::ViewBase<MVC::IView, RemoteServices::IServicesController>, public Ui_ServicesView
    {
        Q_OBJECT

    public:
        ServicesView(QWidget* parent);
        virtual ~ServicesView() override final = default;

        void OnServicePaired(RemoteServices::IService& service);
        void OnServiceUnparied(RemoteServices::IService& service);

    protected:
        virtual MVC::IListenerUniquePtr CreateListener() override final;

    private slots:
        void OnSettingsButtonClicked();

    private:
        Services::CameraService* m_cameraService = nullptr;
    };
}