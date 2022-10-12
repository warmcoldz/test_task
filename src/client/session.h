#pragma once

#include "options.h"
#include "token_data_base.h"
#include <core/range.h>
#include <protocol/record_builder.h>
#include <boost/asio.hpp>
#include <boost/asio/spawn.hpp>
#include <boost/range/algorithm/equal.hpp>
#include <string>

namespace app::client {

class Session
{
public:
    Session(
        const Options& options,
        boost::asio::ip::tcp::socket& socket,
        boost::asio::yield_context& yield);

public:
    void SendGreetings();
    void ReceiveReadyRecord();
    void SendTokens();

private:
    void ValidateReadyRecord(core::ConstBlobRange record);
    void SendToken(const std::string& token);

private:
    const Options& m_options;
    boost::asio::ip::tcp::socket& m_socket;
    boost::asio::yield_context& m_yield;
    TokenDataBase m_tokenDataBase;
    uint32_t m_counter{ 0 };
    protocol::RecordBuilder m_recordBuilder;
};

} // namespace app::client
