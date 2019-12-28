#include "pch.h"
#include "SettingsDialog.h"

#include <QPainter>
#include <QMessageBox>
#include <QMouseEvent>

using namespace Scanner3DClient;
using namespace Scanner3DClient::GUI;

SettingsDialog::SettingsDialog(QWidget* parent, Services::ScannerService& scannerService, Services::ConfigService& configService, Services::CameraService& cameraService, Services::TrayService& trayService) :
    QDialog{ parent },
    m_scanerService{ scannerService },
    m_configService{ configService },
    m_cameraService{ cameraService },
    m_trayService{ trayService }
{
    setupUi(this);
}

SettingsDialog::~SettingsDialog()
{
    if (m_configResponseHandle)
        m_configResponseHandle->Invalidate();

    if (m_previewResponseHandle)
        m_previewResponseHandle->Invalidate();

    if (m_trayResponseHandle)
        m_trayResponseHandle->Invalidate();
}

void SettingsDialog::open()
{
    setEnabled(false);

    if (m_configResponseHandle)
        m_configResponseHandle->Invalidate();

    m_configResponseHandle = m_configService.SendGetConfigRequest([this](const auto& config)
    {
        OnConfigResponse(config);
    });

    CLIENT_ASSERT(m_configResponseHandle);
    if (!m_configResponseHandle)
        close();

    return QDialog::show();
}

void SettingsDialog::AssignConfig(Services::ConfigService::Config&& config)
{
    static const auto radiansToDegrees = 180.0 / M_PI;

    m_assignedConfig = std::move(config);

    const auto& cameraConfig = m_assignedConfig.CameraConfig;
    m_widthSpinBox->setValue(cameraConfig.Width);
    m_heightSpinBox->setValue(cameraConfig.Height);
    m_brightnessSpinBox->setValue(cameraConfig.Brightness);
    m_sharpnessSpinBox->setValue(cameraConfig.Sharpness);
    m_contrastSpinBox->setValue(cameraConfig.Contrast);
    m_isoSpinBox->setValue(cameraConfig.ISO);
    m_saturationSpinBox->setValue(cameraConfig.Saturation);
    m_rotationSpinBox->setValue(cameraConfig.Rotation);

    const auto& scannerConfig = m_assignedConfig.ScannerConfig;
    m_binarizationTresholdMinSpinBox->setValue(scannerConfig.TresholdMin);
    m_binarizationTresholdMaxSpinBox->setValue(scannerConfig.TresholdMax);
    m_originXSpinBox->setValue(scannerConfig.Origin.X);
    m_originYSpinBox->setValue(scannerConfig.Origin.Y);
    m_cameraLaserInclinationDoubleSpinBox->setValue(static_cast<double>(scannerConfig.CameraLaserInclinationInRadians) * radiansToDegrees);
    m_axisCameraInclinationDoubleSpinBox->setValue(static_cast<double>(scannerConfig.AxisCameraInclinationInRadians) * radiansToDegrees);

    const auto& trayConfig = m_assignedConfig.TrayConfig;
    m_trayStepInDegreesDoubleSpinBox->setValue(static_cast<float>(trayConfig.MotorStepsPerTrayStep) * trayConfig.MotorStepAngleInDegrees);
    m_motorStepDelayInMilisecondsSpinBox->setValue(trayConfig.MotorStepDelayInMiliseconds);
}

void SettingsDialog::OnConfigResponse(std::optional<Services::ConfigService::Config> cameraConfig)
{
    CLIENT_ASSERT(cameraConfig);
    if (!cameraConfig)
        close();

    AssignConfig(std::move(*cameraConfig));
    RefreshPreview();

    setEnabled(true);
}

