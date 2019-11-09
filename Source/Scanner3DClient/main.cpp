#include "pch.h"
#include <QApplication>
#include "MainWindow/MainWindow.h"

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    Scanner3DClient::GUI::CMainWindow mainWindow;

    if (!mainWindow.Initialize())
        return -1;

    mainWindow.show();

    return app.exec();
}