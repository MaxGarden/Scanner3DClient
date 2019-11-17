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

        using CaptureBinarizedImageResponseCallback = std::function<void(std::vector<byte>&&)>;
        using CaptureAveragedPointsResponseCallback = std::function<void(std::vector<Point>&&)>;

    public:
        ScannerService() = default;
        virtual ~ScannerService() override final = default;

        RemoteServices::IResponseHandleSharedPtr SendCaptureBinarizedImageRequest(CaptureBinarizedImageResponseCallback&& callback);
        RemoteServices::IResponseHandleSharedPtr SendCaptureAveragedPointsRequest(CaptureAveragedPointsResponseCallback&& callback);
    };
}