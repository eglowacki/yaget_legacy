/////////////////////////////////////////////////////////////////////////
// IEditorPlugin.h
//
//  Copyright 4/26/2009 Edgar Glowacki.
//
// NOTES:
//
//
// #include "IEditorPlugin.h"
//
/////////////////////////////////////////////////////////////////////////
//! \file

#ifndef NTERFACE_IEDITOR_PLUGIN_H
#define NTERFACE_IEDITOR_PLUGIN_H
#pragma once

#include "Plugin/IPluginObject.h"
#include <string>
#include <vector>

namespace eg
{
    class IEditorPlugin : public IPluginObject
    {
    public:
        //! Load applet
        virtual bool Load(const std::string& name) = 0;
        virtual bool Unload(const std::string& name) = 0;
        virtual std::vector<std::string> List(const std::string& name) const = 0;
    };



} // namespace eg


#define tr_editor "editor"

#endif // NTERFACE_IEDITOR_PLUGIN_H

