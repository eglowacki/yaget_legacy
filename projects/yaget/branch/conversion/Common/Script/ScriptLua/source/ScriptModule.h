///////////////////////////////////////////////////////////////////////
// ScriptModule.h
//
//  Copyright 01/06/2008 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//      Private header file to Script module
//
//
//  #include "ScriptModule.h"
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef SCRIPT_MODULE_H
#define SCRIPT_MODULE_H

#include "ScriptBase.h"
#include <wx/module.h>


namespace eg
{

    /*!
    This class module will intialize Script module
    */
    class ScriptModule : public wxModule
    {
    DECLARE_DYNAMIC_CLASS(ScriptModule)

    public:
        ScriptModule();
        virtual ~ScriptModule();

        // From wxModule
        virtual bool OnInit();
        virtual void OnExit();
    };

} // namespace eg

#endif // SCRIPT_MODULE_H

