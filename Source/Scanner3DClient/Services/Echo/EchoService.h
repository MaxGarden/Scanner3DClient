#pragma once

#include "RemoteServices/Services/SingleConnectionServiceBase.h"

namespace Scanner3DClient::Services
{
    class EchoService final : public RemoteServices::SingleConnectionServiceBase
    {
    public:
        EchoService() = default;
        virtual ~EchoService() override final = default;

        void SendMessage(const std::string& message);

        virtual void OnReceived(const RemoteServices::ServicePayload& payload) override final;

    private:
        std::vector<std::string> m_sentMessages;
    };
}