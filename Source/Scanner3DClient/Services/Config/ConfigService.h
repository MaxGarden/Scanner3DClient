#pragma once

#include "RemoteServices/Services/SingleConnectionRequestResponseServiceBase.h"

namespace Scanner3DClient::Services
{
    class ConfigService final : public RemoteServices::SingleConnectionRequestResponseServiceBase
    {
    public:
        struct Config
        {
            struct CameraConfig
            {
                unsigned short Width;           //multiple of 320
                unsigned short Height;          //multiple of 240
                byte Brightness;                //0 to 100
                byte Sharpness;                 //0 to 100
                byte Contrast;                  //0 to 100
                unsigned short ISO;             //100 to 800
                byte Saturation;                //0 to 100
                unsigned short Rotation;        //0 to 359
            } CameraConfig;

            struct ScannerConfig
            {
                byte TresholdMin;
                byte TresholdMax;

                struct Point
                {
                    unsigned short X;
                    unsigned short Y;
                } Origin;

                float CameraLaserInclinationInRad;
                float AxisCameraInclinationInRad;
            } ScannerConfig;

            struct TrayConfig
            {
                float MotorStepAngleInDegrees; //readonly
                unsigned short MotorStepsPerTrayStep; //from 1 to 6400
                unsigned short MotorStepDelayInMiliseconds;
            } TrayConfig;
        };

        using GetConfigResponseCallback = std::function<void(std::optional<Config>)>;

    public:
        ConfigService() = default;
        virtual ~ConfigService() override final = default;

        bool SendGetConfigRequest(GetConfigResponseCallback&& callback);
        bool SendApplyConfigRequest(const Config& config, GetConfigResponseCallback&& callback);

    private:
        static void OnConfigResponse(const GetConfigResponseCallback& callback, Response&& response);
    };
}