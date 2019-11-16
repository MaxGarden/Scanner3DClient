#include "pch.h"
#include "TrayService.h"

using namespace Scanner3DClient::Services;

bool TrayService::SendStepForward(StepResponseCallback&& callback)
{
    return SendStepRequest('f', std::move(callback));
}

bool TrayService::SendStepBackward(StepResponseCallback&& callback)
{
    return SendStepRequest('b', std::move(callback));
}

bool TrayService::SendStepRequest(Request::RequestType requestType, StepResponseCallback&& callback)
{
    return SendRequest(Request{ requestType, {} }, [callback = std::move(callback)](auto&& response)
    {
        if (callback)
            callback(response.Type == Response::ResponseType::Ok);
    });
}