/////////////////////////////////////////////////////////////////////////
// EditorPlugin.h
//
//  Copyright 4/26/2009 Edgar Glowacki.
//
// NOTES:
//      Concrete implementation of Renderer
//
//
// #include "EditorPlugin.h"
//
/////////////////////////////////////////////////////////////////////////
//! \file

#ifndef EDITOR_PLUGIN_H
#define EDITOR_PLUGIN_H
#pragma once

#include "IEditorPlugin.h"
#include "Asset/IAsset.h"


namespace eg
{

    class EditorPlugin : public IEditorPlugin
    {
    public:
        EditorPlugin(IAsset::Token script);
        virtual ~EditorPlugin();

        //! from IEditorPlugin
        virtual bool Load(const std::string& name);
        virtual bool Unload(const std::string& name);
        virtual std::vector<std::string> List(const std::string& name) const;

        // from IPluginObject
        virtual const char *GetName() const {return "Editor";}
        virtual const Version GetVersion() const;

    private:
    };



} // namespace eg


#endif // EDITOR_PLUGIN_H

