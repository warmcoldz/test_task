#include "options.h"
#include "listen_server.h"
#include <core/program_options.h>
#include <iostream>
#include <string>

using namespace std::string_literals;

namespace app::server {

struct OptionsTraits
{
    using OptionsType = Options;

    static boost::program_options::options_description CreateDescription()
    {
        constexpr uint16_t DefaultPort{ 33337 };
        constexpr uint32_t DefaultThreadCount{ 4 };
        const auto IpAddressAny{ "0.0.0.0"s };

        boost::program_options::options_description desc("Allowed options");
        desc.add_options()
            ("help", "produce help message")
            ("threads,t", boost::program_options::value<decltype(Options::threadCount)>()->default_value(DefaultThreadCount), "thread count.")
            ("ip", boost::program_options::value<decltype(Options::ipAddress)>()->default_value(IpAddressAny), "ip address.")
            ("port,p", boost::program_options::value<decltype(Options::port)>()->default_value(DefaultPort), "listen on a port.")
        ;

        return desc;
    }

    static Options GetOptions(const boost::program_options::variables_map& vm)
    {
        Options options;
        options.threadCount = vm["threads"s].as<decltype(options.threadCount)>();
        options.ipAddress = vm["ip"s].as<decltype(options.ipAddress)>();
        options.port = vm["port"s].as<decltype(options.port)>();
        return options;
    }
};


void RunServer(const Options& options)
{
    ListenServer s{ options };
    s.Run();
}

int main(int argc, char* argv[])
{
    core::ProgramOptionsParser<OptionsTraits> programOptionsParser{ argc, argv };

    if (programOptionsParser.HelpRequested())
    {
        programOptionsParser.PrintHelp();
    }
    else
    {
        RunServer(programOptionsParser.GetOptions());
    }

    return 0;
}

} // namespace app::server


int main(int argc, char* argv[])
{
    try
    {
        return app::server::main(argc, argv);
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
