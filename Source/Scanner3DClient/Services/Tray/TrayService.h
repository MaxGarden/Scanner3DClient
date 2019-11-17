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

        RemoteServices::IResponseHandleSharedPtr SendStepForwardRequest(StepResponseCallback&& callback);
        RemoteServices::IResponseHandleSharedPtr SendStepBackwardRequest(StepResponseCallback&& callback);

    private:
        RemoteServices::IResponseHandleSharedPtr SendStepRequest(Request::RequestType requestType, StepResponseCallback&& callback);
    };
}