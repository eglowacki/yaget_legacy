///////////////////////////////////////////////////////////////////////
// ReplicateModule.h
//
//  Copyright 12/01/2007 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//      Private header file to Replicate module
//
//
//  #include "ReplicateModule.h"
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef REPLICATE_MODULE_H
#define REPLICATE_MODULE_H

#include "ReplicateBase.h"
#include <wx/module.h>


namespace eg
{

    /*!
    This class module will intialize Replicate module
    */
    class ReplicateModule : public wxModule
    {
        DECLARE_DYNAMIC_CLASS(ReplicateModule)

        public:
            ReplicateModule();
            virtual ~ReplicateModule();

            // From wxModule
            virtual bool OnInit();
            virtual void OnExit();
    };

} // namespace eg

#endif // REPLICATE_MODULE_H

