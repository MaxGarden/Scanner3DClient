#pragma once

#include "RemoteServices/Services/SingleConnectionRequestResponseServiceBase.h"

namespace Scanner3DClient::Services
{
    class TrayService final : public RemoteServices::SingleConnectionRequestResponseServiceBase
    {
    public:
        using StepResponseCallback = std::function<void(bool)>;

    public:
        TrayService() = default;
        virtual ~TrayService() override final = default;

        bool SendStepForwardRequest(StepResponseCallback&& callback);
        bool SendStepBackwardRequest(StepResponseCallback&& callback);

    private:
        bool SendStepRequest(Request::RequestType requestType, StepResponseCallback&& callback);
    };
}