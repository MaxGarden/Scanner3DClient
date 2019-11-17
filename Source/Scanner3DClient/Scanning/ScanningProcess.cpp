#include "pch.h"
#include "ScanningProcess.h"

using namespace Scanner3DClient;
using namespace Scanner3DClient::Scanning;

ScanningProcess::ScanningProcess(Services::TrayService& trayService, Services::ConfigService& configService, Services::ScannerService& scannerService) :
    m_trayService{ trayService },
    m_configService{ configService },
    m_scannerService{ scannerService },
    m_stateHandlers
    {
        { State::Idle, []() { return true; } },
        { State::Interrupted, []() { return false; } },
        { State::Canceled, []() { return false; } },
        { State::WaitingForConfig, []() { return true; } },
        { State::ConfigReady, [this]() { return OnConfigReady(); } },
        { State::WaitingForScanner, []() { return true;} },
        { State::ScannerReady, [this]() { return OnScannerReady(); } },
        { State::WaitingForTray, []() { return true; } },
        { State::TrayReady, [this]() { return OnTrayReady(); } },
        { State::Completed, []() { return true; } }
    }
{
}

ScanningProcess::~ScanningProcess()
{
    InvalidateReponseHande();
}

bool ScanningProcess::Start(OnScanStepCallback&& onScanStepCallback)
{
    if (!onScanStepCallback)
        return false;

    InvalidateReponseHande();
    m_responseHandle = m_configService.SendGetConfigRequest([this](const auto& config)
    {
        if (!config)
        {
            StateError();
            return;
        }

        m_config = std::move(*config);

        m_currentTrayAngle = 0.0f;
        m_remaningStepsCount = GetTotalStepsCount();

        m_currentState = (m_remaningStepsCount > 0) ? State::ConfigReady : State::Completed;
    });

    if (!m_responseHandle)
        return false;

    m_onScanStepCallback = std::move(onScanStepCallback);
    return true;
}

void ScanningProcess::Cancel()
{
    m_onScanStepCallback = {};
    InvalidateReponseHande();
    m_currentState = State::Canceled;
}

size_t ScanningProcess::GetRemainingStepsCount() const noexcept
{
    return m_remaningStepsCount;
}

size_t ScanningProcess::GetTotalStepsCount() const noexcept
{
    const auto& trayConfig = m_config.TrayConfig;

    const auto trayStepInDegrees = trayConfig.MotorStepAngleInDegrees * trayConfig.MotorStepsPerTrayStep;
    if (trayStepInDegrees == 0.0f)
        return 0;

    return static_cast<size_t>(std::roundf(360.0f / trayStepInDegrees));
}

float ScanningProcess::GetProgress() const noexcept
{
    const auto totalStepsCount = GetTotalStepsCount();
    if (totalStepsCount == 0)
        return 0.0f;

    return static_cast<float>(GetRemainingStepsCount()) / totalStepsCount;
}

bool ScanningProcess::Update()
{
    const auto iterator = m_stateHandlers.find(m_currentState);
    CLIENT_ASSERT(iterator != m_stateHandlers.cend());

    if (iterator == m_stateHandlers.cend() || !iterator->second())
        return StateError();

    return true;
}

ScanningProcess::State ScanningProcess::GetCurrentState() const noexcept
{
    return m_currentState;
}

bool ScanningProcess::StateError() noexcept
{
    m_currentState = State::Interrupted;
    return false;
}

void ScanningProcess::InvalidateReponseHande()
{
    if (!m_responseHandle)
        return;
    
    m_responseHandle->Invalidate();
    m_responseHandle = nullptr;
}

bool ScanningProcess::OnConfigReady()
{
    return OnTrayReady();
}

bool ScanningProcess::OnScannerReady()
{
    InvalidateReponseHande();
    m_responseHandle = m_trayService.SendStepForwardRequest([this](const auto result)
    {
        if (!result)
        {
            StateError();
            return;
        }
        
        const auto& trayConfig = m_config.TrayConfig;
        m_currentTrayAngle += trayConfig.MotorStepAngleInDegrees * trayConfig.MotorStepsPerTrayStep;
        m_currentState = State::TrayReady;
    });

    if (!m_responseHandle)
        return StateError();

    m_currentState = State::WaitingForScanner;
    return true;
}

bool ScanningProcess::OnTrayReady()
{
    CLIENT_ASSERT(m_remaningStepsCount > 0);
    if (m_remaningStepsCount <= 0)
        return StateError();

    InvalidateReponseHande();
    m_responseHandle = m_scannerService.SendCalculate3DPointsRequest(m_currentTrayAngle, [this](auto&& points)
    {
        m_onScanStepCallback(std::move(points));
        m_currentState = --m_remaningStepsCount > 0 ? State::ScannerReady : State::Completed;
    });

    if (!m_responseHandle)
        return StateError();

    m_currentState = State::WaitingForScanner;
    return true;
}