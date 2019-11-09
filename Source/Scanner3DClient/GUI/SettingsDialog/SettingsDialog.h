#pragma once

#include "UI/ui_SettingsDialog.h"

#include <QDialog>

namespace Scanner3DClient::GUI
{
    class SettingsDialog final : public QDialog, public Ui_SettingsDialog
    {
        Q_OBJECT

    public:
        SettingsDialog(QWidget* parent);
        virtual ~SettingsDialog() override final = default;
    };
}