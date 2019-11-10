#pragma once

#include "RemoteServices/Services/SingleConnectionRequestResponseServiceBase.h"

namespace Scanner3DClient::Services
{
    class CameraService final : public RemoteServices::SingleConnectionRequestResponseServiceBase
    {
    public:
        struct CameraConfig
        {
            unsigned short Width;           //multiple of 320
            unsigned short Height;          //multiple of 240
            byte Brightness;                //0 to 100
            byte Sharpness;                 //0 to 100
            byte Contrast;                  //0 to 100
            unsigned short ISO;             //100 to 800
            byte Saturation;                //0 to 100
        };

        using GetConfigResponseCallback = std::function<void(std::optional<CameraConfig>)>;
        using CaptureImageResponseCallback = std::function<void(std::vector<byte>&&)>;

    public:
        CameraService() = default;
        virtual ~CameraService() override final = default;

        bool SendGetConfigRequest(GetConfigResponseCallback&& callback);
        bool SendApplyConfigRequest(const CameraConfig& config, GetConfigResponseCallback&& callback);
        bool SendCaptureImageRequest(CaptureImageResponseCallback&& callback);

    private:
        static void OnConfigResponse(const GetConfigResponseCallback& callback, Response&& response);
    };
}