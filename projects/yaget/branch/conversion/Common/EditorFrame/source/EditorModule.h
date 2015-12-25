///////////////////////////////////////////////////////////////////////
// EditorModule.h
//
//  Copyright 10/15/2005 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//      Private header file to Editor module
//
//
//  #include "EditorModule.h"
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef EDITOR_MODULE_H
#define EDITOR_MODULE_H

#include "EditorBase.h"
#include <wx/module.h>
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>

namespace eg
{
    class IEditorFrame;
    class Message;

    /*!
    This class module will initialize Direc3D object
    */
    class EditorModule : public wxModule
    {
        DECLARE_DYNAMIC_CLASS(EditorModule)

    public:
        EditorModule();
        virtual ~EditorModule();

        // From wxModule
        virtual bool OnInit();
        virtual void OnExit();

    private:
    };

} // namespace eg

#endif // EDITOR_MODULE_H

