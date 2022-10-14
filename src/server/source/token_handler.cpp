#include "token_handler.h"
#include "options.h"
#include "client_info.h"
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio.hpp>
#include <boost/asio/steady_timer.hpp>
#include <unordered_set>
#include <mutex>
#include <vector>
#include <list>
#include <fstream>
#include <thread>
#include <chrono>
#include <iomanip>

namespace app::server {

struct ClientState
{
    uint64_t sessionId{ 0 };
    uint64_t expected{ 0 };
    uint64_t received{ 0 };
};

struct ClientTokenInfo
{
    ClientConnectionInfo clientInfo;
    ClientState clientState;

    friend std::ostream& operator<<(std::ostream& os, const ClientTokenInfo& state)
    {
        os
            << state.clientInfo.clientId << " (" << state.clientInfo.ip << ':' << state.clientInfo.port << "): "
            << "expected=" << state.clientState.expected << ", received=" << state.clientState.received;
        return os;
    }
};

struct IToken
{
    virtual void Handle(const IClientInfo& client) = 0;
    virtual std::vector<ClientTokenInfo> GetTokenState() const = 0;
    virtual const std::string& GetName() const = 0;
    virtual ~IToken() = default;
};

class Token : public IToken
{
public:
    Token(std::shared_ptr<ILogger> logger, const std::string& token)
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


class TokenHandlerStateLogger
{
public:
    TokenHandlerStateLogger(const Options& options)
        : m_file{ GetLogFilePath(options), std::ios::out | std::ios::trunc }
        , m_interval{ options.interval }
        , m_timer{ m_ioContext }
    {
        ScheduleWork();
        m_loggerThread = std::thread([this]{ m_ioContext.run(); });
    }

    ~TokenHandlerStateLogger()
    {
        m_finished = true;
        m_loggerThread.join();
    }

public:
    void AddTokenHandler(std::shared_ptr<IToken> token)
    {
        m_ioContext.post([this, token = std::move(token)]() mutable
                {
                    std::lock_guard l{m_mutex};
                    m_tokens.push_back(std::move(token));
                });
    }

private:
    void ScheduleWork()
    {
        if (m_finished)
            return;

        m_timer.expires_after(std::chrono::seconds(m_interval));
        m_timer.async_wait([this](const boost::system::error_code& error){ HandleTimer(); });
    }

    void HandleTimer()
    {
        ScheduleWork();
        LogState();
    }

    void LogState()
    {
        auto tokensState{ GetTokensState() };
        m_file << "Tokens count: " << tokensState.size() << std::endl;

        for (const auto& [info, name] : tokensState)
        {
            m_file << "\t[" << name << "]:" << std::endl;

            for (const auto& entry : info)
            {
                m_file << "\t\t" << entry << std::endl;
            }
        }
    }

    std::vector<std::pair<std::vector<ClientTokenInfo>, std::string /*name*/>> GetTokensState() const
    {
        std::vector<std::pair<std::vector<ClientTokenInfo>, std::string /*name*/>> result;

        std::lock_guard l{ m_mutex };
        result.reserve(m_tokens.size());
        for (const auto& token : m_tokens)
        {
            result.push_back(std::make_pair(token->GetTokenState(), token->GetName()));
        }

        return result;
    }

    static std::string GetLogFilePath(const Options& options)
    {
        auto time{ std::time(nullptr) };

        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%F_%R") << "_" << getpid() << ".txt";

        auto timeStr{ ss.str() };
        std::replace(timeStr.begin(), timeStr.end(), ':', '-');
        return options.handlersStatusDir + "/" + timeStr;
    }

private:
    boost::asio::io_context m_ioContext;
    uint32_t m_interval;
    boost::asio::steady_timer m_timer;
    std::ofstream m_file;
    std::thread m_loggerThread;

    std::atomic_bool m_finished{false};
    mutable std::mutex m_mutex;
    std::deque<std::shared_ptr<IToken>> m_tokens;
};


class TokenHandlerManager : public ITokenHandler
{
public:
    TokenHandlerManager(std::shared_ptr<ILogger> logger, const Options& options)
        : m_logger{ std::move(logger) }
        , m_options{ options }
        , m_stateLogger{ m_options }
        , m_work{ boost::asio::make_work_guard(m_ioContext) }
    {
        m_threads.reserve(m_options.threadCount);

        for (int i{ 0 }; i < m_options.threadCount; ++i)
        {
            m_threads.emplace_back([this]{ m_ioContext.run(); });
        }
    }

    ~TokenHandlerManager()
    {
        m_work.reset();

        for (auto& thread : m_threads)
        {
            thread.join();
        }
    }

public:
    void HandleToken(std::shared_ptr<IClientInfo> client, std::string&& token) final
    {
        m_logger->Log(Severity::Info) << "Queue handle task for " << token << std::endl;
        m_ioContext.post([this, client = std::move(client), token = std::move(token)]() mutable
            {
                HandleTokenImpl(std::move(client), std::move(token));
            });
    }

private:
    void HandleTokenImpl(std::shared_ptr<IClientInfo> client, std::string&& token)
    {
        const auto& connInfo{ client->GetClientConnectionInfo() };
        m_logger->Log(Severity::Info)
            << "Handle token [" << token << "] for " << connInfo.clientId << " " << connInfo.ip << ":" << connInfo.port << std::endl;

        bool isNew;
        auto handler{ GetTokenHandler(std::move(token), isNew) };
        handler->Handle(*client);
        if (isNew)
        {
            m_stateLogger.AddTokenHandler(handler);
        }
    }

    std::shared_ptr<IToken> GetTokenHandler(std::string&& token, bool& isNew)
    {
        std::lock_guard l{ m_handlersMutex };

        const auto handlerIt{ m_handlers.find(token) };
        if (handlerIt == m_handlers.cend())
        {
            auto tokenHandler{ std::make_shared<Token>(m_logger, token) };
            m_handlers[std::move(token)] = tokenHandler;
            isNew = true;
            return tokenHandler;
        }
        
        isNew = false;
        return handlerIt->second;
    }

private:
    const std::shared_ptr<ILogger> m_logger;
    const Options m_options;
    TokenHandlerStateLogger m_stateLogger;
    boost::asio::io_context m_ioContext;
    boost::asio::executor_work_guard<boost::asio::io_context::executor_type> m_work;
    std::vector<std::thread> m_threads;
    std::mutex m_handlersMutex;
    std::unordered_map<std::string /*token*/, std::shared_ptr<IToken>> m_handlers;
};


std::shared_ptr<ITokenHandler> CreateTokenHandler(std::shared_ptr<ILogger> logger, const Options& options)
{
    return std::make_shared<TokenHandlerManager>(std::move(logger), options);
}

} // namespace app::server
