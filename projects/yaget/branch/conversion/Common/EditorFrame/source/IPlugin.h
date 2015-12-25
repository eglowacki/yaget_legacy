//////////////////////////////////////////////////////////////////////
// IPlugin.h
//
//  Copyright 12/28/2006 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//      Managed code used to glue classes and objected between MC++ and C#
//      This files provides IPlugin and Message classes
//
//
//  #include "IPlugin.h"
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef MANAGED_I_PLUGIN_H
#define MANAGED_I_PLUGIN_H
#pragma once

#include <Base.h>

//namespace
//{
//} // namespace


namespace yaget
{
    //-------------------------------------------------------------------------------------
    //! Helper function to convert string between C# and MC++ and vice versa
    inline System::String^ stos(const std::string& text)
    {
        System::String^ newText = gcnew System::String(text.c_str());
        return newText;
    }
    //-------------------------------------------------------------------------------------
    inline const std::string stos(System::String^ text)
    {
        std::string newText;
        using namespace System::Runtime::InteropServices;
        const char* chars = (const char*)(Marshal::StringToHGlobalAnsi(text)).ToPointer();
        newText = chars;
        Marshal::FreeHGlobal(System::IntPtr((void*)chars));
        return newText;
    }

	//! logger class with static methods
	public ref class log
	{
	public:
        static void trace(System::String^ text) {}
        static void message(System::String^ text) {}
		static void warning(System::String^ text) {}
		static void error(System::String^ text) {}
	};

	namespace component 
	{
		//! represent unique instance id for any component
		public value struct InstanceId sealed
		{
			InstanceId(System::UInt64 o_id, System::UInt64 c_id) : oId(o_id), cId(c_id) {}
			bool IsValid()
			{
				return oId != 0 && cId != 0;
			}

			virtual System::String^ ToString() override
			{
				return oId.ToString() + ":" + cId.ToString();
			}

			static bool operator==(InstanceId val1, InstanceId val2)
			{
				return val1.oId == val2.oId && val1.cId == val2.cId;
			}

			// this is are helper methods to convert id's into readable names
			enum class NameFormat
			{
				nfNumberNumber,	///< 100:45
				nfNumberName,	///< 100:View
				nfNameName,		///< OakTree:View
				nfNameNumber,	///< OakTree:45
				nfLong			///< Object 'OakTree' of type 'View' (100:45)
			};

			System::String^ Name(NameFormat formating)
			{
				switch (formating)
				{
				case NameFormat::nfNumberNumber:
					return ToString();
					break;

				case NameFormat::nfNumberName:
					return oId.ToString() + ":" + CName();
					break;

				case NameFormat::nfNameName:
					return OName() + ":" + CName();
					break;

				case NameFormat::nfNameNumber:
					return OName() + ":" + cId.ToString();
					break;

				case NameFormat::nfLong:
					return "Object '" + OName() + "' of type '" + CName() + "' (" + ToString() + ")";
					break;
				}

				return ToString();
			}

			//! Object Name
            System::String^ OName() {return "";}

			//! Component Name
            System::String^ CName() {return "";}

			static InstanceId zero()
			{
				return InstanceId(0, 0);
			}

			static InstanceId make(System::UInt64 viewId, System::String^ type)
			{
				eg::component::ComponentId cId(stos(type).c_str());
				return InstanceId(viewId, cId.GetValue());
			}

			//! Object id
			System::UInt64 oId;
			//! component type id
			System::UInt64 cId;
		};
	}


