#include "FooAsset.h"
#include "MessageInterface.h"
#include "File/AssetLoadSystem.h"
#include "File/VirtualFileSystem.h"
#include "IdGameCache.h"
#include "StringHelper.h"
#include <boost/bind.hpp>


namespace eg {

namespace
{

    yaget::VersionHandler<FooAsset> versionHandler;


    /*
    typedef std::pair<VirtualFileFactory::istream_t, std::string> StreamPiece_t;
    bool pieceFinder(const StreamPiece_t& piece, const std::string& filter)
    {
        return WildCompareISafe(filter, piece.second);
    }
    */

    class FooAssetFactory
    {
        FooAssetFactory(const FooAssetFactory&);
        FooAssetFactory& operator=(const FooAssetFactory&);

    public:
        FooAssetFactory(AssetLoadSystem &als)
        {
            log_trace(tr_util) << "FooAssetFactory object created.";

            versionHandler[1] = FooAssetFactory::version_1;
            versionHandler[2] = FooAssetFactory::version_2;

            als.RegisterFactory(FooAsset::kType, boost::bind(&FooAssetFactory::New, this, _1, _2),
                                                 boost::bind(&FooAssetFactory::Load, this, _1, _2, _3),
                                                 boost::bind(&FooAssetFactory::Save, this, _1, _2, _3),
                                                 boost::bind(&FooAssetFactory::Query, this, _1, _2));
        }

        ~FooAssetFactory()
        {
            log_trace(tr_util) << "FooAssetFactory object deleted.";
        }

    private:
        IAsset *New(const std::string& name, uint64_t asssetId)
        {
            IAsset *pAsset = new FooAsset(asssetId, name);
            return pAsset;
        }

        void Load(AssetLoadSystem::IAsset_t asset, VirtualFileFactory::istream_t stream, const std::string& name)
        {
            boost::shared_ptr<FooAsset> fooAsset = boost::dynamic_pointer_cast<FooAsset>(asset);
            if (fooAsset)
            {
                if (stream && stream::size(*stream))
                {
                    if (WildCompareISafe("*.options.foo", name))
                    {
                        // we have options let's process it
                        stream::stream_addapter<std::istream> astream(*stream);
                        astream >> fooAsset->mCycle;
                    }
                    else if (WildCompareISafe("*.foo", name))
                    {
                        stream::stream_addapter<std::istream> astream(*stream);
                        uint32_t version;
                        astream >> version;
                        fooAsset->load(version, astream);
                    }
                }
            }
        }

        void Save(AssetLoadSystem::IAsset_t asset, VirtualFileFactory::ostream_t stream, const std::string& name)
        {
            // we can catch ex::serialize exception here if needed.
            // AssetLoadSystem does this already.
            boost::shared_ptr<FooAsset> fooAsset = boost::dynamic_pointer_cast<FooAsset>(asset);
            if (fooAsset && stream)
            {
                if (WildCompareISafe("*.options.foo", name))
                {
                    stream::stream_addapter<std::ostream> astream(*stream);
                    astream << fooAsset->mCycle;
                }
                else if (WildCompareISafe("*.foo", name))
                {
                    stream::stream_addapter<std::ostream> astream(*stream);
                    astream << FooAsset::version;
                    fooAsset->save(astream);
                }
            }
        }

        std::vector<std::string> Query(AssetLoadSystem::IAsset_t asset, const VirtualFileSystem& vfs)
        {
            vfs;
            const std::string& name = asset->Name();
            // here we control the order of iteration over streams for usage in Load(...) and Save(...) methods
            std::vector<std::string> files;
            std::string file = name;
            file += ".options.foo";
            files.push_back(file);
            file = name;
            file += ".foo";
            files.push_back(file);
            return files;
        }

        // version handlers for FooAsset
        static void version_1(FooAsset& asset, std::istream& stream)
        {
            stream::stream_addapter<std::istream> astream(stream);
            astream >> asset.mColor;
        }

        // version handlers for FooAsset
        static void version_2(FooAsset& asset, std::istream& stream)
        {
            stream::stream_addapter<std::istream> astream(stream);
            astream >> asset.mCountry;
        }
    };


} // namespace


FooAsset::FooAsset(uint64_t id, const std::string& name)
: mId(id)
, mName(name)
, mColor(red)
, mCountry("")
, mCycle(false)
{
}


uint64_t FooAsset::GetId() const
{
    return mId;
}


const std::string& FooAsset::Name() const
{
    return mName;
}


IAsset::Locker_t FooAsset::Lock()
{
    return Locker_t();
}


guid_t FooAsset::Type() const
{
    return FooAsset::kType;
}


void FooAsset::HandleMessage(const Message &msg)
{
    msg;
}


void FooAsset::Register(AssetLoadSystem &als)
{
    static FooAssetFactory factory(als);
}


void FooAsset::load(uint32_t v, std::istream& stream)
{
    try
    {
        stream::LoadMarker file_marker(stream);
        versionHandler.load(v, *this, stream);
        log_trace("main") << "Loaded '" << Name() << "' <FooAsset> asset. version = " << v << ", Color: " << mColor;
    }
    catch (const ex::serialize& e)
    {
        mColor = red;
        log_error << "Could not load asset '" << Name() << "' of type: " << logs::hex<guid_t>(FooAsset::kType) << ". Exception: " << e.what();
    }
}


void FooAsset::save(std::ostream& stream) const
{
    log_trace("main") << "Saving '" << Name() << "' <FooAsset> asset. version = " << FooAsset::version << ", Color: " << mColor;

    stream::SaveMarker file_marker(stream);
    stream::stream_addapter<std::ostream> astream(stream);
    astream << mColor;
    astream << mCountry;
}


} // namespace eg