Services::ConfigService::Config SettingsDialog::CreateConfig() const noexcept
{
    static const auto degreesToRadians = M_PI / 180.0;

    auto result = m_assignedConfig;

    auto& cameraConfig = result.CameraConfig;
    cameraConfig.Width = static_cast<unsigned short>(m_widthSpinBox->value());
    cameraConfig.Height = static_cast<unsigned short>(m_heightSpinBox->value());
    cameraConfig.Brightness = static_cast<byte>(m_brightnessSpinBox->value());
    cameraConfig.Sharpness = static_cast<byte>(m_sharpnessSpinBox->value());
    cameraConfig.Contrast = static_cast<byte>(m_contrastSpinBox->value());
    cameraConfig.ISO = static_cast<unsigned short>(m_isoSpinBox->value());
    cameraConfig.Saturation = static_cast<byte>(m_saturationSpinBox->value());
    cameraConfig.Rotation = static_cast<unsigned short>(m_rotationSpinBox->value());

    auto& scannerConfig = result.ScannerConfig;
    scannerConfig.TresholdMin = static_cast<byte>(m_binarizationTresholdMinSpinBox->value());
    scannerConfig.TresholdMax = static_cast<byte>(m_binarizationTresholdMaxSpinBox->value());
    scannerConfig.Origin.X = static_cast<unsigned short>(m_originXSpinBox->value());
    scannerConfig.Origin.Y = static_cast<unsigned short>(m_originYSpinBox->value());
    scannerConfig.CameraLaserInclinationInRadians = static_cast<float>(m_cameraLaserInclinationDoubleSpinBox->value() * degreesToRadians);
    scannerConfig.AxisCameraInclinationInRadians = static_cast<float>(m_axisCameraInclinationDoubleSpinBox->value() * degreesToRadians);

    auto& trayConfig = result.TrayConfig;
    trayConfig.MotorStepsPerTrayStep = static_cast<unsigned short>(m_trayStepInDegreesDoubleSpinBox->value() / trayConfig.MotorStepAngleInDegrees);
    trayConfig.MotorStepDelayInMiliseconds = static_cast<unsigned short>(m_motorStepDelayInMilisecondsSpinBox->value());

    return result;
}

void SettingsDialog::OnCaptureImageResponse(std::vector<byte>&& image)
{
    if (image.empty())
        QMessageBox::critical(this, tr("Error"), tr("Captured image is empty!"));
    else
    {
        const auto& cameraConfig = m_assignedConfig.CameraConfig;
        SetPreview(std::move(image));
    }

    m_previewGroupBox->setEnabled(true);
}

void SettingsDialog::mouseDoubleClickEvent(QMouseEvent* event)
{
    const auto& globalPosition = event->globalPos();
    const auto newOrigin = m_previewLabel->mapFromGlobal(globalPosition);
    const auto& cameraConfig = m_assignedConfig.CameraConfig;
    const auto boundingRect = m_previewLabel->visibleRegion().boundingRect();

    if (boundingRect.contains(newOrigin))
    {
        m_originXSpinBox->setValue(newOrigin.x());
        m_originYSpinBox->setValue(newOrigin.y());
    }

    QDialog::mouseDoubleClickEvent(event);
}

void SettingsDialog::SetPreview(std::vector<byte>&& imageData, std::optional<QPoint> newOrigin)
{
    const auto& cameraConfig = m_assignedConfig.CameraConfig;
    m_imageData = std::move(imageData);

    auto imagePixmap = QPixmap::fromImage(QImage{ m_imageData.data(), cameraConfig.Width, cameraConfig.Height, QImage::Format_Grayscale8 });

    const auto drawPointer = [this, &imagePixmap](QPoint position, const auto& color)
    {
        if (position.x() < 0 || position.y() < 0 || position.x() >= imagePixmap.width() || position.y() >= imagePixmap.height())
            return;

        QPainter imagePainter{ &imagePixmap };
        imagePainter.setPen(color);
        imagePainter.drawPoint(position);
        imagePainter.drawEllipse(position, 10, 10);
    };

    if (newOrigin)
        drawPointer(*newOrigin, QColor::fromRgbF(1.0f, 0.0f, 0.0f));

    const auto& origin = m_assignedConfig.ScannerConfig.Origin;
    drawPointer({ origin.X, origin.Y }, QColor::fromRgbF(0.0f, 1.0f, 0.0f));

    m_previewLabel->setPixmap(imagePixmap);
    m_previewLabel->adjustSize();
}