    //-------------------------------------------------------------------------------------
    // our plugin interface to .NET code
    namespace managed
    {
        /*
        //-------------------------------------------------------------------------------------
        //! All message id's from eg::mtype namespace are here to expose it .NET
        public ref class mtype
        {
        public:
            static const guid_t kDummy                  = eg::mtype::kDummy;
            static const guid_t kGetValidWindow         = eg::mtype::kGetValidWindow;
            static const guid_t kAllowTools             = eg::mtype::kAllowTools;
            static const guid_t kNewToolPlugin          = eg::mtype::kNewToolPlugin;
            static const guid_t kNewCommand             = eg::mtype::kNewCommand;
            static const guid_t kShutdownBegin          = eg::mtype::kShutdownBegin;
            static const guid_t kComponentNew           = eg::mtype::kComponentNew;
            static const guid_t kComponentDelete        = eg::mtype::kComponentDelete;
            static const guid_t kAllToolPluginInit      = eg::mtype::kAllToolPluginInit;
            static const guid_t kComponentSelected      = eg::mtype::kComponentSelected;
            static const guid_t kComponentDeselected    = eg::mtype::kComponentDeselected;
            static const guid_t kGetChoiceLabels        = eg::mtype::kGetChoiceLabels;
            static const guid_t kLoginInfo              = eg::mtype::kLoginInfo;
			static const guid_t kWorldList              = eg::mtype::kWorldList;
			static const guid_t kWorldSelected          = eg::mtype::kWorldSelected;
			static const guid_t kNetworkError			= eg::mtype::kNetworkError;


            //! This is send from each IPlugin object from Init
            //! mAnyData will point to IPlugin object
            static const guid_t kAddPanel               = 0xd2c50722;

            //! This is send from IPlugin object when it's no longer needed
            //! and it needs to be removed from system
            //! mAnyData will point to IPlugin object
            static const guid_t kRemovePanel            = 0x52e0c1cc;

            //! This is send after kAllToolPluginInit, which in most cases will be processed
            //! by one IPlugin which will assure that all of then are visible
            //! and will initialize DockManager, since all windows must be already created
            static const guid_t kShowPlugins            = 0x13157f9b;

        };
        */


        /*
        //-------------------------------------------------------------------------------------
        //! Simple replication of eg::Message class
        public ref class Message
        {
        public:
            Message() : mType(mtype::kDummy), mAnyData(nullptr)
            {
            }

            Message(guid_t id, Object^ object) : mType(id), mAnyData(object)
            {
            }

            //! This will send this message
            void Send();

            guid_t mType;
            Object^ mAnyData;
        };
        */

        /*
        //! Convert messaeg and it's data form C++ into .NET
        static Message^ Marshal(const eg::Message& msg);
        //! Convert messaeg and it's data .NET into C++
        static eg::Message Marshal(Message^ msg);
        //! Checks to see if we can convert .NET message into C++
        //! This is used in Message::Send() method when we get message from
        //! .NET ato see if we need to send it to C++ (which in most cases)
        //! will route it back to .NET, or just send to .NET
        static bool CanMarshal(Message^ msg);

        //-------------------------------------------------------------------------------------
        //! Callback method for any messages from Dispatcher
        public delegate void OnMessageDelegate(Message^);
        */


        //-------------------------------------------------------------------------------------
        //! plugin interface definition
        //! This interface will be derived in .NET
        public ref class IPlugin abstract
        {
        public:
            //! Called by plugin loader to let plugin initialized
            //virtual bool Init() = 0;

            //! Called by framework to close all plugins, release any resource
            //! and save any ui settings.
            virtual void Close() = 0;

            //! Return TRUE if we can clone this panel. otherwise false
            //virtual bool CanClone() = 0;
            //! If CanClone() returned TRUE, then this method
            //! will be called and new instance of this Plugin needs to be created.
            //virtual void Clone() = 0;
            //! Return TRUE if this panel is cloned. This is used in context menu
            //! to provide only cloning on original panels and allow to remove cloned panels
            //virtual bool IsClone() = 0;
            //! This is only called on cloned panels, which will remove it entirely form dock manager
            //virtual void RemoveClone() = 0;
            //! Each cloned panel will return name of the original panel
            //virtual property System::String^ OwnerName;

            //! Return name of this pluging (read only)
            property System::String^ Name
            {
                virtual System::String^ get() abstract;
                virtual void set(System::String^ /*n*/) sealed {}
            }
            /*
            //! Return Control this plugin represents (read only)
            virtual property System::Windows::Forms::Control^ Panel
            {
                virtual System::Windows::Forms::Control^ get() abstract;
                virtual void set(System::Windows::Forms::Control^ n) sealed {}
            }

            //! Return icon image for this panel.
            //! \note do we need to provide 2 versions of icons (large and small)
            //! or just provide one size and let the user scale it to fit it's usage
            virtual property System::Drawing::Image^ Icon
            {
                virtual System::Drawing::Image^ get() abstract;
                virtual void set(System::Drawing::Image^ n) sealed {}
            }
            */
        };

