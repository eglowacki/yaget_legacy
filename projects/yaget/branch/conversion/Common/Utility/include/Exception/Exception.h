/////////////////////////////////////////////////////////////////////////
// Exception.h
//
//  Copyright 3/29/2009 Edgar Glowacki.
//
// NOTES:
//      Exception handling classes used in Yaget engine
//
//
// #include "Exception/Exception.h"
//
/////////////////////////////////////////////////////////////////////////
//! \file

#ifndef EXCEPTION_EXCEPTION_H
#define EXCEPTION_EXCEPTION_H
#pragma once

#include "Base.h"
#include <exception>

namespace eg {
namespace ex
{
    //#include <boost/format.hpp>
    //using namespace boost;
    //throw ex::standard(str(format("shaderObject is not valid for '%1%'.") % name).c_str());



    //! Base exception class which all yaget engine other
    //! exception are derived.
    class standard : public std::exception
    {
    public:
        standard(const char *msg) : std::exception(msg)
        {}
    };

    //! Any serialization error which might leave object in
    //! unfinished state
    class serialize : public standard
    {
    public:
        serialize(const char *msg) : standard(msg)
        {}
    };

    //! Any runtime script error can generate this exception,
    //! like compiling and execution
    class script_runtime : public standard
    {
    public:
        script_runtime(const char *msg) : standard(msg)
        {}
    };

    //! Any generic error in managing resources/assets
    //! like compiling and execution
    class resource : public standard
    {
    public:
        resource(const char *msg) : standard(msg)
        {}
    };

    //! Any plugin error in, mostly during loading
    class plugin : public standard
    {
    public:
        plugin(const char *msg) : standard(msg)
        {}
    };


    //! During development we can throw this error for missing functionality
    class program : public standard
    {
    public:
        program(const char *msg) : standard(msg)
        {}
    };

    //! Any major object can throw this exception in ctor
    class bad_init : public standard
    {
    public:
        bad_init(const char *msg) : standard(msg)
        {}
    };

    //! Any method/function can throw this exception in ctor for invalid parameters
    class bad_param : public standard
    {
    public:
        bad_param(const char *msg) : standard(msg)
        {}
    };

} } // namespace ex // namespace eg



#endif // EXCEPTION_EXCEPTION_H

