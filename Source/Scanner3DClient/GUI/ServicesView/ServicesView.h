#pragma once

#include "MVC/ViewBase.h"
#include "RemoteServices/Controllers/ServicesController.h"
#include "UI/ui_ServicesView.h"
#include "Services/Config/ConfigService.h"
#include "Services/Camera/CameraService.h"
#include "Services/Scanner/ScannerService.h"
#include "Services/Tray/TrayService.h"
#include "Scanning/ScanningProcess.h"

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

        void Update();

    protected:
        virtual MVC::IListenerUniquePtr CreateListener() override final;

    private slots:
        void OnSettingsButtonClicked();
        void OnStartButtonClicked();

    private:
        Services::ConfigService* m_configService = nullptr;
        Services::CameraService* m_cameraService = nullptr;
        Services::ScannerService* m_scannerService = nullptr;
        Services::TrayService* m_trayService = nullptr;

        Scanning::ScanningProcessUniquePtr m_currentScanningProcess;
    };
}