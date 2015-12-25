// Test Math module
#include "BaseRegistrate.h"
#include "Math/Vector.h"
#include "Property/PropertyData.h"
#include "Registrate.h"
#include "File/ArchiveHelpers.h"
#include "File/VirtualFileSystem.h"
#include <boost/bind.hpp>
#include <UnitTest++.h>

using namespace eg;

component::InstanceId testId(component::ObjectId(1000), component::ComponentId("Test"));


class TestPropertyHolder
{
public:
    TestPropertyHolder()
    : mPropertySet(testId, "TestPropertyHolder", boost::bind(&TestPropertyHolder::onPropertyChanged, this, _1))
    , mState(get_propV<int32_t>(mPropertySet, "State", 5))
    , mSpeed(get_propV<float>(mPropertySet, "Speed", 23.5f))
    , mColor(get_prop<Color3_Tag, Vector3>(mPropertySet, "Color", v3::ONE()))
    , mStateChanged(0)
    , mSpeedChanged(0)
    , mColorChanged(0)
    {
    }

    void onPropertyChanged(const IProperty *pProperty)
    {
        if (pProperty->GetPropId() == mState.GetPropId())
        {
            mStateChanged++;
        }
        else if (pProperty->GetPropId() == mSpeed.GetPropId())
        {
            mSpeedChanged++;
        }
        else if (pProperty->GetPropId() == mColor.GetPropId())
        {
            mColorChanged++;
        }
    }

    int32_t mStateChanged, mSpeedChanged, mColorChanged;
    PropertySet mPropertySet;
    Property<int32_t>& mState;
    Property<float>& mSpeed;
    Property<Vector3>& mColor;

};

static int32_t timesHitA = 0;
static void onPropertyChangedA(const IProperty * /*pProperty*/)
{
    timesHitA++;
}


static int32_t timesHitB = 0;
static void onPropertyChangedB(const IProperty * /*pProperty*/)
{
    timesHitB++;
}

TEST(PropertyDataClass)
{
    //-------------------------------------------------------------------------------------
    PropertyOwn<int32_t, int32_t> intData(0, "IntData", 5, 0);
    CHECK_EQUAL(5, intData());
    intData = 10;
    CHECK_EQUAL(10, intData());
    int32_t tempValue = intData;
    CHECK_EQUAL(10, tempValue);

    //-------------------------------------------------------------------------------------
    PropertySet propertySet;
    propertySet.AddProperty<float, float>("FloatData", 10.0f);

    float newFloatValue = prop_cast<float>(propertySet, "FloatData");
    CHECK_EQUAL(10.0f,newFloatValue);

    prop_cast<float>(propertySet, "FloatData") = 20.0f;
    newFloatValue = prop_cast<float>(propertySet, "FloatData");
    CHECK_EQUAL(20.0f, newFloatValue);

    //-------------------------------------------------------------------------------------
    TestPropertyHolder testPropertyHolder;

    CHECK_EQUAL(5, testPropertyHolder.mState());
    testPropertyHolder.mState = 10;
    CHECK_EQUAL(10, testPropertyHolder.mState());

    CHECK_CLOSE(23.5f, testPropertyHolder.mSpeed(), 0.01f);
    testPropertyHolder.mSpeed = 54.4f;
    CHECK_CLOSE(54.4f, testPropertyHolder.mSpeed(), 0.01f);

    CHECK_EQUAL(v3::ONE(), testPropertyHolder.mColor());
    testPropertyHolder.mColor = v3::YONE();
    CHECK_EQUAL(v3::YONE(), testPropertyHolder.mColor());

    CHECK_EQUAL(1, testPropertyHolder.mStateChanged);
    CHECK_EQUAL(1, testPropertyHolder.mSpeedChanged);
    CHECK_EQUAL(1, testPropertyHolder.mColorChanged);

    VirtualFileSystem& fileSystem = REGISTRATE(VirtualFileSystem);

    CHECK(fileSystem.RegisterFactory("prop", "MemoryFile"));

    VirtualFileFactory::ostream_t os = fileSystem.AttachFileStream("UnitTest.prop");
    CHECK(os != 0);
    SaveToArchive<boost::archive::text_oarchive>(testPropertyHolder.mPropertySet, *os);

    os.reset();

    testPropertyHolder.mSpeed = 0;
    CHECK_EQUAL(0, testPropertyHolder.mSpeed());
    testPropertyHolder.mState = 0;
    CHECK_EQUAL(0, testPropertyHolder.mState());
    testPropertyHolder.mColor = v3::ONE();
    CHECK_EQUAL(v3::ONE(), testPropertyHolder.mColor());

    testPropertyHolder.mPropertySet.AddNotifySlot(&onPropertyChangedA, "PropertyTestChangedA");

    VirtualFileFactory::istream_t is = fileSystem.GetFileStream("UnitTest.prop");
    CHECK(is != 0);
    LoadFromArchive<boost::archive::text_iarchive>(testPropertyHolder.mPropertySet, *is);
    CHECK_EQUAL(3, timesHitA);

    CHECK_EQUAL(10, testPropertyHolder.mState());
    CHECK_CLOSE(54.4f, testPropertyHolder.mSpeed(), 0.01f);
    CHECK_EQUAL(v3::YONE(), testPropertyHolder.mColor());

    CHECK(testPropertyHolder.mColor.IsType<Color3_Tag>());
    CHECK(testPropertyHolder.mColor.IsValue<Vector3>());

    testPropertyHolder.mStateChanged = 0;
    testPropertyHolder.mSpeedChanged = 0;
    testPropertyHolder.mColorChanged = 0;

    testPropertyHolder.mPropertySet.AddNotifySlot(&onPropertyChangedB, "PropertyTestChangedB");
    is = fileSystem.GetFileStream("UnitTest.prop");
    CHECK(is != 0);
    LoadFromArchive<boost::archive::text_iarchive>(testPropertyHolder.mPropertySet, *is);
    CHECK_EQUAL(6, timesHitA);
    CHECK_EQUAL(3, timesHitB);


    testPropertyHolder.mPropertySet.RemoveNotifySlot("PropertyTestChangedA");

    is = fileSystem.GetFileStream("UnitTest.prop");
    CHECK(is != 0);
    LoadFromArchive<boost::archive::text_iarchive>(testPropertyHolder.mPropertySet, *is);

    CHECK_EQUAL(2, testPropertyHolder.mStateChanged);
    CHECK_EQUAL(2, testPropertyHolder.mSpeedChanged);
    CHECK_EQUAL(2, testPropertyHolder.mColorChanged);

    CHECK_EQUAL(6, timesHitB);
    CHECK_EQUAL(6, timesHitA);


    VirtualFileFactory::ostream_t propOut = fileSystem.AttachFileStream("DataSet.prop");
    CHECK(propOut != 0);
    CHECK(SaveProperySet<boost::archive::text_oarchive>(testPropertyHolder.mPropertySet, *propOut));

    PropertySet newPropertySet;
    CHECK(testPropertyHolder.mPropertySet != newPropertySet);

    VirtualFileFactory::istream_t propIn = fileSystem.GetFileStream("DataSet.prop");
    CHECK(propIn != 0);
    CHECK(LoadPropertySet<boost::archive::text_iarchive>(newPropertySet, *propIn));

    CHECK(newPropertySet == newPropertySet);
    CHECK(testPropertyHolder.mPropertySet == newPropertySet);

    newPropertySet["State"] = (int32_t)47;
    CHECK(testPropertyHolder.mPropertySet != newPropertySet);
    int32_t settedValue = newPropertySet["State"];
    CHECK_EQUAL(47, settedValue);
}

