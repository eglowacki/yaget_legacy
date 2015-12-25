//////////////////////////////////////////////////////////////////////
// PluginLoader.h
//
//  Copyright 12/27/2006 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//      Managed C++ to facilitate loading of C# dll's and forms
//
//
//  #include "PluginLoader.h"
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef MANAGED_PLUGIN_LOADER_H
#define MANAGED_PLUGIN_LOADER_H
#pragma once

#include <string>
#include <vector>


namespace yaget
{
    namespace managed
    {
        //! Load all .NET plugin, created IPlugin instances, Initialize and send kAllToolPluginInit message
        //! \param pluginNameList collection of plugin names (.DLL = .YEP)
        //! \param packageName associate user defined name with this collection of plugins
        //!                    It is treated as a group after creation
        //! \return true at least one lugin loaded, otherwise false if no plugins or error on load
        bool LoadPlugins(const std::vector<std::string>& pluginNameList, const std::string& packageName);

        //! Unload all plugins associated with this package name
        void UnloadPlugins(const std::string& packageName);


    } // namespace managed
} // namespace yaget

#endif // MANAGED_PLUGIN_LOADER_H

