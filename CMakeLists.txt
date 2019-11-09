CMAKE_MINIMUM_REQUIRED(VERSION 3.14)
PROJECT(Scanner3DClient)

SET(CMAKE_AUTOMOC ON)
SET(CMAKE_AUTORCC ON)
SET(CMAKE_AUTOUIC ON)
SET(CMAKE_AUTOMOC_MOC_OPTIONS "-b<pch.h>")

SET(CMAKE_CXX_STANDARD 17)

FIND_PACKAGE(Qt5 COMPONENTS Core Widgets REQUIRED)

SET(SCANNER_3D_CLIENT_SOURCES
    Source/pch.h
    Source/pch.cpp
    
    Source/Scanner3DClient/Scanner3DClient.h
    Source/Scanner3DClient/main.cpp
    
    Source/Scanner3DClient/Services/ClientServicesBuilder.h
    Source/Scanner3DClient/Services/ClientServicesBuilder.cpp
    
    Source/Scanner3DClient/Services/Camera/CameraService.h
    Source/Scanner3DClient/Services/Camera/CameraService.cpp
    
    Source/Scanner3DClient/GUI/MainWindow/MainWindow.h
    Source/Scanner3DClient/GUI/MainWindow/MainWindow.cpp
    
    Source/Scanner3DClient/GUI/ClientView/ClientView.h
    Source/Scanner3DClient/GUI/ClientView/ClientView.cpp
    
    Source/Scanner3DClient/GUI/ServicesView/ServicesView.h
    Source/Scanner3DClient/GUI/ServicesView/ServicesView.cpp
)

ADD_SUBDIRECTORY(Dependencies/RemoteServices)

ADD_EXECUTABLE(Scanner3DClient ${SCANNER_3D_CLIENT_SOURCES})

ADD_DEPENDENCIES(Scanner3DClient RemoteServices)

TARGET_INCLUDE_DIRECTORIES(Scanner3DClient PRIVATE Source Source/Scanner3DClient Dependencies/RemoteServices/Include Dependencies/RemoteServices/Dependencies/Networking/Include)
TARGET_LINK_LIBRARIES(Scanner3DClient RemoteServices Qt5::Core Qt5::Widgets)

SET(CMAKE_INCLUDE_CURRENT_DIR ON)