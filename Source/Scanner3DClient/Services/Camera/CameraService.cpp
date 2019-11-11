#include "pch.h"
#include "CameraService.h"

using namespace Scanner3DClient::Services;

bool CameraService::SendCaptureImageRequest(CaptureImageResponseCallback&& callback)
{
    return SendRequest(Request{ 'c', {} }, [this, callback = std::move(callback)](auto&& response)
    {
        if (!callback)
            return;

        if (response.Type != Response::ResponseType::Ok)
            callback({});
        else
            callback(std::move(response.Payload));
    });
}