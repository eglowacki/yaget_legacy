///////////////////////////////////////////////////////////////////////
// Signals.h
//
//  Copyright 10/23/2008 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//      Interface to Signal class which provides connection
//		to Dispather channels and can receive messages
//		on registered channels
//
//
//  #include "Signals.h"
//
//////////////////////////////////////////////////////////////////////
//! \file


// Signals.h
#ifndef SIGNALS_H
#define SIGNALS_H

#ifndef __cplusplus_cli
#error "This file is only used by cli/c++ (/clr option) compiler."
#endif // __cplusplus_cli

namespace yaget
{

	// forward declarations
	namespace managed 
	{ 
		delegate void CallbackDelegate(const eg::Message& /*msg*/);

		class Channel;
	}

	public delegate void DispatcherDelegate(System::UInt32 type, System::Object^ value);

	public ref class Signal
	{
	public:
		Signal(DispatcherDelegate^ dispatcherCallback, guid_t type);
		~Signal();
		!Signal();

	private:
		//! called by native dispatcher
		void onNativeCallback(const eg::Message& msg);

		DispatcherDelegate^ mDispatcherCallback;
		bool mIsDisposed;
		managed::Channel *mConnection;
		managed::CallbackDelegate^ mDelegate;
	};
}

#endif // SIGNALS_H
