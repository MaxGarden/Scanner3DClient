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
    //TODO
}

void ServicesView::OnServiceUnparied(RemoteServices::IService& service)
{
    //TODO
}

MVC::IListenerUniquePtr ServicesView::CreateListener()
{
    return std::make_unique<ServicesViewListener>(*this);
}

void ServicesView::OnSettingsButtonClicked()
{
    (new SettingsDialog(this))->show();
}