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
            unsigned char Brightness;       //0 to 100
            char Sharpness;                 //-100 to 100
            char Contrast;                  //-100 to 100
            unsigned short Iso;             //100 to 800
            char Saturation;                //-100 to 100
        };

        using GetConfigResponseCallback = std::function<void(std::optional<CameraConfig>)>;

    public:
        CameraService() = default;
        virtual ~CameraService() override final = default;

        bool SendGetConfigRequest(GetConfigResponseCallback&& callback);
        bool SendApplyConfigResult(const CameraConfig& config, GetConfigResponseCallback&& callback);

    private:
        void OnConfigResponse(const GetConfigResponseCallback& callback, Response&& response);
    };
}