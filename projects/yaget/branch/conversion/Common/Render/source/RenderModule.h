///////////////////////////////////////////////////////////////////////
// RenderModule.h
//
//  Copyright 10/9/2005 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//      Private header file to Render module
//
//
//  #include "RenderModule.h"
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef RENDER_MODULE_H
#define RENDER_MODULE_H

#include "RenderBase.h"
#include <wx/module.h>
#pragma warning (disable:4312)  // 'variable' : conversion from 'type' to 'type' of greater size
    #include <atlcomcli.h>
    #include <d3d9.h>
#pragma warning (default : 4312)


namespace eg
{

    /*!
    This class module will intialize Direc3D object
    */
    class RenderModule : public wxModule
    {
        DECLARE_DYNAMIC_CLASS(RenderModule)

        public:
            RenderModule();
            virtual ~RenderModule();

            // From wxModule
            virtual bool OnInit();
            virtual void OnExit();

            //! We expose this as a static, since we only can have one instance
            //! and for easy access
            static CComPtr<IDirect3D9> D3D;
    };

} // namespace eg

#endif // RENDER_MODULE_H

