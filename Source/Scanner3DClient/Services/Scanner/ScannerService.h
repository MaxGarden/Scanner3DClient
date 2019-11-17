#pragma once

#include "RemoteServices/Services/SingleConnectionRequestResponseServiceBase.h"

namespace Scanner3DClient::Services
{
    class ScannerService final : public RemoteServices::SingleConnectionRequestResponseServiceBase
    {
    public:
        struct Point
        {
            unsigned short X;
            unsigned short Y;
        };

        struct Point3D
        {
            float X;
            float Y;
            float Z;
        };

        using CaptureBinarizedImageResponseCallback = std::function<void(std::vector<byte>&&)>;
        using CaptureAveragedPointsResponseCallback = std::function<void(std::vector<Point>&&)>;
        using Calculate3DPointsResponseCallback = std::function<void(std::vector<Point3D>&&)>;

    public:
        ScannerService() = default;
        virtual ~ScannerService() override final = default;

        RemoteServices::IResponseHandleSharedPtr SendCaptureBinarizedImageRequest(CaptureBinarizedImageResponseCallback&& callback);
        RemoteServices::IResponseHandleSharedPtr SendCaptureAveragedPointsRequest(CaptureAveragedPointsResponseCallback&& callback);
        RemoteServices::IResponseHandleSharedPtr SendCalculate3DPointsRequest(float trayAngle, Calculate3DPointsResponseCallback&& callback);
    };
}