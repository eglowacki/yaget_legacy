///////////////////////////////////////////////////////////////////////
// ObjectsModule.h
//
//  Copyright 10/15/2005 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//      Private header file to Objects module
//
//
//  #include "ObjectsModule.h"
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef OBJECTS_MODULE_H
#define OBJECTS_MODULE_H

#include "ObjectsBase.h"
#include <wx/module.h>
#include <boost/shared_ptr.hpp>

namespace eg
{
    class IObjectManager;
    class IObjectTemplates;

    /*!
    This class module will initialize Direc3D object
    */
    class ObjectsModule : public wxModule
    {
        DECLARE_DYNAMIC_CLASS(ObjectsModule)

    public:
        ObjectsModule();
        virtual ~ObjectsModule();

        // From wxModule
        virtual bool OnInit();
        virtual void OnExit();

    private:
        boost::shared_ptr<IObjectManager> mObjectManager;
        boost::shared_ptr<IObjectTemplates> mObjectTemplates;
    };

} // namespace eg

#endif // OBJECTS_MODULE_H

