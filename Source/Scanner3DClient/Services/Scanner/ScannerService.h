#pragma once

#include "RemoteServices/Services/SingleConnectionRequestResponseServiceBase.h"

namespace Scanner3DClient::Services
{
    class ScannerService final : public RemoteServices::SingleConnectionRequestResponseServiceBase
    {
    public:
        using CaptureBinarizedImageResponseCallback = std::function<void(std::vector<byte>&&)>;

    public:
        ScannerService() = default;
        virtual ~ScannerService() override final = default;

        bool SendCaptureBinarizedImageRequest(CaptureBinarizedImageResponseCallback&& callback);
    };
}