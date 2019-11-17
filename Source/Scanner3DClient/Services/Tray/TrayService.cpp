#include "pch.h"
#include "TrayService.h"

using namespace Scanner3DClient::Services;

RemoteServices::IResponseHandleSharedPtr TrayService::SendStepForwardRequest(StepResponseCallback&& callback)
{
    return SendStepRequest('f', std::move(callback));
}

RemoteServices::IResponseHandleSharedPtr TrayService::SendStepBackwardRequest(StepResponseCallback&& callback)
{
    return SendStepRequest('b', std::move(callback));
}

RemoteServices::IResponseHandleSharedPtr TrayService::SendStepRequest(Request::RequestType requestType, StepResponseCallback&& callback)
{
    return SendRequest(Request{ requestType, {} }, [callback = std::move(callback)](auto&& response)
    {
        if (callback)
            callback(response.Type == Response::ResponseType::Ok);
    });
}