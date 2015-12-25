// TestAreaComponent
#include "ComponentEnumsComponents.h"
#include "ComponentEnumsObjects.h"
#include "IObjectManager.h"
#include "IObjectTemplates.h"
#include "IComponentArea.h"
#include "IComponentEntity.h"
#include "IComponentWidget.h"
#include "Registrate.h"
#include "Message/Dispatcher.h"
#include "IdGameCache.h"
#include "File/VirtualFileSystem.h"
//#include "File/VirtualFileFactory.h"
#include <wx/log.h>
#include <iostream>
#include <boost/signal.hpp>

#include <UnitTest++.h>

using namespace eg;

struct TemplateCreator : boost::signals::trackable
{
    bool validObject;
    component::ObjectId oId;
    void operator()(Message& msg)
    {
        component::ObjectId currId = msg.GetValue<component::ObjectId>();

        if (currId == oId)
        {
            validObject = true;
        }
    }
};


TEST(Components_Templates)
{
    // connect to kObjectConstructed message
    Dispatcher& disp = REGISTRATE(Dispatcher);
    TemplateCreator templateCreator;
    templateCreator.validObject = false;
    disp[mtype::kObjectConstructed].connect(boost::ref(templateCreator));

    IObjectTemplates& ot = REGISTRATE(IObjectTemplates);

    // create test template
    IObjectTemplates::Components_t components;
    components.insert(ComponentType::kArea);
    components.insert(ComponentType::kWidget);

    component::ObjectId tIdTree(idspace::get_burnable());
    ot.Add(tIdTree, components);

    // Create object based on template
    component::ObjectId oIdOak(idspace::get_burnable());
    templateCreator.oId = oIdOak;
    CHECK(ot.Create(tIdTree, oIdOak, "Oak"));
    CHECK(templateCreator.validObject);

    VirtualFileSystem& fileSystem = REGISTRATE(VirtualFileSystem);
    CHECK(fileSystem.RegisterFactory("template", "MemoryFile", 0, false));

    // let save template data to file
    {
        VirtualFileFactory::ostream_t templateStream = fileSystem.AttachFileStream("pong.template");
        ot.Save(templateStream);
    }

    // remove template
    ot.Remove(tIdTree);

    component::ObjectId oIdSpruce(idspace::get_burnable());
    templateCreator.oId = oIdSpruce;
    templateCreator.validObject = false;
    CHECK(ot.Create(tIdTree, oIdSpruce, "Spruce") == false);
    CHECK(templateCreator.validObject == false);

    // load template data
    {
        VirtualFileFactory::istream_t templateStream = fileSystem.GetFileStream("pong.template");
        ot.Load(templateStream);
    }

    // Create object based on template
    component::ObjectId oIdPalm(idspace::get_burnable());
    templateCreator.oId = oIdPalm;
    templateCreator.validObject = false;
    CHECK(ot.Create(tIdTree, oIdPalm, "Palm"));
    CHECK(templateCreator.validObject);
}
