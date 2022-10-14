#include "token_handler_state_logger.h"
#include <iomanip>

namespace app::server {

TokenHandlerStateLogger::TokenHandlerStateLogger(const Options& options)
    : m_file{ GetLogFilePath(options), std::ios::out | std::ios::trunc }
    , m_interval{ options.interval }
    , m_timer{ m_ioContext }
{
    ScheduleWork();
    m_loggerThread = std::thread([this]{ m_ioContext.run(); });
}

TokenHandlerStateLogger::~TokenHandlerStateLogger()
{
    m_finished = true;
    m_loggerThread.join();
}

void TokenHandlerStateLogger::AddTokenHandler(std::shared_ptr<ITokenHandler> tokenHandler)
{
    m_ioContext.post([this, tokenHandler = std::move(tokenHandler)]() mutable
            {
                std::lock_guard l{m_mutex};
                m_tokenHandlers.push_back(std::move(tokenHandler));
            });
}

void TokenHandlerStateLogger::ScheduleWork()
{
    if (m_finished)
        return;

    m_timer.expires_after(std::chrono::seconds(m_interval));
    m_timer.async_wait([this](const boost::system::error_code& error){ HandleTimer(); });
}

void TokenHandlerStateLogger::HandleTimer()
{
    ScheduleWork();
    LogState();
}

void TokenHandlerStateLogger::LogState()
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

std::vector<std::pair<std::vector<ClientTokenInfo>, std::string /*name*/>> TokenHandlerStateLogger::GetTokensState() const
{
    std::vector<std::pair<std::vector<ClientTokenInfo>, std::string /*name*/>> result;

    std::lock_guard l{ m_mutex };
    result.reserve(m_tokenHandlers.size());
    for (const auto& tokenHandler : m_tokenHandlers)
    {
        result.push_back(std::make_pair(tokenHandler->GetTokenState(), tokenHandler->GetName()));
    }

    return result;
}

std::string TokenHandlerStateLogger::GetLogFilePath(const Options& options)
{
    auto time{ std::time(nullptr) };

    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%F_%R") << "_" << getpid() << ".txt";

    auto timeStr{ ss.str() };
    std::replace(timeStr.begin(), timeStr.end(), ':', '-');
    return options.handlersStatusDir + "/" + timeStr;
}

} // namespace app::server
