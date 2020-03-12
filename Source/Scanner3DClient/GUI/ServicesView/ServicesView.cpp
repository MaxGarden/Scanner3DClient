#include "pch.h"
#include "ServicesView.h"
#include "GUI/SettingsDialog/SettingsDialog.h"
#include "RemoteServices/Controllers/ServicesControllerListenerBase.h"

#include <QMessageBox>

using namespace Scanner3DClient::GUI;

class ServicesViewListener final : public RemoteServices::ServicesControllerListenerBase
{
public:
    ServicesViewListener(ServicesView& servicesView);
    virtual ~ServicesViewListener() override final = default;

    virtual void OnServicePaired(RemoteServices::IService& service) override final;
    virtual void OnServiceUnparied(RemoteServices::IService& service) override final;

private:
    ServicesView& m_servicesView;
};

ServicesViewListener::ServicesViewListener(ServicesView& servicesView) :
    m_servicesView{ servicesView }
{
}

void ServicesViewListener::OnServicePaired(RemoteServices::IService& service)
{
    m_servicesView.OnServicePaired(service);
}

void ServicesViewListener::OnServiceUnparied(RemoteServices::IService& service)
{
    m_servicesView.OnServiceUnparied(service);
}

ServicesView::ServicesView(QWidget* parent) :
    QWidget{ parent }
{
    setupUi(this);
}

void ServicesView::OnServicePaired(RemoteServices::IService& service)
{
    const auto tryAssingService = [&service](auto*& servicePointer)
    {
        const auto castedService = dynamic_cast<typename std::remove_reference<decltype(servicePointer)>::type>(&service);
        if (!castedService)
            return false;

        CLIENT_ASSERT(!servicePointer);
        servicePointer = castedService;
        return true;
    };

    const auto result = tryAssingService(m_configService)
        || tryAssingService(m_cameraService)
        || tryAssingService(m_scannerService)
        || tryAssingService(m_trayService);

    CLIENT_ASSERT(result);
}

void ServicesView::OnServiceUnparied(RemoteServices::IService& service)
{
    const auto tryUnassingService = [&service](auto*& servicePointer)
    {
        if (servicePointer == &service)
        {
            servicePointer = nullptr;
            return true;
        }

        return false;
    };

    const auto result = tryUnassingService(m_configService)
        || tryUnassingService(m_cameraService)
        || tryUnassingService(m_scannerService)
        || tryUnassingService(m_trayService);

    CLIENT_ASSERT(result);
}

void ServicesView::Update()
{
    m_settingsPushButton->setEnabled(!m_currentScanningProcess 
        || m_currentScanningProcess->GetCurrentState() == Scanning::ScanningProcess::State::Completed);

    if (m_currentScanningProcess)
    {
        if (!m_currentScanningProcess->Update())
        {
            QMessageBox::critical(this, tr("Error"), tr("Scanning error"));
            m_currentScanningProcess.reset();
        }

        m_resultProgressBar->setValue(m_currentScanningProcess->GetProgress() * 100.0f);
        m_startPushButton->setText("Abort");

        if (m_currentScanningProcess->GetCurrentState() == Scanning::ScanningProcess::State::Completed)
            m_currentScanningProcess.reset();
    }
    else
        m_startPushButton->setText("Start");

    m_scanningResultWidget->update();
}

MVC::IListenerUniquePtr ServicesView::CreateListener()
{
    return std::make_unique<ServicesViewListener>(*this);
}

void ServicesView::OnSettingsButtonClicked()
{
    if (!m_trayService || !m_scannerService || !m_configService || !m_cameraService)
    {
        QMessageBox::critical(this, tr("Error"), tr("Missing service!"));
        return;
    }

    const auto settingsDialog = new SettingsDialog{ this, *m_scannerService, *m_configService, *m_cameraService, *m_trayService };
    
    settingsDialog->setAttribute(Qt::WA_DeleteOnClose);
    settingsDialog->open();
}

void ServicesView::OnStartButtonClicked()
{
    if (!m_trayService || !m_configService || !m_scannerService)
    {
        QMessageBox::critical(this, tr("Error"), tr("Missing service!"));
        return;
    }

    if (m_currentScanningProcess)
    {
        m_currentScanningProcess->Cancel();
        m_currentScanningProcess.reset();
    }
    else
    {
        m_scanningResultWidget->ClearPoints();
        m_currentScanningProcess = std::make_unique<Scanning::ScanningProcess>(*m_trayService, *m_configService, *m_scannerService);

        m_currentScanningProcess->Start([this](auto&& points)
        {
            m_scanningResultWidget->AddPoints(std::move(points));
        });
    }
}
