#include "client.h"
#include "options.h"
#include <core/program_options.h>
#include <boost/system/system_error.hpp>
#include <iostream>
#include <string>

using namespace std::string_literals;

namespace app::client {

struct OptionsTraits
{
    using OptionsType = Options;

    static boost::program_options::options_description CreateDescription()
    {
        constexpr uint16_t DefaultPort{ 33337 };

        boost::program_options::options_description desc("Allowed options");
        desc.add_options()
            ("help", "produce help message")
            ("id", boost::program_options::value<decltype(Options::clientId)>()->required(), "client id.")
            ("path", boost::program_options::value<decltype(Options::path)>()->required(), "path.")
            ("token_count,n", boost::program_options::value<decltype(Options::tokenCount)>()->required(), "token count.")
            ("ip", boost::program_options::value<decltype(Options::ipAddress)>()->required(), "ip address.")
            ("port,p", boost::program_options::value<decltype(Options::port)>()->default_value(DefaultPort), "listen on a port.")
        ;

        return desc;
    }

    static Options GetOptions(const boost::program_options::variables_map& vm)
    {
        Options options;
        options.clientId = vm["id"s].as<decltype(options.clientId)>();
        options.path = vm["path"s].as<decltype(options.path)>();
        options.tokenCount = vm["token_count"s].as<decltype(options.tokenCount)>();
        options.ipAddress = vm["ip"s].as<decltype(options.ipAddress)>();
        options.port = vm["port"s].as<decltype(options.port)>();
        return options;
    }
};


void RunClient(const Options& options)
{
    Client client{ options };
    client.Run();
}

int main(int argc, char* argv[])
{
    core::ProgramOptionsParser<OptionsTraits> programOptionsParser{ argc, argv };
    RunClient(programOptionsParser.GetOptions());
    return 0;
}

} // namespace app::client


int main(int argc, char* argv[])
{
    try
    {
        return app::client::main(argc, argv);
    }
    catch (const boost::system::system_error& e)
    {
        std::cerr << "Boost system exception: " << e.what() << ", code: " << e.code() << std::endl;
        return 1;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 2;
    }
}
