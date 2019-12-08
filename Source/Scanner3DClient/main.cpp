#include "pch.h"
#include <QApplication>
#include "GUI/MainWindow/MainWindow.h"

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    Scanner3DClient::GUI::MainWindow mainWindow;

    if (!mainWindow.Initialize("raspberrypi.local", 50485))
        return -1;

    mainWindow.show();

    return app.exec();
}