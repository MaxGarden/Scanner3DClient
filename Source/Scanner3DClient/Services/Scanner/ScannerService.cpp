#include "pch.h"
#include "ScannerService.h"

using namespace Scanner3DClient::Services;

bool ScannerService::SendCaptureBinarizedImageRequest(CaptureBinarizedImageResponseCallback&& callback)
{
    return SendRequest(Request{ 'b', {} }, [this, callback = std::move(callback)](auto&& response)
    {
        if (!callback)
            return;

        if (response.Type != Response::ResponseType::Ok)
            callback({});
        else
            callback(std::move(response.Payload));
    });
}