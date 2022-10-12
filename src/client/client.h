#pragma once

#include "options.h"
#include <core/range.h>
#include <boost/asio.hpp>

namespace app::client {

class Client
{
public:
    explicit Client(const Options& options);

public:
    void Run();

private:
    const Options m_options;
    boost::asio::io_context m_ioContext;
};

} // namespace app::client
