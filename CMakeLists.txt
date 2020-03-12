CMAKE_MINIMUM_REQUIRED(VERSION 3.14)
PROJECT(Scanner3DClient)

SET(CMAKE_AUTOMOC ON)
SET(CMAKE_AUTORCC ON)
SET(CMAKE_AUTOUIC ON)
SET(CMAKE_AUTOMOC_MOC_OPTIONS -b <pch.h>)

SET(CMAKE_CXX_STANDARD 17)

FIND_PACKAGE(Qt5 COMPONENTS Core Widgets REQUIRED)
find_package(OpenGL REQUIRED)

SET(SCANNER_3D_CLIENT_SOURCES
    Source/pch.h
    Source/pch.cpp

    Source/Scanner3DClient/Scanner3DClient.h
    Source/Scanner3DClient/main.cpp

    Source/Scanner3DClient/Services/ClientServicesBuilder.h
    Source/Scanner3DClient/Services/ClientServicesBuilder.cpp

    Source/Scanner3DClient/Services/Camera/CameraService.h
    Source/Scanner3DClient/Services/Camera/CameraService.cpp

    Source/Scanner3DClient/Services/Config/ConfigService.h
    Source/Scanner3DClient/Services/Config/ConfigService.cpp

    Source/Scanner3DClient/Services/Scanner/ScannerService.h
    Source/Scanner3DClient/Services/Scanner/ScannerService.cpp

    Source/Scanner3DClient/Services/Tray/TrayService.h
    Source/Scanner3DClient/Services/Tray/TrayService.cpp

    Source/Scanner3DClient/GUI/MainWindow/MainWindow.h
    Source/Scanner3DClient/GUI/MainWindow/MainWindow.cpp

    Source/Scanner3DClient/GUI/ClientView/ClientView.h
    Source/Scanner3DClient/GUI/ClientView/ClientView.cpp

    Source/Scanner3DClient/GUI/ServicesView/ServicesView.h
    Source/Scanner3DClient/GUI/ServicesView/ServicesView.cpp

    Source/Scanner3DClient/GUI/SettingsDialog/SettingsDialog.h
    Source/Scanner3DClient/GUI/SettingsDialog/SettingsDialog.cpp

    Source/Scanner3DClient/GUI/ConnectDialog/ConnectDialog.h
    Source/Scanner3DClient/GUI/ConnectDialog/ConnectDialog.cpp

    Source/Scanner3DClient/GUI/ScanningResultWidget/ScanningResultWidget.h
    Source/Scanner3DClient/GUI/ScanningResultWidget/ScanningResultWidget.cpp

    Source/Scanner3DClient/Scanning/ScanningProcess.h
    Source/Scanner3DClient/Scanning/ScanningProcess.cpp
)

SET(GUI_TYPE)

IF(WIN32)
    SET(GUI_TYPE WIN32)
ENDIF(WIN32)

ADD_SUBDIRECTORY(Dependencies/RemoteServices)

ADD_EXECUTABLE(Scanner3DClient ${GUI_TYPE} ${SCANNER_3D_CLIENT_SOURCES})

ADD_DEPENDENCIES(Scanner3DClient RemoteServices)

TARGET_INCLUDE_DIRECTORIES(Scanner3DClient PRIVATE Source Source/Scanner3DClient Dependencies/RemoteServices/Include Dependencies/RemoteServices/Dependencies/Networking/Include)
TARGET_LINK_LIBRARIES(Scanner3DClient RemoteServices Qt5::Core Qt5::Widgets ${OPENGL_gl_LIBRARY})

SET(CMAKE_INCLUDE_CURRENT_DIR ON)
