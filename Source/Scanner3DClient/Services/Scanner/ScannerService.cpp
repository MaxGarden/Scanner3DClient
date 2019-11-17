#include "pch.h"
#include "ScannerService.h"

using namespace Scanner3DClient::Services;

RemoteServices::IResponseHandleSharedPtr ScannerService::SendCaptureBinarizedImageRequest(CaptureBinarizedImageResponseCallback&& callback)
{
    return SendRequest(Request{ 'b', {} }, [callback = std::move(callback)](auto&& response)
    {
        if (!callback)
            return;

        if (response.Type != Response::ResponseType::Ok)
            callback({});
        else
            callback(std::move(response.Payload));
    });
}

template<typename CallbackType, typename DataType>
static void OnResponse(const CallbackType& callback, RemoteServices::RequestResponseServiceBase::Response&& response)
{
    if (!callback)
        return;

    if (response.Type != RemoteServices::RequestResponseServiceBase::Response::ResponseType::Ok)
        return callback({});

    auto& payload = response.Payload;
    CLIENT_ASSERT(payload.size() % sizeof(DataType) == 0);
    if (payload.size() % sizeof(DataType) != 0)
        return callback({});

    const auto beginIterator = reinterpret_cast<const DataType*>(payload.data());
    const auto endIterator = beginIterator + payload.size() / sizeof(DataType);

    callback(std::vector<DataType>{ beginIterator, endIterator });
}

RemoteServices::IResponseHandleSharedPtr ScannerService::SendCaptureAveragedPointsRequest(CaptureAveragedPointsResponseCallback&& callback)
{
    return SendRequest(Request{ 'a', {} }, [callback = std::move(callback)](auto&& response)
    {
        OnResponse<CaptureAveragedPointsResponseCallback, Point>(callback, std::move(response));
    });
}

RemoteServices::IResponseHandleSharedPtr ScannerService::SendCalculate3DPointsRequest(float trayAngle, Calculate3DPointsResponseCallback&& callback)
{
    const auto beginIterator = reinterpret_cast<const byte*>(&trayAngle);
    const auto endIterator = beginIterator + sizeof(float);

    return SendRequest(Request{ 'r', { beginIterator, endIterator } }, [callback = std::move(callback)](auto&& response)
    {
        OnResponse<Calculate3DPointsResponseCallback, Point3D>(callback, std::move(response));
    });
}