#pragma once

#include "RemoteServices/DataModels/Client.h"
#include "UI/ui_ConnectDialog.h"

#include <QDialog>

namespace Scanner3DClient::GUI
{
    class ConnectDialog final : public QDialog, public Ui_ConnectDialog
    {
        Q_OBJECT

    public:
        ConnectDialog(QWidget* parent, RemoteServices::IClient& client);
        virtual ~ConnectDialog() override final = default;

    private slots:
        void OnConnectButtonClicked();

    private:
        RemoteServices::IClient& m_client;
    };
}