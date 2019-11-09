#pragma once

#include <QMainWindow>
#include "RemoteServices/DataModels/Client.h"
#include "RemoteServices/Controllers/ClientController.h"

namespace Scanner3DClient::GUI
{
    class CMainWindow final : public QMainWindow
    {
        Q_OBJECT

    public:
        CMainWindow();
        virtual ~CMainWindow() override final;

        bool Initialize();
        void Finalize();

    private slots:
        void OnUpdateTimer();

    private:
        bool m_initialized = false;

        std::unique_ptr<QTimer> m_updateTimer;

        RemoteServices::IClientSharedPtr m_client;
        RemoteServices::IClientControllerSharedPtr m_clientController;
    };
}