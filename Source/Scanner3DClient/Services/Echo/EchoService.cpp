#include "pch.h"
#include "EchoService.h"
#include <iostream>

using namespace Scanner3DClient::Services;

void EchoService::SendMessage(const std::string& message)
{
    m_sentMessages.emplace_back(message);
    if(Send(RemoteServices::ServicePayload{ message.cbegin(), message.cend() }))
        std::cout << "Send echo:" << message << std::endl;
}

void EchoService::OnReceived(const RemoteServices::ServicePayload& payload)
{
    std::string message{ payload.cbegin(), payload.cend() };

    const auto iterator = std::find(m_sentMessages.cbegin(), m_sentMessages.cend(), message);
    if (iterator == m_sentMessages.cend())
        Send(RemoteServices::ServicePayload{ payload });
    else
    {
        std::cout << "Received echo: " << message << std::endl;
        m_sentMessages.erase(iterator);
    }
}