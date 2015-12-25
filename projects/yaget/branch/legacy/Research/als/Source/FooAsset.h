/////////////////////////////////////////////////////////////////////////
// FooAsset.h
//
//  Copyright 4/4/2009 Edgar Glowacki.
//
// NOTES:
//
//
// #include "FooAsset.h"
//
/////////////////////////////////////////////////////////////////////////
//! \file

#ifndef FOO_ASSET_H
#define FOO_ASSET_H
#pragma once

#include "Asset/IAsset.h"
#include "Logger/Log.h"
#include "Exception/Exception.h"

namespace eg
{
    class AssetLoadSystem;
    class IdGameCache;
    namespace {class FooAssetFactory;}

    class FooAsset : public IAsset
    {
    public:
        static const guid_t kType = 0x8be15aa6;

        FooAsset(uint64_t id, const std::string& name);

        enum Lamp {red, yellow, green};

        Lamp Color() const {return mColor;}
        void SetColor(Lamp color) {mColor = color;}
        void SetCountry(const std::string& country) {mCountry = country;}

        // from IAsset
        virtual uint64_t GetId() const;
        virtual const std::string& Name() const;
        virtual Locker_t Lock();
        virtual guid_t Type() const;
        virtual void HandleMessage(const Message &msg);

        static void Register(AssetLoadSystem &als);

    private:
        uint64_t mId;
        std::string mName;
        Lamp mColor;
        std::string mCountry;
        bool mCycle;

        friend FooAssetFactory;
        void load(uint32_t v, std::istream& stream);
        void save(std::ostream& stream) const;

        static const uint32_t version = 2;
    };

    typedef boost::shared_ptr<FooAsset> FooAsset_t;


} // namespace eg

#endif // FOO_ASSET_H

