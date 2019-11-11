#pragma once

#include "RemoteServices/Services/SingleConnectionRequestResponseServiceBase.h"

namespace Scanner3DClient::Services
{
    class CameraService final : public RemoteServices::SingleConnectionRequestResponseServiceBase
    {
    public:
        using CaptureImageResponseCallback = std::function<void(std::vector<byte>&&)>;

    public:
        CameraService() = default;
        virtual ~CameraService() override final = default;

        bool SendCaptureImageRequest(CaptureImageResponseCallback&& callback);
    };
}