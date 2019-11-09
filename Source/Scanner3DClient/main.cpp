#include "pch.h"
#include <QApplication>
#include "GUI/MainWindow/MainWindow.h"

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    Scanner3DClient::GUI::MainWindow mainWindow;

    if (!mainWindow.Initialize())
        return -1;

    mainWindow.show();

    return app.exec();
}