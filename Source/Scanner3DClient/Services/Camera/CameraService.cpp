#include "pch.h"
#include "CameraService.h"

#include <iostream>

using namespace Scanner3DClient::Services;

bool CameraService::SendGetConfigRequest(const GetConfigResponseCallback& callback)
{
    CLIENT_ASSERT(callback);
    if (!callback)
        return false;

    return SendRequest(Request{ 'g', {} }, [this, &callback](auto&& response)
    {
        OnConfigResponse(callback, std::move(response));
    });
}

bool CameraService::SendApplyConfigResult(const CameraConfig& config, const GetConfigResponseCallback& callback)
{
    CLIENT_ASSERT(callback);
    if (!callback)
        return false;

    const auto beginIterator = reinterpret_cast<const RemoteServices::byte*>(&config);
    const auto endIterator = beginIterator + sizeof(CameraConfig);
    auto payload = RemoteServices::ServicePayload{ beginIterator, endIterator };

    return SendRequest(Request{ 'a', std::move(payload) }, [this, &callback](auto&& response)
    {
        OnConfigResponse(callback, std::move(response));
    });
}

void CameraService::OnConfigResponse(const GetConfigResponseCallback& callback, Response&& response)
{
    if (response.Type != Response::ResponseType::Ok)
        return callback(std::nullopt);

    auto& payload = response.Payload;
    CLIENT_ASSERT(payload.size() == sizeof(CameraConfig));
    if (payload.size() != sizeof(CameraConfig))
        callback(std::nullopt);

    const auto cameraConfig = reinterpret_cast<CameraConfig*>(payload.data());
    callback(*cameraConfig);
}