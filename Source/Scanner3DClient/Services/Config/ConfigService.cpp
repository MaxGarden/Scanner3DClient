#include "pch.h"
#include "ConfigService.h"

using namespace Scanner3DClient::Services;

bool ConfigService::SendGetConfigRequest(GetConfigResponseCallback&& callback)
{
    return SendRequest(Request{ 'g', {} }, [this, callback = std::move(callback)](auto&& response)
    {
        OnConfigResponse(callback, std::move(response));
    });
}

bool ConfigService::SendApplyConfigRequest(const Config& config, GetConfigResponseCallback&&callback)
{
    const auto beginIterator = reinterpret_cast<const RemoteServices::byte*>(&config);
    const auto endIterator = beginIterator + sizeof(Config);
    auto payload = RemoteServices::ServicePayload{ beginIterator, endIterator };

    return SendRequest(Request{ 'a', std::move(payload) }, [this, callback = std::move(callback)](auto&& response)
    {
        OnConfigResponse(callback, std::move(response));
    });
}

void ConfigService::OnConfigResponse(const GetConfigResponseCallback& callback, Response&& response)
{
    if (!callback)
        return;

    if (response.Type != Response::ResponseType::Ok)
        return callback(std::nullopt);

    auto& payload = response.Payload;
    CLIENT_ASSERT(payload.size() == sizeof(Config));
    if (payload.size() != sizeof(Config))
        callback(std::nullopt);

    const auto cameraConfig = reinterpret_cast<Config*>(payload.data());
    callback(*cameraConfig);
}