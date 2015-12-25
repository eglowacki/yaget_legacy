///////////////////////////////////////////////////////////////////////
// CollisionModule.h
//
//  Copyright 01/06/2008 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//      Private header file to Collision module
//
//
//  #include "CollisionModule.h"
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef COLLISION_MODULE_H
#define COLLISION_MODULE_H

#include "CollisionBase.h"
#include <wx/module.h>


namespace eg
{

    /*!
    This class module will intialize collision module
    */
    class CollisionModule : public wxModule
    {
    DECLARE_DYNAMIC_CLASS(CollisionModule)

    public:
        CollisionModule();
        virtual ~CollisionModule();

        // From wxModule
        virtual bool OnInit();
        virtual void OnExit();
    };

} // namespace eg

#endif // COLLISION_MODULE_H

