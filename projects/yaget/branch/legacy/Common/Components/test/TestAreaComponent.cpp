// TestAreaComponent
#include "ComponentEnumsComponents.h"
#include "ComponentEnumsObjects.h"
#include "IObjectManager.h"
#include "IComponentArea.h"
#include "IComponentEntity.h"
#include "IdGameCache.h"
#include "File/VirtualFileSystem.h"
#include <iostream>
#include <UnitTest++.h>

using namespace eg;

namespace
{
    component::ObjectId comparedGdyniaId((uint32_t)0);



} // namespace


TEST(Components_Area)
{
    component::InstanceId iId(component::ObjectId(10), ComponentType::kArea);
    IComponent *pComponent = component::newc("CharacterSelection", iId);
    CHECK(pComponent != 0);

    IComponentArea *pArea = component::cast<IComponentArea>(pComponent);
    CHECK(pArea != 0);

    std::vector<component::ObjectId> objects;
    objects.push_back(component::ObjectId("Menu"));
    objects.push_back(component::ObjectId("Sound"));
    //Property<std::vector<ObjectId> >& areaProperty = prop_cast<std::vector<ObjectId> >(pComponent, "Components");
    //prop_cast<std::vector<ObjectId> >(pComponent, "Components") = objects;

    IPropertySet& propSet = pComponent->GetPropertyData();
    propSet["Objects"] = objects;

    std::vector<component::ObjectId> newObjects = propSet["Objects"];
    CHECK(objects == newObjects);
}



TEST(Components_Entity)
{
    component::InstanceId iId(component::ObjectId(10), ComponentType::kEntity);
    IComponent *pComponent = component::newc("CharacterSelection", iId);
    CHECK(pComponent != 0);

    IComponentEntity *pEntity = component::cast<IComponentEntity>(pComponent);
    CHECK(pEntity != 0);

    IPropertySet& propSet = pComponent->GetPropertyData();
    propSet["Position"] = Vector3(1, 2, 3);
    propSet["Scale"] = Vector3(4, 5, 6);
    propSet["Orientation"] = Quaternion(7, 8, 9, 10);

    Vector3 newPosition = propSet["Position"];
    Vector3 newScale = propSet["Scale"];
    Quaternion newOrientation = propSet["Orientation"];

    CHECK_EQUAL(Vector3(1, 2, 3), newPosition);
    CHECK_EQUAL(Vector3(4, 5, 6), newScale);
    CHECK_EQUAL(Quaternion(7, 8, 9, 10), newOrientation);
}



TEST(Components_InitResources)
{
    VirtualFileSystem& fileSystem = REGISTRATE(VirtualFileSystem);
    CHECK(fileSystem.RegisterFactory("object", "MemoryFile", 0, false));
    CHECK(fileSystem.RegisterFactory("component", "MemoryFile", 0, false));
    CHECK(fileSystem.RegisterFactory("map", "MemoryFile", 0, false));
}



TEST(Components_CreateResources)
{
    VirtualFileSystem& fileSystem = REGISTRATE(VirtualFileSystem);
    IObjectManager& om = REGISTRATE(IObjectManager);

    component::ObjectId gdyniaId = idspace::get_burnable();
    comparedGdyniaId = gdyniaId;
    IComponentEntity *pCityNode = component::newc<IComponentEntity>("Gdynia", gdyniaId);
    CHECK(pCityNode);

    IPropertySet& cityPropSet = property::cast(component::getId(pCityNode));
    cityPropSet["Position"] = Vector3(1, 2, 3);
    cityPropSet["Scale"] = Vector3(4, 5, 6);
    cityPropSet["Orientation"] = Quaternion(7, 8, 9, 10);

    VirtualFileFactory::ostream_t ostream = fileSystem.AttachFileStream("gdynia.object");
    CHECK(ostream != 0);
    om.SaveObject(gdyniaId, ostream);

    // let's save our gdynia id value here
    ostream = fileSystem.AttachFileStream("poland.map");
    CHECK(ostream != 0);
    (*ostream) << gdyniaId;

    component::freec(pCityNode);
    Message(mtype::kFrameTick, 0.0f).Send();
}


TEST(Components_LoadResources)
{
    VirtualFileSystem& fileSystem = REGISTRATE(VirtualFileSystem);
    IObjectManager& om = REGISTRATE(IObjectManager);

    VirtualFileFactory::istream_t istream = fileSystem.GetFileStream("poland.map");
    CHECK(istream != 0);

    component::ObjectId gdyniaId;
    (*istream) >> gdyniaId;
    CHECK(gdyniaId == comparedGdyniaId);

    istream = fileSystem.GetFileStream("gdynia.object");
    CHECK(istream != 0);
    CHECK(om.LoadObject(gdyniaId, istream));

    IComponentEntity *pCityNode = component::get<IComponentEntity>(gdyniaId);
    CHECK(pCityNode);

    IPropertySet& cityPropSet = property::cast(component::getId(pCityNode));
    CHECK(cityPropSet.IsValid());

    Vector3 newPosition = cityPropSet["Position"];
    Vector3 newScale = cityPropSet["Scale"];
    Quaternion newOrientation = cityPropSet["Orientation"];

    CHECK_EQUAL(Vector3(1, 2, 3), newPosition);
    CHECK_EQUAL(Vector3(4, 5, 6), newScale);
    CHECK_EQUAL(Quaternion(7, 8, 9, 10), newOrientation);
}


TEST(Components_SaveLoad)
{
    VirtualFileSystem& fileSystem = REGISTRATE(VirtualFileSystem);
    if (VirtualFileFactory::ostream_t ostream = fileSystem.AttachFileStream("SaveTest.object"))
    {
        stream::SaveMarker saveMarker(*ostream);

        // save block of data here ---------
        std::string version("Version");
        (*ostream) << version;

        int32_t number = 56;
        (*ostream) << " " << number;
        // ---------------------------------
    }

    if (VirtualFileFactory::istream_t istream = fileSystem.GetFileStream("SaveTest.object"))
    {
        stream::LoadMarker loadMarker(*istream);

        // load block of data here ---------
        std::string version;
        (*istream) >> version;
        CHECK_EQUAL("Version", version);

        int32_t number;
        (*istream) >> number;
        CHECK_EQUAL(56, number);
        // ---------------------------------
    }
}
