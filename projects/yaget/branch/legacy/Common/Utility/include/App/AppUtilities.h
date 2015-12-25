/////////////////////////////////////////////////////////////////////////
// AppUtilities.h
//
//  Copyright 4/11/2009 Edgar Glowacki.
//
// NOTES:
//      Common functions and global utilities
//
//
// #include "App/AppUtilities.h"
//
/////////////////////////////////////////////////////////////////////////
//! \file

#ifndef APP_APP_UTILITIES_H
#define APP_APP_UTILITIES_H
#pragma once

#include "Base.h"
#include <string>
#include <map>
#include <boost/any.hpp>

namespace eg
{
    namespace util
    {

        /*
        inline void sleep(uint32_t miliseconds)
        {
            boost::xtime xt;
            boost::xtime_get(&xt, boost::TIME_UTC);
            xt.nsec += miliseconds * 1000000.0;
            boost::thread::sleep(xt);
        }
        */

        void sleep(uint32_t miliseconds);

        inline void sleep(double seconds)
        {
            uint32_t duration = static_cast<uint32_t>(seconds * 1000.0);
            util::sleep(duration);
        }


        //! Return full path to current module
        std::string getAppPath();
        //! Return name of current module with extension stripped
        std::string getAppName();
        //! Return path and name combine (getExecutablePath + file::sep + getAppName)
        std::string getAppFullPath();

    } // namespace util

    namespace app
    {
        typedef void *handle_t;
    }


} // namespace eg

#endif // APP_APP_UTILITIES_H

