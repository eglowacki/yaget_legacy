#include "Precompiled.h"
//#include "vld.h"
#include "RegistrateModule.h"
#include "Registrate.h"
#include "Logger/Log.h"
#include <wx/dynlib.h>
#include <wx/filename.h>
#include <boost/bind.hpp>

WXDLL_ENTRY_FUNCTION();


namespace eg {


IMPLEMENT_DYNAMIC_CLASS(RegistrateModule, wxModule)


RegistrateModule::RegistrateModule()
{
}


RegistrateModule::~RegistrateModule()
{
}


bool RegistrateModule::OnInit()
{
    wxLogTrace(TR_REGISTRATE, "Module Initialized.");
    return true;
}


void RegistrateModule::OnExit()
{
    wxLogTrace(TR_REGISTRATE, "Module Shutdown.");
}


} // namespace eg



