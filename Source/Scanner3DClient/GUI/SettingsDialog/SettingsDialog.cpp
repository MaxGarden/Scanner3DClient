#include "pch.h"
#include "SettingsDialog.h"

using namespace Scanner3DClient::GUI;

SettingsDialog::SettingsDialog(QWidget* parent) :
    QDialog{ parent }
{
    setupUi(this);
}