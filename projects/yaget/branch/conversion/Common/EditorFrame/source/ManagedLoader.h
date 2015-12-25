//////////////////////////////////////////////////////////////////////
// ManagedLoader.h
//
//  Copyright 12/26/2006 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//      Wrapper for loading and initialization of managed plugins
//      implmented as a plugin object
//
//
//  #include "ManagedLoader.h"
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef MANAGED_LOADER_H
#define MANAGED_LOADER_H
#pragma once

#include "EditorBase.h"
#include "IMetPlugin.h"
#include "MessageInterface.h"

#pragma warning (disable : 4793) // 'vararg' : causes native code generation for function 'void boost::detail::sp_enable_shared_from_this(const boost::detail::shared_count &,...)'
#include <boost/signal.hpp>
#pragma warning (default : 4793)

namespace eg
{
    /*!
    This is simple glue between ManagedLoader and MC++ code which
    in turn will call and interact with .NET (in our case it will
    mostly be C#). The ManagedLoader just routes calls to IManagedInterface.

    \note Main reason for this glue to be here is that trying to convert
    this class to MC++ had issues with wxWidgets and I did not invetigate much further.
    */
    class IManagedInterface
    {
    public:
        virtual ~IManagedInterface() = 0 {}
        virtual bool Init() = 0;
    };

    /*!
    This intercas with IManagedInterface to set and get any data
    */
    class ManagedLoader : public IMetPlugin, public boost::signals::trackable
    {
    public:
        ManagedLoader(const char *pName, IManagedInterface *pMI);
        virtual ~ManagedLoader();

        // from IMetPlugin
        virtual wxWindow *Init(wxWindow *pParentWindow);
        virtual const char *GetName() const;

    private:
        IManagedInterface *mpInterface;
        std::string mName;
    };




    namespace managed
    {
        void LogTrace(const std::string& text);
        void LogMessage(const std::string& text);
        void LogWarning(const std::string& text);
        void LogError(const std::string& text);
    } // namespace managed


} // namespace eg

#endif // MANAGED_LOADER_H

