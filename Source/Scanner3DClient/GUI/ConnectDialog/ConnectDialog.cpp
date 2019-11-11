#include "pch.h"
#include "ConnectDialog.h"

#include <QMessageBox>

using namespace Scanner3DClient::GUI;

ConnectDialog::ConnectDialog(QWidget* parent, RemoteServices::IClient& client) :
    QDialog{ parent },
    m_client{ client }
{
    setupUi(this);
}

void ConnectDialog::OnConnectButtonClicked()
{
    const auto address = m_addressLineEdit->text().toStdString();
    const auto port = static_cast<unsigned short>(m_portSpinBox->value());

    if (!m_client.ConnectToServer(address, port))
        QMessageBox::critical(this, tr("Error"), QString{ "Cannot connect to %1:%2" }.arg(address.c_str()).arg(port));
    else
        close();
}