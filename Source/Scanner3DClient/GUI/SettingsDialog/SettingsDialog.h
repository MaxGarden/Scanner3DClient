#pragma once

#include "Services/Config/ConfigService.h"
#include "Services/Camera/CameraService.h"
#include "UI/ui_SettingsDialog.h"

#include <QDialog>
#include <QImage>

namespace Scanner3DClient::GUI
{
    class SettingsDialog final : public QDialog, public Ui_SettingsDialog
    {
        Q_OBJECT

    public:
        SettingsDialog(QWidget* parent, Services::ConfigService& configService, Services::CameraService& cameraService);
        virtual ~SettingsDialog() override final;

        virtual void open() override final;

    private:
        void AssignConfig(Services::ConfigService::Config&& config);
        void OnConfigResponse(std::optional<Services::ConfigService::Config> config);
        Services::ConfigService::Config CreateConfig() const noexcept;

        void OnCaptureImageResponse(std::vector<byte>&& image);

    protected:
        virtual void mouseDoubleClickEvent(QMouseEvent* event) override final;

    private:
        void SetPreview(std::vector<byte>&& imageData, std::optional<QPoint> newOrigin = std::nullopt);

    private slots:
        void OnISOSliderValueChanged(int value);
        void OnISOSpinBoxValueChanged(int value);
        void OnWidthSpinBoxValueChanged(int value);
        void OnHeightSpinBoxValueChanged(int value);

        void OnOkButtonClicked();
        void OnApplyButtonClicked();
        void OnRevertButtonClicked();

        void OnRefreshPreviewButtonClicked();
        
        void OnShowAdvancedScannerConfigChanged(int state);

        void OnRefreshPreviewOrigin();

    private:
        Services::ConfigService& m_configService;
        Services::CameraService& m_cameraService;

        Services::ConfigService::Config m_assignedConfig = {};

        std::vector<byte> m_imageData;

        static SettingsDialog* s_activeSettingsDialog;
    };
}