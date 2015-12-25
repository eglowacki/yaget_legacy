#include "Precompiled.h"
//#include "vld.h"
#include "ObjectsModule.h"
#include "ObjectManager.h"
#include "ObjectTemplates.h"
#include "MessageInterface.h"
#include "Registrate.h"
#include "Logger/Log.h"
//#include "BaseDLLs.h"
#include <wx/dynlib.h>
#include <boost/shared_ptr.hpp>

WXDLL_ENTRY_FUNCTION();


namespace eg {

IMPLEMENT_DYNAMIC_CLASS(ObjectsModule, wxModule)


ObjectsModule::ObjectsModule()
{
}


ObjectsModule::~ObjectsModule()
{
}


bool ObjectsModule::OnInit()
{
    wxLogTrace(TR_OBJECTS, "ObjectsModule Initialized.");

    Dispatcher& dispatcher = registrate::ref_cast<Dispatcher>("Dispatcher");
    config::Konsole& konsole = registrate::ref_cast<config::Konsole>("Konsole");

    mObjectManager.reset(new ObjectManager(dispatcher, konsole));
    registrate::RegisterObject(mObjectManager.get(), "IObjectManager");

    mObjectTemplates.reset(new ObjectTemplates);
    registrate::RegisterObject(mObjectTemplates.get(), "IObjectTemplates");

    return true;
}


void ObjectsModule::OnExit()
{
    registrate::RegisterObject(boost::any(), wxT("IObjectManager"));
    mObjectManager.reset();

    registrate::RegisterObject(boost::any(), wxT("IObjectTemplates"));
    mObjectTemplates.reset();

    wxLogTrace(TR_OBJECTS, "ObjectsModule Shutdown.");
}


} // namespace eg//
