#include "token_handler_manager.h"
#include "token_handler_state_logger.h"
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

class TokenHandlerManager : public ITokenHandlerManager
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

    std::shared_ptr<ITokenHandler> GetTokenHandler(std::string&& token, bool& isNew)
    {
        std::lock_guard l{ m_handlersMutex };

        const auto handlerIt{ m_handlers.find(token) };
        if (handlerIt == m_handlers.cend())
        {
            auto tokenHandler{ CreateTokenHandler(m_logger, token) };
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
    std::unordered_map<std::string /*token*/, std::shared_ptr<ITokenHandler>> m_handlers;
};

std::shared_ptr<ITokenHandlerManager> CreateTokenHandlerManager(std::shared_ptr<ILogger> logger, const Options& options)
{
    return std::make_shared<TokenHandlerManager>(std::move(logger), options);
}

} // namespace app::server
