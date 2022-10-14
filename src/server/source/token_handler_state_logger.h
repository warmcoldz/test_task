#pragma once

#include "options.h"
#include "token_handler.h"
#include <boost/asio.hpp>
#include <fstream>
#include <boost/asio/steady_timer.hpp>
#include <thread>
#include <atomic>
#include <deque>

namespace app::server {

class TokenHandlerStateLogger
{
public:
    TokenHandlerStateLogger(const Options& options);
    ~TokenHandlerStateLogger();

public:
    void AddTokenHandler(std::shared_ptr<ITokenHandler> token);

private:
    void ScheduleWork();
    void HandleTimer();
    void LogState();
    std::vector<std::pair<std::vector<ClientTokenInfo>, std::string /*name*/>> GetTokensState() const;
    static std::string GetLogFilePath(const Options& options);

private:
    boost::asio::io_context m_ioContext;
    uint32_t m_interval;
    boost::asio::steady_timer m_timer;
    std::ofstream m_file;
    std::thread m_loggerThread;

    std::atomic_bool m_finished{false};
    mutable std::mutex m_mutex;
    std::deque<std::shared_ptr<ITokenHandler>> m_tokenHandlers;
};

} // namespace app::server
