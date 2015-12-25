///////////////////////////////////////////////////////////////////////
// ComponentsModule.h
//
//  Copyright 10/15/2005 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//      Private header file to Objects module
//
//
//  #include "ComponentsModule.h"
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef COMPONENTS_MODULE_H
#define COMPONENTS_MODULE_H

#include "ComponentsBase.h"
#include <wx/module.h>
#include <boost/shared_ptr.hpp>

namespace eg
{

    /*!
    This class module will initialize Direc3D object
    */
    class ComponentsModule : public wxModule
    {
        DECLARE_DYNAMIC_CLASS(ComponentsModule)

    public:
        ComponentsModule();
        virtual ~ComponentsModule();

        // From wxModule
        virtual bool OnInit();
        virtual void OnExit();

    private:
    };

} // namespace eg

#endif // COMPONENTS_MODULE_H

