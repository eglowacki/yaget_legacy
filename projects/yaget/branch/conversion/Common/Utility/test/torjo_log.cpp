#include "torjo_log.h"
#pragma warning(push)
    #pragma warning (disable : 4244)  // '' : conversion from 'int' to 'unsigned short', possible loss of data
    #pragma warning (disable : 4512)  // '' : assignment operator could not be generated
    #include <boost/logging/format/formatter/tags.hpp>
    #include <boost/logging/format/formatter/high_precision_time.hpp>
    #include <boost/logging/format/destination/file.hpp>
#pragma warning(pop)

//! own filter implementation to take
bool FilterLevelModule::is_enabled(bl::level::type level, const char *module_name) const
{
    if (finder_t::filter::is_enabled(level))
    {
        if (module_name)
        {
            // let's compare pModuleName with allowed ones
            if (std::string(module_name) == "util")
            {
                return true;
            }
            else if (std::string(module_name) == "render")
            {
                return true;
            }
            else if (std::string(module_name) == "all")
            {
                return true;
            }
        }
    }

    return false;
}


BOOST_DEFINE_LOG_FILTER(LoggerFilter, FilterLevelModule)
BOOST_DEFINE_LOG(Logger, Logger_t)

//using namespace boost::logging;

namespace
{
    const uint32_t kDest_file   = 0x0001;
    const uint32_t kDest_debug  = 0x0002;
    const uint32_t kDest_cout   = 0x0004;
    const uint32_t kDest_Buffer = 0x0008;

} // namespace

namespace eg {
namespace config {

void InitLogs(const std::string& logName)
{
    // options which are exposed to user by means of configuration file
    // which destinations for log. This is only set once at start,
    // but we could allow changes at runtime (but why?).
    // level, which can be change at run time
    // module names, which also can be change at run time
    // for now, we leave output of log in hard coded format
    uint32_t dest = kDest_file|kDest_debug|kDest_cout|kDest_Buffer;
    bool bLogEnabled = true;

    std::string logFilePath = logName.empty() ? "torjo_log.txt" : logName;
    Logger()->writer().add_formatter(bl::formatter::tag::module(), "(%) ");
    Logger()->writer().add_formatter(bl::formatter::tag::level());
    Logger()->writer().add_formatter(bl::formatter::high_precision_time("$hh:$mm.$ss.$mili "));
    Logger()->writer().add_formatter(bl::formatter::tag::file_line());
    Logger()->writer().add_formatter(bl::formatter::append_newline());
    Logger()->writer().add_destination(bl::destination::dbg_window());
    Logger()->writer().add_destination(bl::destination::cout());
    Logger()->writer().add_destination(bl::destination::file(logFilePath, bl::destination::file_settings().initial_overwrite(true).do_append(false)));

    // cleat any default destinations
    Logger()->writer().router().set_route().clear();

    if (dest & kDest_file)
    {
        Logger()->writer().router().append_route().clear()
            // format for file output:
            // 'hh:mm:ss:mil [level] (module_name) message itself\n'
            .fmt(bl::formatter::spacer(bl::formatter::tag::module(), "(%) "))
            .fmt(bl::formatter::tag::level())
            .fmt(bl::formatter::high_precision_time("$hh:$mm.$ss.$mili "))
            .fmt(bl::formatter::append_newline())
            .dest(bl::destination::file(logFilePath, bl::destination::file_settings().initial_overwrite(true).do_append(false)));
    }

    if (dest & kDest_cout)
    {
        Logger()->writer().router().append_route().clear()
            // format for cout:
            // 'hh:mm:ss:mil [level] (module_name) message itself\n'
            .fmt(bl::formatter::spacer(bl::formatter::tag::module(), "(%) "))
            .fmt(bl::formatter::tag::level())
            .fmt(bl::formatter::high_precision_time("$hh:$mm.$ss.$mili "))
            .fmt(bl::formatter::append_newline())
            .dest(bl::destination::cout());
    }

    if (dest & kDest_debug)
    {
        Logger()->writer().router().append_route().clear().clear()
            // output to debug window, where
            // we want to show full file path and line number
            // first, so when user double clicks in Visual Studio,
            // it will take you to that source line
            // format:
            // 'full_file_path(line_number) : hh:mm:ss:mil [level] (module_name) message itself\n'
            .fmt(bl::formatter::spacer(bl::formatter::tag::module(), "(%) "))
            .fmt(bl::formatter::tag::level())
            .fmt(bl::formatter::high_precision_time("$hh:$mm.$ss.$mili "))
            .fmt(bl::formatter::tag::file_line())
            .fmt(bl::formatter::append_newline())
            .dest(bl::destination::dbg_window());
    }

    if (dest & kDest_Buffer)
    {
        // this is clearly a memory leak here
        // but for testing I'm keeping here
        std::stringstream *memStream = new std::stringstream;
        Logger()->writer().router().append_route().clear().clear()
            // output to debug window, where
            // we want to show full file path and line number
            // first, so when user double clicks in Visual Studio,
            // it will take you to that source line
            // format:
            // 'full_file_path(line_number) : hh:mm:ss:mil [level] (module_name) message itself\n'
            .fmt(bl::formatter::spacer(bl::formatter::tag::module(), "(%) "))
            .fmt(bl::formatter::tag::level())
            .fmt(bl::formatter::high_precision_time("$hh:$mm.$ss.$mili "))
            .fmt(bl::formatter::tag::file_line())
            .fmt(bl::formatter::append_newline())
            .dest(bl::destination::stream(memStream));
    }
    

    Logger()->mark_as_initialized();
    if (!bLogEnabled)
    {
        LoggerFilter()->set_enabled(false);
    }

}


} // namespace config
} // namespace eg
