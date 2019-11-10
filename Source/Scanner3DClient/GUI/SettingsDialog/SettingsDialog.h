#pragma once

#include "Services/Camera/CameraService.h"
#include "UI/ui_SettingsDialog.h"

#include <QDialog>

namespace Scanner3DClient::GUI
{
    class SettingsDialog final : public QDialog, public Ui_SettingsDialog
    {
        Q_OBJECT

    public:
        SettingsDialog(QWidget* parent, Services::CameraService& cameraService);
        virtual ~SettingsDialog() override final;

        virtual void open() override final;

    private:
        void AssignCameraConfig(Services::CameraService::CameraConfig&& cameraConfig);
        void OnCameraConfigResponse(std::optional<Services::CameraService::CameraConfig> cameraConfig);
        Services::CameraService::CameraConfig CreateCameraConfig() const noexcept;

    private slots:
        void OnISOSliderValueChanged(int value);
        void OnISOSpinBoxValueChanged(int value);
        void OnWidthSpinBoxValueChanged(int value);
        void OnHeightSpinBoxValueChanged(int value);

        void OnOkButtonClicked();
        void OnApplyButtonClicked();
        void OnRevertButtonClicked();

    private:
        Services::CameraService& m_cameraService;
        Services::CameraService::CameraConfig m_assignedCameraConfig = {};

        static SettingsDialog* s_activeSettingsDialog;
    };
}