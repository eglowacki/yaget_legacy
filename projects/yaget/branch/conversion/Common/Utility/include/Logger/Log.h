/////////////////////////////////////////////////////////////////////////
// Log.h
//
//  Copyright 1/1/2009 Edgar Glowacki.
//
// NOTES:
//      Using torjo log library:
//      http://torjo.com/log2/doc/html/index.html
//
//
// #include "Logger/Log.h"
//
/////////////////////////////////////////////////////////////////////////
//! \file

#ifndef LOGGER_LOG_H
#define LOGGER_LOG_H
#pragma once

#define YAGET_USE_LOGGER

#ifdef YAGET_USE_LOGGER

#include "Base.h"
#pragma warning(push)
    #pragma warning (disable : 4244)  // '' : conversion from 'int' to 'unsigned short', possible loss of data
    #pragma warning (disable : 4512)  // '' : assignment operator could not be generated
    #include <boost/logging/format_fwd.hpp>
    //#include <boost/logging/format.hpp>
#pragma warning(pop)

//-------------------------------------------------------------------------------------
// trying to create logger manualy
namespace bl = boost::logging;


//! create log message format
typedef bl::tag::holder<
                        bl::optimize::cache_string_one_str<>,
                        bl::tag::time,
                        bl::tag::file_line,
                        bl::tag::level,
                        bl::tag::module
                       > log_format_string_t;

//! set this format as the current one
BOOST_LOG_FORMAT_MSG(log_format_string_t)


//! create finder for best fit of filter and logger types
using namespace bl::scenario::usage;
typedef use<
            filter_::change::often<>,
            filter_::level::use_levels,
            logger_::change::often<>,
            logger_::favor::correctness,
            logger_::gather::ostream_like
            > finder_t;

typedef finder_t::logger Logger_t;


//! Since we also want to provide filter based on level and module name we
//! derive our own
struct FilterLevelModule : finder_t::filter
{
    bool is_enabled(bl::level::type level, const char *module_name) const;
    std::vector<std::string> traceModules;
};


BOOST_DECLARE_LOG_FILTER(LoggerFilter, FilterLevelModule)
BOOST_DECLARE_LOG(Logger, Logger_t)

#define BOOST_LOG_USE_LOG_IF_LEVEL_MODULE(l, holder, the_level, the_module_name) BOOST_LOG_USE_LOG(l, read_msg().gather().out(), holder->is_enabled(bl::level::the_level, the_module_name))

#define log_fatal BOOST_LOG_USE_LOG_IF_LEVEL_MODULE(Logger(), LoggerFilter(), fatal, "all").set_tag(BOOST_LOG_TAG_FILELINE).set_tag(BOOST_LOG_TAG_LEVEL(fatal))
#define log_error BOOST_LOG_USE_LOG_IF_LEVEL_MODULE(Logger(), LoggerFilter(), error, "all").set_tag(BOOST_LOG_TAG_FILELINE).set_tag(BOOST_LOG_TAG_LEVEL(error))
#define log_warning BOOST_LOG_USE_LOG_IF_LEVEL_MODULE(Logger(), LoggerFilter(), warning, "all").set_tag(BOOST_LOG_TAG_FILELINE).set_tag(BOOST_LOG_TAG_LEVEL(warning))
#define log_trace(module_name) BOOST_LOG_USE_LOG_IF_LEVEL_MODULE(Logger(), LoggerFilter(), info, module_name).set_tag(BOOST_LOG_TAG_FILELINE).set_tag(BOOST_LOG_TAG_LEVEL(info)).set_tag(BOOST_LOG_TAG(module)(module_name))
#define log_debug BOOST_LOG_USE_LOG_IF_LEVEL_MODULE(Logger(), LoggerFilter(), debug, "all").set_tag(BOOST_LOG_TAG_FILELINE).set_tag(BOOST_LOG_TAG_LEVEL(debug))


namespace eg
{
    namespace logs
    {
        //inline void not_impl(const char *text) {log_error << "Functionality '" << text << "' is not implemented yet!!!.";}
        #define logs_not_impl(text) {log_error << "Functionality '" << (text) << "' is not implemented yet!!!.";}

        //! this can be used to format log stream to show numbers as hex '0xf'
        struct hex_on_placeholder {};
        //! turn of hex formating and go back dec
        struct hex_off_placeholder {};

        //! \code
        //! log_debug << "Flags " << logs::hex_on << number_to_show << logs::hex_off;
        static hex_on_placeholder hex_on;
        static hex_off_placeholder hex_off;

        //! This can be used to simply printing hex numbers
        //! log_debug << "Flags " << logs::hex(number_to_show);
        template <typename T>
        struct hex
        {
            hex(const T& v) : value(v) {}
            T value;
        };

        inline const char *boolean(bool v) { return v ? "TRUE" : "FALSE"; }
        template <typename T>
        inline const char *ptr(T *v) { return v ? "YES" : "NO"; }

        //! {@
        //! This functions are needed for log overloading output
        inline std::ostream& operator<<(std::ostream& out, const hex_on_placeholder&) {return out << std::hex << std::showbase;}
        inline std::ostream& operator<<(std::ostream& out, const hex_off_placeholder&) {return out << std::dec << std::noshowbase;}

        template <typename T>
        inline std::ostream& operator<<(std::ostream& out, const hex<T>& placeholder) {return out << hex_on << placeholder.value << hex_off;}
        //! }@

        const uint32_t kDest_file = 0x0001;
        const uint32_t kDest_debug = 0x0002;
        const uint32_t kDest_cout = 0x0004;

        /*
        enum {
            disable_all = (type)-1,
            enable_all = 0,
            debug = 1000,
            info = 2000,
            warning = 3000,
            error = 4000,
            fatal = 5000
        };
        */

        //! Initialize logs. Any logged messages before hand,
        //! will be added to a log when this function is called.
        //!
        //! \param logName used in file name for log output. If empty, then use '<full_app_path>.log'
        //! \param destFlags where to output log messages
        //! \param logLevel minimum log level to output
        //! \param bActive if true, than log is active, otherwise nothing will show up
        //! \param traceModules which modules names can be seen for log_trace type
        //!                     This can be used to instrument modules in detail, but have
        //!                     disable for production, etc.
        //!
        //! \code
        //! boost::logging::level::enable_all
        //!
        //! enum {
        //!    disable_all = (type)-1,
        //!    enable_all = 0,
        //!    debug = 1000,
        //!    info = 2000,
        //!    warning = 3000,
        //!    error = 4000,
        //!    fatal = 5000
        //! };
        //! \endcode
        void Initialize(const std::string& logName, uint32_t destFlags, int logLevel, bool bActive, const std::vector<std::string>& traceModules);

        //! Memory leaker does not handle very well objects created before main
        //! and destroyed after main. Use this as a global initializer to
        //! create log before MemoryLeaker is initialized
        //!
        struct LoggerInit
        {
            LoggerInit(const std::string& logName, uint32_t destFlags, int logLevel, bool bActive, const std::vector<std::string>& traceModules)
            {
                Initialize(logName, destFlags, logLevel, bActive, traceModules);
            }
        };

    } // namespace logs
} // namespace eg

#endif // YAGET_USE_LOGGER

#endif // LOGGER_LOG_H
