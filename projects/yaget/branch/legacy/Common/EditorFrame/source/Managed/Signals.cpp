//#pragma managed(push, off)
//#include "MessageInterface.h"
//#include "ObjectID.h"
//#include "Property/PropertyDataConversion.h"
//#include "IComponent.h"
//#include "Bridge.h"
//#pragma managed(pop)
//#include <gcroot.h>

//#include <string>

//#using <System.dll>

//#include "../IPlugin.h"
//#include "Signals.h"

//using namespace System;
//using namespace System::Threading;

#if 0
namespace
{

	boost::any convertValue(Object^ value)
	{
		using namespace eg;

		if (value == nullptr)
		{
			return boost::any();
		}

		try 
		{
			if (value->GetType() == bool::typeid)
			{
				bool v = safe_cast<bool>(value);
				return v;
			}
			else if (value->GetType() == Int32::typeid)
			{
				Int32 v = safe_cast<Int32>(value);
				return (int32_t)v;
			}
			else if (value->GetType() == UInt32::typeid)
			{
				UInt32 v = safe_cast<UInt32>(value);
				return (uint32_t)v;
			}
			else if (value->GetType() == Int64::typeid)
			{
				Int64 v = safe_cast<Int64>(value);
				return (int64_t)v;
			}
			else if (value->GetType() == UInt64::typeid)
			{
				UInt64 v = safe_cast<UInt64>(value);
				return (uint64_t)v;
			}
			else if (value->GetType() == Single::typeid)
			{
				Single v = safe_cast<Single>(value);
				return (float)v;
			}
			else if (value->GetType() == String::typeid)
			{
				String^ v = safe_cast<String^>(value);
				return yaget::stos(v);
			}
			else if (value->GetType() == yaget::component::InstanceId::typeid)
			{
				yaget::component::InstanceId tmpValue = safe_cast<yaget::component::InstanceId>(value);
				eg::component::InstanceId v = eg::component::make(tmpValue.oId, tmpValue.cId);
				return v;
			}
		}
		catch(InvalidCastException^ ex)
		{
			yaget::log::error("Could not cast Object value: " + ex->ToString());
		}

		return boost::any();
	}


	//! return value type based on type (Message guid_t type)
	Object^ convertValue(const eg::Message& msg)
	{
		using namespace eg;
		if (msg.mAnyData.empty())
		{
			// we allow empty value
			return gcnew Object();
		}

		if (msg.Is<bool>())
		{
			bool v = msg.GetValue<bool>(); 
			return v;
		}
		else if (msg.Is<int>())
		{
			Int32 v = msg.GetValue<int>();
			return v;
		}
		else if (msg.Is<unsigned int>())
		{
			UInt32 v = msg.GetValue<unsigned int>();
			return v;
		}
		else if (msg.Is<int64_t>())
		{
			Int64 v = msg.GetValue<int64_t>();
			return v;
		}
		else if (msg.Is<uint64_t>())
		{
			UInt64 v = msg.GetValue<uint64_t>();
			return v;
		}
		else if (msg.Is<float>())
		{
			Single v = msg.GetValue<float>();
			return v;
		}
		else if (msg.Is<std::string>())
		{
			String^ v = yaget::stos(msg.GetValue<std::string>());
			return v;
		}
		else if (msg.Is<eg::component::InstanceId>())
		{
			eg::component::InstanceId value = msg.GetValue<eg::component::InstanceId>();

			yaget::component::InstanceId v(value.first, value.second);;
			return v;
		}
		else
		{
			yaget::log::error("Could not cast Message.mValue to Object.Value.");
		}

		return gcnew Object();
	}

} // namespace


namespace yaget {

Signal::Signal(DispatcherDelegate^ dispatcherCallback, guid_t type)
: mDispatcherCallback(dispatcherCallback)
, mIsDisposed(false)
, mConnection(0)
{
	mDelegate = gcnew managed::CallbackDelegate(this, &Signal::onNativeCallback);
	managed::CallbackNative native = (managed::CallbackNative)System::Runtime::InteropServices::Marshal::GetFunctionPointerForDelegate(mDelegate).ToPointer();
	mConnection = new managed::Channel(native, type);
}


Signal::~Signal()
{
	if (!mIsDisposed) 
	{ 
		// release once only
		// clean up managed resources here.
		this->!Signal();
	}
	mIsDisposed = true;
}


Signal::!Signal() // maps to finalizer
{
	if (!mIsDisposed) 
	{
		// Clean up unmanaged resources here.
		delete mConnection;
		mConnection = 0;
	}
}


void Signal::onNativeCallback(const eg::Message& msg)
{
	//cli::array<Byte>^ bytes = gcnew cli::array<Byte>(length);
	//Marshal::Copy(data, bytes, 0, length);
	//ManagedData^ mdata = gcnew ManagedData(bytes);
	//OnCallback(mdata);
	if (mDispatcherCallback != nullptr)
	{
		// convert pValue into correct value wrapped into an object
		Object^ value = convertValue(msg);
		mDispatcherCallback(msg.mType, value);
	}
}


	public ref class Konsole
	{
	public:
		static void Execute(String^ cmd)
		{
			yaget::managed::konsoleExecute(stos(cmd));
		}
	};


	public ref class message sealed
	{
	public:
		static void Send(UInt32 type, Object^ value)
		{
			boost::any data = convertValue(value);
			eg::Message(type, data).Send();
		}

        static void Send(component::InstanceId iId, UInt32 type, Object^ value)
        {
            eg::component::InstanceId instanceId(iId.oId, iId.cId);
            if (eg::IComponent *pComponent = eg::component::get(instanceId))
            {
                boost::any data = convertValue(value);
                eg::Message msg(type, data);
                pComponent->HandleMessage(msg);
            }
            //eg::Message(type, data).Send();
        }
	};



} // namespace yaget

#endif // 0