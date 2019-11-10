#include "pch.h"
#include "CameraService.h"

#include <iostream>

using namespace Scanner3DClient::Services;

bool CameraService::SendGetConfigRequest(GetConfigResponseCallback&& callback)
{
    return SendRequest(Request{ 'g', {} }, [this, callback = std::move(callback)](auto&& response)
    {
        OnConfigResponse(callback, std::move(response));
    });
}

bool CameraService::SendApplyConfigRequest(const CameraConfig& config, GetConfigResponseCallback&&callback)
{
    const auto beginIterator = reinterpret_cast<const RemoteServices::byte*>(&config);
    const auto endIterator = beginIterator + sizeof(CameraConfig);
    auto payload = RemoteServices::ServicePayload{ beginIterator, endIterator };

    return SendRequest(Request{ 'a', std::move(payload) }, [this, callback = std::move(callback)](auto&& response)
    {
        OnConfigResponse(callback, std::move(response));
    });
}

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

void CameraService::OnConfigResponse(const GetConfigResponseCallback& callback, Response&& response)
{
    if (!callback)
        return;

    if (response.Type != Response::ResponseType::Ok)
        return callback(std::nullopt);

    auto& payload = response.Payload;
    CLIENT_ASSERT(payload.size() == sizeof(CameraConfig));
    if (payload.size() != sizeof(CameraConfig))
        callback(std::nullopt);

    const auto cameraConfig = reinterpret_cast<CameraConfig*>(payload.data());
    callback(*cameraConfig);
}