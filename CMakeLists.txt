CMAKE_MINIMUM_REQUIRED(VERSION 3.14)
PROJECT(Scanner3DClient)

SET(CMAKE_CXX_STANDARD 17)

SET(SCANNER_3D_CLIENT_SOURCES
    Source/pch.h
    Source/pch.cpp
    
    Source/Scanner3DClient/main.cpp
)

ADD_SUBDIRECTORY(Dependencies/RemoteServices)

ADD_EXECUTABLE(Scanner3DClient ${SCANNER_3D_CLIENT_SOURCES})

ADD_DEPENDENCIES(Scanner3DClient RemoteServices)

TARGET_INCLUDE_DIRECTORIES(Scanner3DClient PRIVATE Source Dependencies/RemoteServices/Include Dependencies/RemoteServices/Dependencies/Networking/Include)
TARGET_LINK_LIBRARIES(Scanner3DClient RemoteServices)

SET(CMAKE_INCLUDE_CURRENT_DIR ON)