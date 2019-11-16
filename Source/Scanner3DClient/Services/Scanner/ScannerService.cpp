#include "pch.h"
#include "ScannerService.h"

using namespace Scanner3DClient::Services;

bool ScannerService::SendCaptureBinarizedImageRequest(CaptureBinarizedImageResponseCallback&& callback)
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

bool ScannerService::SendCaptureAveragedPointsRequest(CaptureAveragedPointsResponseCallback&& callback)
{
    return SendRequest(Request{ 'a', {} }, [callback = std::move(callback)](auto&& response)
    {
        if (!callback)
            return;

        if (response.Type != Response::ResponseType::Ok)
            return callback({});

        auto& payload = response.Payload;
        CLIENT_ASSERT(payload.size() % sizeof(Point) == 0);
        if (payload.size() % sizeof(Point) != 0)
            return callback({});

        const auto beginIterator = reinterpret_cast<const Point*>(payload.data());
        const auto endIterator = beginIterator + payload.size() / sizeof(Point);

        callback(std::vector<Point>{ beginIterator, endIterator });
    });
}