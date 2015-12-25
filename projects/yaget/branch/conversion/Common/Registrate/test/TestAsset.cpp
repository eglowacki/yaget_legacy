// Test IAsset, Asset and AssetLoadSystem
#include "BaseRegistrate.h"
#include "Config/ConfigHelper.h"
#include "Math/Vector.h"
#include "Asset/Asset.h"
#include <wx/sstream.h>
#include <wx/fileconf.h>
#include <UnitTest++.h>
#pragma warning (disable : 4267) // var' : conversion from 'size_t' to 'type', possible loss of data
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#pragma warning (default : 4267)


class UnitTestAsset : public eg::Asset<UnitTestAsset>
{
public:
    typedef eg::asset::UserData U;

    static const guid_t kType = 0xb3bb64b1;

    UnitTestAsset(const std::string& name)
    : eg::Asset<UnitTestAsset>(name)
    , mDummy(0)
    {
    }

private:
    //! look for BOOST_CLASS_VERSION below to see which version is used
    friend class boost::serialization::access;

    // Split version of serialize methods
    template<class Archive>
    void save(Archive & ar, const unsigned int /*version*/) const
    {
        DataLocker lock(*this);
        ar & mDummy;
    }

    template<class Archive>
    void load(Archive & ar, const unsigned int /*version*/)
    {
        DataLocker lock(*this);
        ar & mDummy;
    }

    BOOST_SERIALIZATION_SPLIT_MEMBER()

    // Dummy variable showing saving and loading
    int mDummy;
};

// outside any namespace
EG_SERIALIZE_ASSET_VERSION(UnitTestAsset, 1);


TEST(AssetLoad)
{
    //eg::AutoRegisterAssetFactory<UnitTestAsset> unitTestFactory;
}


