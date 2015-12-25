#include "Precompiled.h"
#include "LogDispatcher.h"
#include "MessageInterface.h"

// file touched during perforce crash
// Dispatcher.cpp

namespace yaget
{
	using namespace eg;

void LogDispatcher::DoLogString(const wxString& msg, time_t /*timestamp*/)
{
	Message(mtype::kLogDispatcher, std::string(msg.c_str())).Send();
}


namespace log
{

void Activate()
{
	wxLogChain *logChain = new wxLogChain(new LogDispatcher);
    logChain;
}


void Deactivate()
{
	if (wxLogChain *pOldLog = dynamic_cast<wxLogChain *>(wxLog::GetActiveTarget()))
	{
		wxLog *pLastPlog = pOldLog->GetOldLog();
		pOldLog->DetachOldLog();
		wxLog::SetActiveTarget(pLastPlog);
		delete pOldLog;
	}
}

} // namespace log
} // namespace yaget
