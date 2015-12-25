//////////////////////////////////////////////////////////////////////
// Bridge.h
//
//  Copyright 10/25/2008 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//      Provides functions, classes and any other
//		helper object to deal with connecting to c++ code
//		from cli
//
//  #include "Bridge.h"
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef MANAGED_BRIDGE_H
#define MANAGED_BRIDGE_H
#pragma once


// forward declarations
namespace eg { class Message; }
namespace {	class Connection; }


namespace yaget
{
	namespace managed
	{
		//! callback type to pass from cli to c++ to be triggered on
		//! connected channel
		typedef void (__stdcall *CallbackNative)(const eg::Message& /*msg*/);

		//! This object provides connection to specific dispatcher channel
		class Channel
		{
		public:
			Channel(CallbackNative callback, guid_t type);
			~Channel();

		private:
			Connection *mpConnection;
		};

		//! Temporary here until I figure out how to expose classes with boost::signal
		//! it crashes dll right now if boost::signal include is present in any clr files.
		void konsoleExecute(const std::string& cmd);

		namespace vf
		{
			//! return list of all files based on filter
			std::vector<std::string> GetFileList(const std::string& filter);

			//! Return fully path name to file
			std::string GetFqn(const std::string& fileName);

		} // namespace vf

	} // namespace managed
} // namespace yaget


#endif // MANAGED_BRIDGE_H
