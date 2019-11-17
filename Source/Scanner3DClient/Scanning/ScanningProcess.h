#pragma once

#include "Services/Tray/TrayService.h"
#include "Services/Config/ConfigService.h"
#include "Services/Scanner/ScannerService.h"

namespace Scanner3DClient::Scanning
{
    class ScanningProcess final
    {
    public:
        enum class State
        {
            Idle,
            Interrupted,
            Canceled,
            WaitingForConfig,
            ConfigReady,
            WaitingForScanner,
            ScannerReady,
            WaitingForTray,
            TrayReady,
            Completed
        };

        using OnScanStepCallback = std::function<void(std::vector<Services::ScannerService::Point3D>&&)>;

    public:
        ScanningProcess(Services::TrayService& trayService, Services::ConfigService& configService, Services::ScannerService& scannerService);
        ~ScanningProcess();

        bool Start(OnScanStepCallback&& onScanStepCallback);
        void Cancel();

        size_t GetRemainingStepsCount() const noexcept;
        size_t GetTotalStepsCount() const noexcept;

        float GetProgress() const noexcept;
        State GetCurrentState() const noexcept;

        bool Update();

    private:
        bool StateError() noexcept;
        void InvalidateReponseHande();

        bool OnConfigReady();
        bool OnScannerReady();
        bool OnTrayReady();

    private:
        Services::TrayService& m_trayService;
        Services::ConfigService& m_configService;
        Services::ScannerService& m_scannerService;

        Services::ConfigService::Config m_config = {};

        float m_currentTrayAngleInDegrees;
        size_t m_remaningStepsCount;
        State m_currentState = State::Idle;

        OnScanStepCallback m_onScanStepCallback;

        const std::map<State, std::function<bool()>> m_stateHandlers;

        RemoteServices::IResponseHandleSharedPtr m_responseHandle;
    };
}