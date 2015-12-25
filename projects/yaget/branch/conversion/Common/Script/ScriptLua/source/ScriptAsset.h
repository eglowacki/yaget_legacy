//////////////////////////////////////////////////////////////////////
// ScriptAsset.h
//
//  Copyright 05/30/2008 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//
//
//  #include "ScriptAsset.h"
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef SCRIPT_ASSET_H
#define SCRIPT_ASSET_H
#pragma once

#include "Asset/MediaAsset.h"
#include "Asset/AssetHandle.h"


namespace eg
{

    class ScriptAsset : public eg::MediaAsset<ScriptAsset>
    {
    public:
        static const guid_t kType = 0x348d0713;

        //! Represents first - source and second - fqn
        typedef std::pair<std::string, std::string> Data_t;

        //! Iterator to script data, which contains source and fqn
        //! \note When iterating over and script get's reloaded, all iterators are undefined,
        //! do we want to handle it? I could add std::vector<std::string> for source and fqn
        //! but for the time being, iterators are only used right after fully loaded script data
        //! and on the same thread.
        class const_iterator : public std::iterator<std::forward_iterator_tag, Data_t>
        {
        public:
            const_iterator(const std::vector<std::string>& sources, const std::vector<std::string>& fqns, bool bEnd);
            Data_t operator *() const;
            const_iterator& operator ++();
            const_iterator operator ++(int);
            friend bool operator ==(const const_iterator& lhs, const const_iterator& rhs);
            friend bool operator !=(const const_iterator& lhs, const const_iterator& rhs);

        private:
            std::vector<std::string>::const_iterator mSourceIt;
            std::vector<std::string>::const_iterator mFqnIt;
        };

        //! same as stl iterators interface
        const_iterator begin() const;
        const_iterator end() const;
        bool empty() const;

        ScriptAsset(const std::string& name);
        ~ScriptAsset();

        //! Create new script command (instance) if one already does not exist
        //! Otherwise do nothing
        //! \param cmdId uinque (to this instance of an asset) commnad id.
        //! \param cmdName command object name to create
        //! \param oId objectId this script instance belongs to
        //void NewCommand(uint64_t cmdId, const std::string& cmdName, uint64_t oId);

        // from IAsset
        virtual guid_t Type() const {return ScriptAsset::kType;}

    private:
        // From eg::MediaAsset
        virtual void SaveBlob(uint8_t *&pData, size_t& size) const;
        virtual void LoadBlob(const uint8_t *pData, size_t size, const std::string& streamName);

        void onFinishedLoading(Asset_t asset, const std::string& name);

        //! This will take Name() of this asset and make safe package name
        //! by removing any wild char and replacing spaces, '/' and '\' with underscores
        std::string makePackName() const;

        struct Package
        {
            void clear() {mSources.clear(); mFqns.clear();}

            std::vector<std::string> mSources;
            std::vector<std::string> mFqns;
        };

        Package mPackage;
        //! This is used to temporary save data while loding in LoadBlob()
        //! anfd it's main use to when source is hot-reloaded
        Package mPackageLoader;
        //! all user provided assembly names, seperate by new line
        //std::string mUserAssemblies;
    };


    typedef asset::AssetHandle<ScriptAsset, asset::AutoLock<ScriptAsset>, asset::LoadTypeDynamic> ScriptAsset_t;

} // namespace eg


EG_SERIALIZE_ASSET_VERSION(eg::ScriptAsset, 1);

#endif // SCRIPT_ASSET_H