        /*
        public ref class PluginBase abstract : IPlugin
        {
        public:
            virtual bool CanClone() override { return false; }
            virtual void Clone() override {}
            virtual bool IsClone() override { return false; }
            virtual void RemoveClone() override {}
            virtual property String^ OwnerName
            {
                String^ get() override
                {
                    return Name;
                }
            }
        };
        */


        /*
        //-------------------------------------------------------------------------------------
        //! Static instance of getter of DockManager, set before any .NET plugins are executed
        public ref class DockManager
        {
        public:
            static property SandDockManager^ Docker;
        };
        */


#if 0
        //-------------------------------------------------------------------------------------
        //! Glue between C++ Dispatcher and .NET
        public ref class Dispatcher
        {
        public:
            static property Dispatcher^ Get
            {
                Dispatcher^ get()
                {
                    if (sInstance == nullptr)
                    {
                        sInstance = gcnew Dispatcher();
                    }
                    return sInstance;
                }
            }

            /*!
            This allows calling like
            Dispatcher.Get[mtype.kShutdownBegin] = some_C#_delegate;
            which will add some_C#_delegate to it's list of delegates for requested channel.
            */
            property OnMessageDelegate^ default[guid_t]
            {
                //! This always returns the same object to pass C# message
                //! to C++ Dispatcher
                OnMessageDelegate^ get(guid_t index)
                {
                    return OnMessageFrom_NET;
                }

                //! Add new delegate for some specific message
                //! \note we have no way of removing delegates once added.
                void set(guid_t index, OnMessageDelegate^ value)
                {
                    if (!mDispatchTable.ContainsKey(index))
                    {
                        AddNewCallback(index, value);
                        mDispatchTable[index] = value;
                    }
                    else
                    {
                        mDispatchTable[index] += value;
                    }

                    // we are hookup for notifications
                }
            }

            //! This is used internal to this module only
            void SendMessageTo_NET(Message^ msg)
            {
                if (mDispatchTable.ContainsKey(msg->mType))
                {
                    mDispatchTable[msg->mType](msg);
                }
            }

        private:
            Dispatcher() : mDispatchTable(gcnew Dictionary<guid_t, OnMessageDelegate^>())
            {
                OnMessageFrom_NET = gcnew OnMessageDelegate(this, &Dispatcher::SendMessageTo_C);
            }

            //! Helper method to add FromDispatcherCallback to new requested channel
            void AddNewCallback(guid_t index, OnMessageDelegate^ value);

            //! Called by C# code to send message to C++ Dispatcher
            //! which in turn will notify all C# hookup code
            void SendMessageTo_C(Message^ msg)
            {
                eg::Dispatcher& disp = eg::registrate::ref_cast<eg::Dispatcher>(_T("Dispatcher"));
                disp[msg->mType](Marshal(msg));
            }

            //! Called from C# to pass message to C++ Dispatcher
            OnMessageDelegate^ OnMessageFrom_NET;
            //! Mapping of dispather channels and delegates
            Dictionary<guid_t, OnMessageDelegate^> mDispatchTable;


            //! Our one and only instance (Factory pattern)
            static Dispatcher^ sInstance = nullptr;
        };

        //-------------------------------------------------------------------------------------
        /*!
        Used as a callback from Dispatcher.
        This is connect to C++ Dispatcher and
        when any of the channels are sending messages they are pushed into MC++ Dispatcher
        Going from C++ Dispatcher to C#
        */
        class FromDispatcherCallback
        {
        public:
            void operator()(eg::Message& msg) const
            {
                //Dispatcher::Get->[msg.mType](Marshal(msg));
                Dispatcher% disp = *Dispatcher::Get;
                // we need to tell Dispatcher that we are relying message to C# code
                //disp[msg.mType](Marshal(msg));
                disp.SendMessageTo_NET(Marshal(msg));
            }
        };

        void Dispatcher::AddNewCallback(guid_t index, OnMessageDelegate^ value)
        {
            // if this channel does not exist yet, we need to connect to C++ Dispatcher
            eg::Dispatcher& disp = eg::registrate::ref_cast<eg::Dispatcher>(_T("Dispatcher"));
            disp[index].connect(FromDispatcherCallback());
        }

        /*!
        object which provides app related methods
        */
        public ref class App
        {
        public:
            //! Return name of application, without any folder prefix and extension
            static String^ Name();
        };

#endif // 0
    } // namespace managed

} // namespace yaget

#endif // MANAGED_I_PLUGIN_H

