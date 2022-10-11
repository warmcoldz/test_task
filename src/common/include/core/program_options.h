#pragma once

#include <boost/program_options.hpp>
#include <iostream>

namespace app::core { 

template <typename OptionTraits>
class ProgramOptionsParser
{
public:
    ProgramOptionsParser(int argc, char* argv[])
        : m_desc{ OptionTraits::CreateDescription() }
    {
        boost::program_options::store(
            boost::program_options::command_line_parser(argc, argv).options(m_desc).run(), m_vm);

        boost::program_options::notify(m_vm);
    }

public:
    bool HelpRequested() const
    {
        return m_vm.count("help");
    }

    void PrintHelp()
    {
        std::clog << m_desc;
    }

    typename OptionTraits::OptionsType GetOptions() const
    {
        return OptionTraits::GetOptions(m_vm);
    }

private:
    const boost::program_options::options_description m_desc;
    boost::program_options::variables_map m_vm;
};

} // namespace app::core
