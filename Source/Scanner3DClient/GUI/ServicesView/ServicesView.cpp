#include "pch.h"
#include "ServicesView.h"
#include "GUI/SettingsDialog/SettingsDialog.h"
#include "RemoteServices/Controllers/ServicesControllerListenerBase.h"

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
        const auto castedService = dynamic_cast<std::remove_reference<decltype(servicePointer)>::type>(&service);
        if (!castedService)
            return false;

        CLIENT_ASSERT(!servicePointer);
        servicePointer = castedService;
        return true;
    };

    const auto result = tryAssingService(m_configService) 
        || tryAssingService(m_cameraService) 
        || tryAssingService(m_scannerService);

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
        || tryUnassingService(m_scannerService);

    CLIENT_ASSERT(result);
}

MVC::IListenerUniquePtr ServicesView::CreateListener()
{
    return std::make_unique<ServicesViewListener>(*this);
}

void ServicesView::OnSettingsButtonClicked()
{
    if (!m_scannerService || !m_configService || !m_cameraService)
        return;

    (new SettingsDialog(this, *m_scannerService, *m_configService, *m_cameraService))->open();
}