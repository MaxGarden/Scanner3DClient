#pragma once

#include <QMainWindow>
#include "RemoteServices/DataModels/Client.h"
#include "RemoteServices/Controllers/ClientController.h"
#include "UI/ui_MainWindow.h"

namespace Scanner3DClient::GUI
{
    class MainWindow final : public QMainWindow, public Ui_MainWindow
    {
        Q_OBJECT

    public:
        MainWindow();
        virtual ~MainWindow() override final;

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