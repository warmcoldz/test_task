#include "token_handler.h"
#include <chrono>
#include <mutex>
#include <thread>

namespace app::server {

class TokenHandler : public ITokenHandler
{
public:
    TokenHandler(std::shared_ptr<ILogger> logger, const std::string& token)
        : m_logger{ std::move(logger) }
        , m_token{ token }
    {
    }

    void Handle(const IClientInfo& client) final
    {
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(500ms);

        std::lock_guard l{ m_mutex };

        auto& state{ m_clientsInfo[client.GetClientConnectionInfo()] };
        const uint64_t clientSessionId{ client.GetSessionId() };
        if (state.sessionId != clientSessionId)
        {
            state.sessionId = clientSessionId;
            state.expected = client.GetExpectedTokenCount();
        }

        ++state.received;
    }

    std::vector<ClientTokenInfo> GetTokenState() const
    {
        std::vector<ClientTokenInfo> tokenState;

        std::lock_guard l{ m_mutex };
        tokenState.reserve(m_clientsInfo.size());

        for (const auto& [info, state] : m_clientsInfo)
        {
            tokenState.push_back(ClientTokenInfo{info, state});
        }

        return tokenState;
    }

    const std::string& GetName() const
    {
        return m_token;
    }

private:
    const std::shared_ptr<ILogger> m_logger;
    const std::string m_token;

    mutable std::mutex m_mutex;
    std::unordered_map<ClientConnectionInfo, ClientState, ClientConnectionInfoHash> m_clientsInfo;
};

std::shared_ptr<ITokenHandler> CreateTokenHandler(std::shared_ptr<ILogger> logger, const std::string& token)
{
    return std::make_shared<TokenHandler>(std::move(logger), token);
}

} // namespace app::server
