#include "pch.h"
#include "SettingsDialog.h"

#include <QMessageBox>

using namespace Scanner3DClient;
using namespace Scanner3DClient::GUI;

SettingsDialog* SettingsDialog::s_activeSettingsDialog = nullptr;

SettingsDialog::SettingsDialog(QWidget* parent, Services::CameraService& cameraService) :
    QDialog{ parent },
    m_cameraService{ cameraService }
{
    CLIENT_ASSERT(!s_activeSettingsDialog);
    s_activeSettingsDialog = this;

    setupUi(this);
}

SettingsDialog::~SettingsDialog()
{
    if(s_activeSettingsDialog == this)
        s_activeSettingsDialog = nullptr;
}

void SettingsDialog::open()
{
    setEnabled(false);
    const auto result = m_cameraService.SendGetConfigRequest([this](const auto& cameraConfig)
    {
        if (this == s_activeSettingsDialog)
            OnCameraConfigResponse(cameraConfig);
    });

    CLIENT_ASSERT(result);
    if (!result)
        close();

    return QDialog::show();
}

void SettingsDialog::AssignCameraConfig(Services::CameraService::CameraConfig&& cameraConfig)
{
    m_assignedCameraConfig = std::move(cameraConfig);

    m_widthSpinBox->setValue(m_assignedCameraConfig.Width);
    m_heightSpinBox->setValue(m_assignedCameraConfig.Height);
    m_brightnessSpinBox->setValue(m_assignedCameraConfig.Brightness);
    m_sharpnessSpinBox->setValue(m_assignedCameraConfig.Sharpness);
    m_contrastSpinBox->setValue(m_assignedCameraConfig.Contrast);
    m_isoSpinBox->setValue(m_assignedCameraConfig.ISO);
    m_saturationSpinBox->setValue(m_assignedCameraConfig.Saturation);
}

void SettingsDialog::OnCameraConfigResponse(std::optional<Services::CameraService::CameraConfig> cameraConfig)
{
    CLIENT_ASSERT(cameraConfig);
    if (!cameraConfig)
        close();

    AssignCameraConfig(std::move(*cameraConfig));

    setEnabled(true);
}

Services::CameraService::CameraConfig SettingsDialog::CreateCameraConfig() const noexcept
{
    auto result = m_assignedCameraConfig;

    result.Width = m_widthSpinBox->value();
    result.Height = m_heightSpinBox->value();
    result.Brightness = m_brightnessSpinBox->value();
    result.Sharpness = m_sharpnessSpinBox->value();
    result.Contrast = m_contrastSpinBox->value();
    result.ISO = m_isoSpinBox->value();
    result.Saturation = m_saturationSpinBox->value();

    return result;
}

void SettingsDialog::OnCaptureImageResponse(std::vector<byte>&& image)
{
    if (image.empty())
        QMessageBox::critical(this, tr("Error"), tr("Captured image is empty!"));
    else
    {
        const auto previewImage = QImage{ image.data(), m_assignedCameraConfig.Width, m_assignedCameraConfig.Height, QImage::Format_Grayscale8 };
        m_previewLabel->setPixmap(QPixmap::fromImage(previewImage));
        m_previewLabel->adjustSize();
    }

    m_previewGroupBox->setEnabled(true);
}

void SettingsDialog::OnISOSliderValueChanged(int value)
{
    if (m_isoSpinBox)
        m_isoSpinBox->setValue(value * 100);
}

static int AdjustValue(int value, int step)
{
    return static_cast<int>(round(static_cast<double>(value) / step) * step);
}

void SettingsDialog::OnISOSpinBoxValueChanged(int value)
{
    m_isoSlider->setValue(value / 100);
    m_isoSpinBox->setValue(AdjustValue(value, 100));
}

void SettingsDialog::OnWidthSpinBoxValueChanged(int value)
{
    m_widthSpinBox->setValue(AdjustValue(value, 320));
}

void SettingsDialog::OnHeightSpinBoxValueChanged(int value)
{
    m_heightSpinBox->setValue(AdjustValue(value, 240));
}

void SettingsDialog::OnOkButtonClicked()
{
    auto configToApply = CreateCameraConfig();
    const auto result = m_cameraService.SendApplyConfigRequest(configToApply, nullptr);

    if (!result)
        QMessageBox::critical(this, tr("Error"), tr("Cannot send apply request!"));
    else
        close();
}

void SettingsDialog::OnApplyButtonClicked()
{
    auto configToApply = CreateCameraConfig();
    const auto result = m_cameraService.SendApplyConfigRequest(configToApply, [this](const auto& cameraConfig)
    {
        if (this == s_activeSettingsDialog)
            OnCameraConfigResponse(cameraConfig);
    });

    if (!result)
        QMessageBox::critical(this, tr("Error"), tr("Cannot send apply request!"));
    else
        setEnabled(false);
}

void SettingsDialog::OnRevertButtonClicked()
{
    AssignCameraConfig(std::move(m_assignedCameraConfig));
}

void SettingsDialog::OnRefreshPreviewButtonClicked()
{
    const auto result = m_cameraService.SendCaptureImageRequest([this](auto&& image)
    {
        if (this == s_activeSettingsDialog)
            OnCaptureImageResponse(std::move(image));
    });

    CLIENT_ASSERT(result);
    if (!result)
        QMessageBox::critical(this, tr("Error"), tr("Cannot send capture request!"));
    else
        m_previewGroupBox->setEnabled(false);
}