/*
#include <iostream>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

#include <boost/log/trivial.hpp>
#include <boost/log/common.hpp>
#include <boost/log/formatters.hpp>
#include <boost/log/filters.hpp>

#include <boost/log/utility/init/to_file.hpp>
#include <boost/log/utility/init/to_console.hpp>
#include <boost/log/utility/init/common_attributes.hpp>

#include <boost/log/attributes/timer.hpp>

#include <boost/log/sources/logger.hpp>
#include <boost/log/sinks/debug_output_backend.hpp>



namespace logging = boost::log;
namespace fmt = boost::log::formatters;
namespace flt = boost::log::filters;
namespace sinks = boost::log::sinks;
namespace attrs = boost::log::attributes;
namespace src = boost::log::sources;
namespace keywords = boost::log::keywords;

void init_logging()
{
    boost::shared_ptr< logging::core > core = logging::core::get();

    // Create the sink. The backend requires synchronization in the frontend.
    typedef sinks::synchronous_sink< sinks::debug_output_backend > sink_t;
    boost::shared_ptr< sink_t > sink(new sink_t());

    // Set the special filter to the frontend
    // in order to skip the sink when no debugger is available
    sink->set_filter(sinks::debug_output_backend::debugger_presence_filter());

    core->add_sink(sink);
}
*/

#include <string>
#include <fstream>
#include <iostream>
#include <boost/shared_ptr.hpp>
#include <boost/log/core.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/debug_output_backend.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/filters.hpp>
#include <boost/log/formatters.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/sources/severity_channel_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/utility/empty_deleter.hpp>
#include <boost/log/utility/init/common_attributes.hpp>

#include <boost/log/trivial.hpp>
#include <UnitTest++.h>
#include <XmlTestReporter.h>

namespace logging = boost::log;
namespace src = boost::log::sources;
namespace sinks = boost::log::sinks;
namespace flt = boost::log::filters;
namespace fmt = boost::log::formatters;
namespace keywords = boost::log::keywords;


void init_logging()
{
    boost::shared_ptr< logging::core > core = logging::core::get();
    //logging::add_common_attributes();

    // Create the sink. The backend requires synchronization in the frontend.
    typedef sinks::synchronous_sink< sinks::debug_output_backend > sink_debug_t;
    boost::shared_ptr< sink_debug_t > sink_debug(new sink_debug_t());

    // Set the special filter to the frontend
    // in order to skip the sink when no debugger is available
    sink_debug->set_filter(sinks::debug_output_backend::debugger_presence_filter());

    core->add_sink(sink_debug);

    boost::shared_ptr< sinks::text_file_backend > backend =
        boost::make_shared< sinks::text_file_backend >(
        keywords::file_name = "file_%5N.log",                                          /*< file name pattern >*/
        keywords::rotation_size = 5 * 1024 * 1024,                                     /*< rotate the file upon reaching 5 MiB size... >*/
        keywords::time_based_rotation = sinks::file::rotation_at_time_point(12, 0, 0)  /*< ...or every day, at noon, whichever comes first >*/
        );

    // Wrap it into the frontend and register in the core.
    // The backend requires synchronization in the frontend.
    typedef sinks::synchronous_sink< sinks::text_file_backend > sink_file_t;
    boost::shared_ptr< sink_file_t > sink_file(new sink_file_t(backend));

    sink_file->locked_backend()->set_formatter(fmt::stream
        << fmt::attr("LineID") // First an attribute "LineID" is written to the log
        << " [" << fmt::date_time< boost::posix_time::ptime >("TimeStamp", "%d.%m.%Y %H:%M:%S.%f")
        << "] [" << fmt::time_duration< boost::posix_time::time_duration >("Uptime")
        << "] [" // then this delimiter separates it from the rest of the line
        << fmt::if_(flt::has_attr("Tag"))
        [
            fmt::stream << fmt::attr< std::string >("Tag") // then goes another attribute named "Tag"
            // Note here we explicitly stated that its type
            // should be std::string. We could omit it just
            // like we did it with the "LineNumber", but in this case
            // library would have to detect the actual attribute
            // type in run time which has the following consequences:
            // - On the one hand, the attribute would have been output
            //   even if it has another type (not std::string).
            // - On the other, this detection does not come for free
            //   and will result in performance decrease.
            //
            // In general it's better you to specify explicitly which
            // type should an attribute have wherever it is possible.
            // You may specify an MPL sequence of types if the attribute
            // may have more than one type. And you will have to specify
            // it anyway if the library is not familiar with it (see
            // boost/log/type_dispatch/standard_types.hpp for the list
            // of the supported out-of-box types).
            << "] [" // yet another delimiter
        ]
    << fmt::named_scope("Scope", keywords::iteration = fmt::reverse) << "] "
        << fmt::message()); // here goes the log record text

    core->add_sink(sink_file);


}


int main()
{
    //init_logging();

    //src::logger lg;
    //BOOST_LOG(lg) << "Hello world!\n";

    //return UnitTest::RunAllTests();


    //init_logging();
        
    BOOST_LOG_TRIVIAL(trace) << "A trace severity message";
    BOOST_LOG_TRIVIAL(debug) << "A debug severity message";
    BOOST_LOG_TRIVIAL(info) << "An informational severity message";
    BOOST_LOG_TRIVIAL(warning) << "A warning severity message";
    BOOST_LOG_TRIVIAL(error) << "An error severity message";
    BOOST_LOG_TRIVIAL(fatal) << "A fatal severity message";

    return UnitTest::RunAllTests();

    /*
    // or this to generate xml report
    std::string testFileName = "your_report_file_name.xml";
    std::ofstream f(testFileName.c_str());
    UnitTest::XmlTestReporter reporter(f);
    UnitTest::TestRunner runner(reporter);
    result = runner.RunTestsIf(UnitTest::Test::GetTestList(), NULL, UnitTest::True(), 0);
    return result;
    */
}