void SettingsDialog::RefreshPreview()
{
    auto imageCallback = [this](auto&& image)
    {
        OnCaptureImageResponse(std::move(image));
    };

    if (m_previewResponseHandle)
        m_previewResponseHandle->Invalidate();

    if (m_rawPreviewRadioButton->isChecked())
        m_previewResponseHandle = m_cameraService.SendCaptureImageRequest(std::move(imageCallback));
    else if (m_binarizedPreviewRadioButton->isChecked())
        m_previewResponseHandle = m_scanerService.SendCaptureBinarizedImageRequest(std::move(imageCallback));
    else if (m_averagedPreviewRadioButton->isChecked())
    {
        m_previewResponseHandle = m_scanerService.SendCaptureAveragedPointsRequest([this, imageCallback = std::move(imageCallback)](auto&& points)
        {
            std::vector<byte> image;

            const auto& cameraConfig = m_assignedConfig.CameraConfig;
            image.resize(static_cast<size_t>(cameraConfig.Width) * cameraConfig.Height, 0);

            for (const auto& point : points)
            {
                const auto index = point.Y * cameraConfig.Width + point.X;
                image[index] = 255;
            }

            imageCallback(std::move(image));
        });
    }
    else
        CLIENT_ASSERT(false);

    CLIENT_ASSERT(m_previewResponseHandle);
    if (!m_previewResponseHandle)
        QMessageBox::critical(this, tr("Error"), tr("Cannot send capture request!"));
    else
        m_previewGroupBox->setEnabled(false);
}

void SettingsDialog::OnStepResponse(bool response)
{
    if(!response)
        QMessageBox::critical(this, tr("Error"), tr("Step failed!"));

    m_stepButtonsWidget->setEnabled(true);
}

void SettingsDialog::OnISOSliderValueChanged(int value)
{
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

void SettingsDialog::OnRotationSliderValueChanged(int value)
{
    m_rotationSpinBox->setValue(value * 90);
}

void SettingsDialog::OnRotationSpinBoxValueChanged(int value)
{
    m_rotationSlider->setValue(value / 90);
    m_rotationSpinBox->setValue(AdjustValue(value, 90));
}

void SettingsDialog::OnTrayStepInDegreesDoubleSpinBoxEditingFinished()
{
    const auto& trayConfig = m_assignedConfig.TrayConfig;
    const auto value = m_trayStepInDegreesDoubleSpinBox->value();
    m_trayStepInDegreesDoubleSpinBox->setValue(value - std::fmod(value, trayConfig.MotorStepAngleInDegrees));
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

    if (m_configResponseHandle)
        m_configResponseHandle->Invalidate();

    m_configResponseHandle = m_configService.SendApplyConfigRequest(configToApply, [this](const auto& cameraConfig)
    {
        OnConfigResponse(cameraConfig);
    });

    if (!m_configResponseHandle)
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
    RefreshPreview();
}

void SettingsDialog::OnShowAdvancedScannerConfigChanged(int state)
{
    const auto visible = state == Qt::Checked;

    m_scannerConfigAdvancedWidget->setVisible(visible);
    m_trayConfigAdvancedWidget->setVisible(visible);
}

void SettingsDialog::OnRefreshPreviewOrigin()
{
    const auto& cameraConfig = m_assignedConfig.CameraConfig;

    const auto& newOriginX = static_cast<unsigned short>(m_originXSpinBox->value());
    const auto& newOriginY = static_cast<unsigned short>(m_originYSpinBox->value());

    if (newOriginX >= 0 && newOriginY >= 0 && newOriginX < cameraConfig.Width && newOriginY <= cameraConfig.Height)
        SetPreview(std::move(m_imageData), std::make_optional<QPoint>(newOriginX, newOriginY));
}

void SettingsDialog::OnPreviewTypeRadioButtonToggled(bool toggled)
{
    if (toggled)
        RefreshPreview();
}

void SettingsDialog::OnStepForwardButtonClicked()
{
    if (m_trayResponseHandle)
        m_trayResponseHandle->Invalidate();

    m_trayResponseHandle = m_trayService.SendStepForwardRequest([this](auto result)
    {
        OnStepResponse(result);
    });

    if (!m_trayResponseHandle)
        QMessageBox::critical(this, tr("Error"), tr("Cannot send step forward request!"));
    else
        m_stepButtonsWidget->setEnabled(false);
}

void SettingsDialog::OnStepBackwardButtonClicked()
{
    if (m_trayResponseHandle)
        m_trayResponseHandle->Invalidate();

    m_trayResponseHandle = m_trayService.SendStepBackwardRequest([this](auto result)
    {
        OnStepResponse(result);
    });

    if (!m_trayResponseHandle)
        QMessageBox::critical(this, tr("Error"), tr("Cannot send step backward request!"));
    else
        m_stepButtonsWidget->setEnabled(false);
}