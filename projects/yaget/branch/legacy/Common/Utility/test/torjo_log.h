/////////////////////////////////////////////////////////////////////////
// torjo_log.h
//
//  Copyright 12/30/2008 Edgar Glowacki.
//
// NOTES:
//      Testing torjo log implementation
//
//
// #include "torjo_log.h"
//
/////////////////////////////////////////////////////////////////////////
//! \file

#ifndef TORJO_LOG_H
#define TORJO_LOG_H
#pragma once

#include "Base.h"
#pragma warning(push)
    #pragma warning (disable : 4244)  // '' : conversion from 'int' to 'unsigned short', possible loss of data
    #pragma warning (disable : 4512)  // '' : assignment operator could not be generated
    #include <boost/logging/format_fwd.hpp>
    #include <boost/logging/format.hpp>
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
};


BOOST_DECLARE_LOG_FILTER(LoggerFilter, FilterLevelModule)
BOOST_DECLARE_LOG(Logger, Logger_t)

#define BOOST_LOG_USE_LOG_IF_LEVEL_MODULE(l, holder, the_level, the_module_name) BOOST_LOG_USE_LOG(l, read_msg().gather().out(), holder->is_enabled(bl::level:: the_level, the_module_name))

#define log_fatal BOOST_LOG_USE_LOG_IF_LEVEL_MODULE(Logger(), LoggerFilter(), fatal, "all").set_tag(BOOST_LOG_TAG_FILELINE).set_tag(BOOST_LOG_TAG_LEVEL(fatal))
#define log_error BOOST_LOG_USE_LOG_IF_LEVEL_MODULE(Logger(), LoggerFilter(), error, "all").set_tag(BOOST_LOG_TAG_FILELINE).set_tag(BOOST_LOG_TAG_LEVEL(error))
#define log_warning BOOST_LOG_USE_LOG_IF_LEVEL_MODULE(Logger(), LoggerFilter(), warning, "all").set_tag(BOOST_LOG_TAG_FILELINE).set_tag(BOOST_LOG_TAG_LEVEL(warning))
#define log_trace(module_name) BOOST_LOG_USE_LOG_IF_LEVEL_MODULE(Logger(), LoggerFilter(), info, module_name).set_tag(BOOST_LOG_TAG_FILELINE).set_tag(BOOST_LOG_TAG_LEVEL(info)).set_tag(BOOST_LOG_TAG(module)(module_name))
#define log_debug BOOST_LOG_USE_LOG_IF_LEVEL_MODULE(Logger(), LoggerFilter(), debug, "all").set_tag(BOOST_LOG_TAG_FILELINE).set_tag(BOOST_LOG_TAG_LEVEL(debug))


namespace eg
{
    namespace config
    {

        //! Initialize logs. Any logged messages before hand,
        //! will be added to log when this function is called.
        //!
        //! \param logName if write to file, what name to use.
        //!                In most cases this will be an aplication name.
        void InitLogs(const std::string& logName);

    } // namespace config
} // namespace eg


#endif // TORJO_LOG_H
