///////////////////////////////////////////////////////////////////////
// RegistrateModule.h
//
//  Copyright 1/18/2006 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//      Private header file to Registrate module
//
//
//  #include "RegistrateModule.h"
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef REGISTRATE_MODULE_H
#define REGISTRATE_MODULE_H
#pragma once


#include "BaseRegistrate.h"
#include "MessageInterface.h"
#include "Asset/Asset.h"
#include <wx/module.h>
#include <boost/shared_ptr.hpp>


namespace eg
{
	//class BitmapAsset;
	//class TextureAsset;
	//typedef AutoRegisterAssetFactory<BitmapAsset> BitmapFactory_t;
	//typedef AutoRegisterAssetFactory<TextureAsset> TextureFactory_t;

    /*!
    This class module will initialize Registrate object and activate all
    valid Factories and provide central access to it
    */
    class RegistrateModule : public wxModule
    {
        DECLARE_DYNAMIC_CLASS(RegistrateModule)

    public:
        RegistrateModule();
        virtual ~RegistrateModule();

        // From wxModule
        virtual bool OnInit();
        virtual void OnExit();

	private:
		//boost::shared_ptr<BitmapFactory_t> mBitmapFactory;
		//boost::shared_ptr<TextureFactory_t> mTextureFactory;
    };

} // namespace eg

#endif // REGISTRATE_MODULE_H


