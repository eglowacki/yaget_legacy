// Test ObjectManager
#include "ObjectsBase.h"
#include "IObjectManager.h"
#include "Registrate.h"
#include "File/VirtualFileFactory.h"
#include <boost/shared_ptr.hpp>
//#include <istream>
#include <UnitTest++.h>

using namespace eg;

namespace
{
    struct EnumComponents : public EnumComponentCallback
    {
        EnumComponents()
        : mNumComponents(0) {}

        virtual bool OnComponent(IComponent * /*pComponent*/)
        {
            mNumComponents++;
            return true;
        }

        int mNumComponents;
    };
}


/*!
Test all methods which need to work with empty ObjectManager correctly
*/
TEST(ObjectMananger_Interface)
{
    const component::ObjectId kDummyObjectId("DummyObject");
    const component::ObjectId kDummyObjectCopyId("DummyObjectCopy");
    const component::ComponentId kDummyComponentId("DummyType");
    const component::InstanceId iId(kDummyObjectId, kDummyComponentId);

    IObjectManager& om = REGISTRATE(IObjectManager);

    CHECK_EQUAL((IComponent *)0, om.FindComponentInstance(iId));
    CHECK_EQUAL((IComponent *)0, om.CreateComponent(iId, "DummyObject"));
    CHECK(om.IsObjectExists(kDummyObjectId) == false);
    CHECK(om.LoadObject(kDummyObjectId, VirtualFileFactory::istream_t()) == false);
    CHECK(om.CloneInstance(iId, kDummyObjectCopyId, "NewDummyType") == false);

    EnumComponents typeEnum;
    om.EnumObjectsByType(kDummyComponentId, typeEnum);
    CHECK_EQUAL(0, typeEnum.mNumComponents);

    EnumComponents objectEnum;
    om.EnumObjectsByObject(kDummyObjectId, objectEnum);
    CHECK_EQUAL(0, objectEnum.mNumComponents);

    EnumComponents allEnum;
    om.EnumObjectsByAll(allEnum);
    CHECK_EQUAL(0, allEnum.mNumComponents);

    CHECK_EQUAL("", om.GetComponentName(kDummyComponentId));

    const std::set<Hash>& types = om.GetComponentTypes();
    CHECK(types.empty() == true);
}
