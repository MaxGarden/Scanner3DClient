#include "pch.h"
#include "SettingsDialog.h"

#include <QMessageBox>

using namespace Scanner3DClient;
using namespace Scanner3DClient::GUI;

SettingsDialog* SettingsDialog::s_activeSettingsDialog = nullptr;

SettingsDialog::SettingsDialog(QWidget* parent, Services::ConfigService& configService, Services::CameraService& cameraService) :
    QDialog{ parent },
    m_configService{ configService },
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
    const auto result = m_configService.SendGetConfigRequest([this](const auto& cameraConfig)
    {
        if (this == s_activeSettingsDialog)
            OnConfigResponse(cameraConfig);
    });

    CLIENT_ASSERT(result);
    if (!result)
        close();

    return QDialog::show();
}

void SettingsDialog::AssignConfig(Services::ConfigService::Config&& config)
{
    m_assignedConfig = std::move(config);

    const auto& cameraConfig = m_assignedConfig.CameraConfig;
    m_widthSpinBox->setValue(cameraConfig.Width);
    m_heightSpinBox->setValue(cameraConfig.Height);
    m_brightnessSpinBox->setValue(cameraConfig.Brightness);
    m_sharpnessSpinBox->setValue(cameraConfig.Sharpness);
    m_contrastSpinBox->setValue(cameraConfig.Contrast);
    m_isoSpinBox->setValue(cameraConfig.ISO);
    m_saturationSpinBox->setValue(cameraConfig.Saturation);
}

void SettingsDialog::OnConfigResponse(std::optional<Services::ConfigService::Config> cameraConfig)
{
    CLIENT_ASSERT(cameraConfig);
    if (!cameraConfig)
        close();

    AssignConfig(std::move(*cameraConfig));

    setEnabled(true);
}

Services::ConfigService::Config SettingsDialog::CreateConfig() const noexcept
{
    auto result = m_assignedConfig;
    auto& cameraConfig = result.CameraConfig;

    cameraConfig.Width = m_widthSpinBox->value();
    cameraConfig.Height = m_heightSpinBox->value();
    cameraConfig.Brightness = m_brightnessSpinBox->value();
    cameraConfig.Sharpness = m_sharpnessSpinBox->value();
    cameraConfig.Contrast = m_contrastSpinBox->value();
    cameraConfig.ISO = m_isoSpinBox->value();
    cameraConfig.Saturation = m_saturationSpinBox->value();

    return result;
}

void SettingsDialog::OnCaptureImageResponse(std::vector<byte>&& image)
{
    if (image.empty())
        QMessageBox::critical(this, tr("Error"), tr("Captured image is empty!"));
    else
    {
        const auto& cameraConfig = m_assignedConfig.CameraConfig;
        const auto previewImage = QImage{ image.data(), cameraConfig.Width, cameraConfig.Height, QImage::Format_Grayscale8 };
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
    auto configToApply = CreateConfig();
    const auto result = m_configService.SendApplyConfigRequest(configToApply, nullptr);

    if (!result)
        QMessageBox::critical(this, tr("Error"), tr("Cannot send apply request!"));
    else
        close();
}

void SettingsDialog::OnApplyButtonClicked()
{
    auto configToApply = CreateConfig();
    const auto result = m_configService.SendApplyConfigRequest(configToApply, [this](const auto& cameraConfig)
    {
        if (this == s_activeSettingsDialog)
            OnConfigResponse(cameraConfig);
    });

    if (!result)
        QMessageBox::critical(this, tr("Error"), tr("Cannot send apply request!"));
    else
        setEnabled(false);
}

void SettingsDialog::OnRevertButtonClicked()
{
    AssignConfig(std::move(m_assignedConfig));
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