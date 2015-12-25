//////////////////////////////////////////////////////////////////////
// AssetLoadSystem.h
//
//  Copyright 1/2/2007 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//      Glue between C++ AssetLoadSystem and (MC++) .NET
//
//
//  #include "Managed/AssetLoadSystem.h"
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef MANAGED_ASSET_LOAD_SYSTEM_H
#define MANAGED_ASSET_LOAD_SYSTEM_H
#pragma once

namespace eg
{
    namespace managed
    {
        public ref class AssetLoadSystem
        {
        public:
            //! Used as param in GetAsset
            enum class Load { eLoadAsync, eLoadBlocking };

            //! Return asset of specific type and load data from persistent storage if one exists
            generic <typename T>
            Object^ GetAsset(String^ name, Load loadType);


            /*!
            This will return list of all asset names fitting filter
            \param filter wild card filter to get assets
            \return vector of strings which are asset names. This will not have any path only asset names
            */
            List<String^>^ GetAssetList(String^ filter);

            //! Save asset to persistent storage
            bool SaveAsset(String^ name);

            static property AssetLoadSystem^ Get
            {
                AssetLoadSystem^ get()
                {
                    if (sInstance == nullptr)
                    {
                        sInstance = gcnew AssetLoadSystem();
                    }
                    return sInstance;
                }
            }

        private:
            AssetLoadSystem();

            Object^ GetAsset(String^ name, Load loadType, Type^ assetType);
            //! List of created assets
            Dictionary<String^, Object^>^ mAssetList;

            //! Our one and only instance (Factory pattern)
            static AssetLoadSystem^ sInstance = nullptr;
        };

    } // namespace managed
} // namespace eg

#endif // MANAGED_ASSET_LOAD_SYSTEM_H

