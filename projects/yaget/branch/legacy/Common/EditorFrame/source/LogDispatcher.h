///////////////////////////////////////////////////////////////////////
// LogDispatcher.h
//
//  Copyright 10/18/2008 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//      LoagDispatcher will trigger messages for each log line printed
//
//
//  #include "LogDispatcher.h"
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef LOG_DISPATCHER_H
#define LOG_DISPATCHER_H

#include "EditorBase.h"
#include "wx/log.h"


namespace yaget
{
	namespace mtype
	{
		//! Each call to LogDispatcher::DoLogString(...)
		//! will trigger this event
		//! boost::any one full text line 
		static const guid_t kLogDispatcher = 0x16053109;


	} // namespace mtype


#ifndef SWIG
	class LogDispatcher : public wxLog
	{
	public:
		virtual void DoLogString(const wxString& msg, time_t timestamp);
	};

	namespace log
	{
		void Activate();
		void Deactivate();
	} // namespace log

#endif   // SWIG

} // namespace yaget

#endif // LOG_DISPATCHER_H
