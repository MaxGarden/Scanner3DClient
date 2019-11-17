#pragma once

#include "Services/Config/ConfigService.h"
#include "Services/Camera/CameraService.h"
#include "Services/Scanner/ScannerService.h"
#include "Services/Tray/TrayService.h"
#include "UI/ui_SettingsDialog.h"

#include <QDialog>
#include <QImage>

namespace Scanner3DClient::GUI
{
    class SettingsDialog final : public QDialog, public Ui_SettingsDialog
    {
        Q_OBJECT

    public:
        SettingsDialog(QWidget* parent, Services::ScannerService& scannerService, Services::ConfigService& configService, Services::CameraService& cameraService, Services::TrayService& trayService);
        virtual ~SettingsDialog() override final;

        virtual void open() override final;

    protected:
        virtual void mouseDoubleClickEvent(QMouseEvent* event) override final;

    private:
        void AssignConfig(Services::ConfigService::Config&& config);
        void OnConfigResponse(std::optional<Services::ConfigService::Config> config);
        Services::ConfigService::Config CreateConfig() const noexcept;

        void OnCaptureImageResponse(std::vector<byte>&& image);

        void SetPreview(std::vector<byte>&& imageData, std::optional<QPoint> newOrigin = std::nullopt);
        void RefreshPreview();

        void OnStepResponse(bool response);

    private slots:
        void OnISOSliderValueChanged(int value);
        void OnISOSpinBoxValueChanged(int value);
        void OnWidthSpinBoxValueChanged(int value);
        void OnHeightSpinBoxValueChanged(int value);
        void OnRotationSliderValueChanged(int value);
        void OnRotationSpinBoxValueChanged(int value);

        void OnTrayStepInDegreesDoubleSpinBoxEditingFinished();

        void OnOkButtonClicked();
        void OnApplyButtonClicked();
        void OnRevertButtonClicked();

        void OnRefreshPreviewButtonClicked();
        
        void OnShowAdvancedScannerConfigChanged(int state);

        void OnRefreshPreviewOrigin();
        void OnPreviewTypeRadioButtonToggled(bool toggled);

        void OnStepForwardButtonClicked();
        void OnStepBackwardButtonClicked();

    private:
        Services::ConfigService& m_configService;
        Services::CameraService& m_cameraService;
        Services::ScannerService& m_scanerService;
        Services::TrayService& m_trayService;

        Services::ConfigService::Config m_assignedConfig = {};

        std::vector<byte> m_imageData;

        RemoteServices::IResponseHandleSharedPtr m_configResponseHandle;
        RemoteServices::IResponseHandleSharedPtr m_previewResponseHandle;
        RemoteServices::IResponseHandleSharedPtr m_trayResponseHandle;
    };
}