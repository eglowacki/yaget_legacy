/*
#include "EditorBase.h"
#include "ObjectIterators.h"
#include "Property/IProperty.h"
#include "Interface/IScript.h"
#include "IRenderer.h"
#include "IComponentView.h"
#include "Bridge.h"
#include "IdGameCache.h"
#include "wx/log.h"
#include <algorithm>
*/

//#include <gcroot.h>
//#using <System.dll>

//#include "../IPlugin.h"
//#include "Signals.h"

#if 0

// forward declaration
namespace eg { class IPropertySet;}

using namespace System;
using namespace System::Collections::Generic;

//using namespace System::Runtime::InteropServices;
//using namespace System::Reflection;
namespace yaget {
namespace component {

	//! Types of property we provide, besides the built in types
	//! represented by Vector3 and it provides color picker
	public value struct Color3_Tag sealed {};

	//! represented by Vector4 and it provides color picker (alpha is still not exposed)
	public value struct Color4_Tag sealed {};

	//! represented by std::string and provides file open dialog
	//! This will send kGetResourcePath, where PropertyInfo_t is data
	//! Second message is kGetResourceFilter, where PropertyInfo_t is data
	public value struct File_Tag sealed {};

	//! represented by std::string and provides list box of available materials
	public value struct Material_Tag sealed {};

	//! represented by std::string and provides list box of available render targets
	public value struct RenderTarget_Tag sealed {};

	//! represented by uint32_t and provides bits to selected
	//! This will send kGetChoiceLabels, where std::pair<std::string, std::vector<std::string> > is data
	public value struct Flags_Tag sealed {};

	//! choice from pre-defined string
	//! This will send kGetChoiceLabels, where std::pair<std::string, std::vector<std::string> > is data
	public value struct Choice_Tag sealed {};

	//! This is used to be able provide custom label(s)
	//! This will send kGetChoiceLabels, where std::pair<std::string, std::vector<std::string> > is data
	public value struct CustomLabels_Tag sealed {};

	//! This is used to show enum values by it's name
	//! This will send kGetChoiceLabels, where std::pair<std::string, std::vector<std::string> > is data
	//! .first - name of property (read only)
	//! .second - recipient of this message fills in names of enums
	public value struct Enum_Tag sealed {};

	// this is temporary until I migrate math classes over to C# or find other sdk
	public value struct Vector2_Tag sealed {};
	public value struct Vector3_Tag sealed {};
	public value struct Vector4_Tag sealed {};
	public value struct Quaternion_Tag sealed {};

	public ref class Property abstract
	{
	public:
		//! This will return unique id for this property within property set
		virtual UInt32 GetPropId() = 0;

		//! return name of this property.
		virtual String^ GetName() = 0;

		//! Return current value of this property in a form of std::string. Conversion might occur from type T to std::string
		virtual String^ GetValue() = 0;

		//! Return true if this property is of type
		//! Used for determining what kind of UI editor it needs
		virtual bool IsPropType(Type^ type) = 0;

		//! Return string representing options for Gui editor control
		//! to manipulate editing and displaying options
		virtual String^ GetPropTypeOptions() = 0;

		//! Set property value to newValue. Conversion might occur from std::string to type T
		//! \note It will silently ignore newValue if this is ReadOnly property.
		virtual void SetValue(String^ newValue) = 0;

		//! Return true if this property is of value
		//! Used for actually determining what kid of value type it is
		virtual bool IsPropValue(Type^ type) = 0;

		//! Is this property read only, then return TRUE, otherwise FALSE
		virtual bool IsReadOnly() = 0;

		/*
		//! Get what kind of type this value exposes
		//! This and GetPropValue() methods are used to drive editing capabilities
		//! for this property. IOW, it allows for this property to expose customized
		//! edit controls to editor.
		//!
		//! \note Type, can be any built-ins or one of 'struct *_Tag {};' structure's
		virtual const std::type_info& GetPropType() const = 0;

		//! Get what kind of value this property is
		virtual const std::type_info& GetPropValue() const = 0;
		*/

		//! Return instance id of owner of this property
		virtual InstanceId GetInstanceId() = 0;

		/*
		//! This is primary used by editor to be notified when there is a change to property value.
		//! \note Do not call this directly, it is used by PropertySet to implement that functionality
		//! \param propertyChangedCallback new callback to use
		//! \return previous callback used or PropertyChangedCallback_t() if none
		virtual PropertyChangedCallback_t SetChangeCallback(PropertyChangedCallback_t propertyChangedCallback) = 0;

		IProperty& operator =(const char *pvalue) { return (*this) = std::string(pvalue);}

		IProperty& operator =(const std::string& value);
		operator std::string() const;

		//!@{
		//! We provide certain set of values which can be use to set this IProperty
		//! to that value.
		//! For comments about overriding virtual get and set methods lookup
		//! YAGET_PROP_ASSIGNMENT macro
		YAGET_PROP_ASSIGNMENT(bool);
		YAGET_PROP_ASSIGNMENT(float);
		YAGET_PROP_ASSIGNMENT(int32_t);
		YAGET_PROP_ASSIGNMENT(uint32_t);
		YAGET_PROP_ASSIGNMENT(uint64_t);
		YAGET_PROP_ASSIGNMENT(Vector2);
		YAGET_PROP_ASSIGNMENT(Vector3);
		YAGET_PROP_ASSIGNMENT(Vector4);
		YAGET_PROP_ASSIGNMENT(Quaternion);
		YAGET_PROP_ASSIGNMENT(Matrix33);
		YAGET_PROP_ASSIGNMENT(Matrix44);
		YAGET_PROP_ASSIGNMENT(std::vector<component::ObjectId>);
		YAGET_PROP_ASSIGNMENT(std::vector<std::string>);
		//!@}


		//!@{
		//! Helper method for checking type and value
		template <typename T>
		bool IsType() const {return (typeid(T) == GetPropType()) ? true : false;}
		template <typename T>
		bool IsValue() const {return (typeid(T) == GetPropValue()) ? true : false;}
		//!@}
		*/
		Dictionary<String^, String^>^ FormatTypeOptions()
		{
			Dictionary<String^, String^>^ options = gcnew Dictionary<String^, String^>();
			String^ optionStr = GetPropTypeOptions();
			optionStr = optionStr->Replace('{', ' ');
			optionStr = optionStr->Replace('}', ' ');

			array<Char>^ cmdSep = {';'};
			array<String^>^ commands = optionStr->Split(cmdSep);
			for each (String^ command in commands)
			{
				array<Char>^ kvSep = {'='};
				array<String^>^ keyValue = command->Split(kvSep);
				if (keyValue->Length > 1)
				{
					try
					{
						options->Add(keyValue[0]->Trim(), keyValue[1]->Trim());
					}
					catch (ArgumentNullException^ ex)
					{
						// just skip this double add option
						log::warning("Type Option: " + command + " has invalid key-value pair in this Property: " + GetName() + " for InstanceId: " + GetInstanceId().ToString() + ". Error: " + ex->ToString());
						continue;
					}
					catch (ArgumentException^ ex)
					{
						// just skip this double add option
						log::warning("Type Option: " + command + " is already present in this Property: " + GetName() + " for InstanceId: " + GetInstanceId().ToString() + ". Error: " + ex->ToString());
						continue;
					}
				}
			}

			return options;
		}
	};

	public delegate void NotifySlotDelegate(Property^ changedProp);

	//----------------------------------------------------------------------------
	/*!
	For now, this class only purpose is to provide access to
	editor panels for component data
	This class provides collection of Properties
	*/
	public ref class PropertySet abstract
	{
	public:
		//! Is this property set is valid, or just maybe placeholder
		virtual bool IsValid() = 0;

		//! Get group name for this set of properties
		virtual String^ GetGroupName() = 0;

		//! Returns first property or null if none exist. Needs to be called first before using GetNextProperty(...)
		virtual Property^ GetFirstProperty() = 0;

		//! Returns next iterated property, or NULL if no more. GetFirstProperty(...) must be called before using this.
		virtual Property^ GetNextProperty() = 0;

		//! Find property with specific name and return it, or NULL if does not exists
		virtual Property^ FindProperty(String^ propName) = 0;

		//! Set value on specific property. Return TRUE if was set, otherwise FALSE (property did not exist)
		virtual bool SetProperty(String^ propName, String^ newValue) = 0;

		//! get instance id of owner of this property set
		virtual InstanceId GetInstanceId() = 0;

		//! This allows us to provide callback which will be triggered
		//! everytime there is change in value of any property.
		//! This is useful in editor to be able to display changing values.
		//! Signature of callback slot for global property change event
		//typedef boost::signal<void (const IProperty * /*pProperty*/)> NotifySignal_t;

		//! This will add new notify slot
		//!
		//! \param slot callback function to be called when any of properties change
		//! \param userSlotName user define name. This name must be unique across all callbacks connected to this property set
		//! \note if in debug mode it will assert and in release it will silently replace previous callback.
		//! \code boost::bind(&Foo::onSignal, this, _1) \endcode
		virtual void AddNotifySlot(NotifySlotDelegate^ slot) = 0;

		//! This will remove existing notify slot
		//!
		//! \param userSlotName slot to remove
		virtual void RemoveNotifySlot() = 0;

		//virtual bool operator ==(const IPropertySet& source) const = 0;
		//virtual bool operator !=(const IPropertySet& source) const = 0;
		//virtual IProperty& operator[](const std::string& propName) = 0;
		//virtual const IProperty& operator[](const std::string& propName) const = 0;
	};


	private ref class PropertyBridge : Property
	{
	public:
		PropertyBridge(InstanceId iId, eg::IProperty& prop)
		: mInstanceId(iId)
		, mProp(prop)
		{
		}

		//! This will return unique id for this property within property set
		virtual UInt32 GetPropId() override
		{
			return mProp.GetPropId();
		}

		//! return name of this property.
		virtual String^ GetName() override
		{
			return stos(mProp.GetName());
		}

		virtual String^ GetValue() override
		{
			return stos(mProp.GetValue());
		}

		virtual String^ GetPropTypeOptions() override
		{
			return stos(mProp.GetPropTypeOptions());
		}
                                                 
		virtual void SetValue(String^ newValue) override
		{
			mProp.SetValue(stos(newValue));
		}

		virtual bool IsPropType(Type^ type) override
		{
			if (type == bool::typeid && mProp.IsType<bool>()
				|| type == Int32::typeid && mProp.IsType<int32_t>()
				|| type == UInt32::typeid && mProp.IsType<uint32_t>()
				|| type == Single::typeid && mProp.IsType<float>()
				|| type == String::typeid && mProp.IsType<std::string>()
				|| type == InstanceId::typeid && mProp.IsType<eg::component::InstanceId>()
				|| type == Color3_Tag::typeid && mProp.IsType<eg::Color3_Tag>()
				|| type == Color4_Tag::typeid && mProp.IsType<eg::Color4_Tag>()
				|| type == File_Tag::typeid && mProp.IsType<eg::File_Tag>()
				|| type == Flags_Tag::typeid && mProp.IsType<eg::Flags_Tag>()
				|| type == Vector2_Tag::typeid && mProp.IsType<eg::Vector2>()
				|| type == Vector3_Tag::typeid && mProp.IsType<eg::Vector3>()
				|| type == Vector4_Tag::typeid && mProp.IsType<eg::Vector4>()
				|| type == Quaternion_Tag::typeid && mProp.IsType<eg::Quaternion>()
				)
			{
				return true;
			}

			return false;
		}

		virtual bool IsPropValue(Type^ type) override
		{
			if (type == bool::typeid && mProp.IsValue<bool>()
				|| type == Int32::typeid && mProp.IsValue<int32_t>()
				|| type == UInt32::typeid && mProp.IsValue<uint32_t>()
				|| type == Single::typeid && mProp.IsValue<float>()
				|| type == String::typeid && mProp.IsValue<std::string>()
				|| type == InstanceId::typeid && mProp.IsValue<eg::component::InstanceId>()
				|| type == Color3_Tag::typeid && mProp.IsValue<eg::Color3_Tag>()
				|| type == Color4_Tag::typeid && mProp.IsValue<eg::Color4_Tag>()
				|| type == File_Tag::typeid && mProp.IsValue<eg::File_Tag>()
				|| type == Flags_Tag::typeid && mProp.IsValue<eg::Flags_Tag>()
				)
			{
				return true;
			}

			return false;
		}

		virtual bool IsReadOnly() override
		{
			return mProp.IsReadOnly();
		}

		virtual InstanceId GetInstanceId() override
		{
			return mInstanceId;
		}

	private:
		InstanceId mInstanceId;
		eg::IProperty& mProp;
	};


	namespace local
	{
		delegate void PropertyNotifyDelegate(const eg::IProperty * /*pProperty*/);
		typedef void (__stdcall *PropertyNotifyNative)(const eg::IProperty * /*pProperty*/);
	} // namespace local



	private ref class PropertySetBridge : PropertySet
	{
	public:
		//! Is this property set is valid, or just maybe placeholder
		PropertySetBridge(InstanceId iId, eg::IPropertySet& propSet)
		: mInstanceId(iId)
		, mPropSet(propSet)
		, mNotifyPropertySlot(nullptr)
		, mSlotId(0)
		{
		}

		virtual bool IsValid() override
		{
			return mInstanceId.IsValid();
		}

		virtual String^ GetGroupName() override
		{
			return stos(mPropSet.GetGroupName());
		}

		virtual Property^ GetFirstProperty() override
		{
			if (eg::IProperty *pProp = mPropSet.GetFirstProperty())
			{
				Property^ prop = gcnew PropertyBridge(mInstanceId, *pProp);
				return prop;
			}

			return nullptr;
		}

		virtual Property^ GetNextProperty() override
		{
			if (eg::IProperty *pProp = mPropSet.GetNextProperty())
			{
				Property^ prop = gcnew PropertyBridge(mInstanceId, *pProp);
				return prop;
			}

			return nullptr;
		}

		virtual Property^ FindProperty(String^ propName) override
		{
			if (eg::IProperty *pProp = mPropSet.FindProperty(stos(propName)))
			{
				Property^ prop = gcnew PropertyBridge(mInstanceId, *pProp);
				return prop;
			}

			return nullptr;
		}

		virtual bool SetProperty(String^ propName, String^ newValue) override
		{
			return mPropSet.SetProperty(stos(propName), stos(newValue));
		}

		virtual InstanceId GetInstanceId() override
		{
			return mInstanceId;
		}

		//! This allows us to provide callback which will be triggered
		//! everytime there is change in value of any property.
		//! This is useful in editor to be able to display changing values.
		//! Signature of callback slot for global property change event
		//typedef boost::signal<void (const IProperty * /*pProperty*/)> NotifySignal_t;

		//! This will add new notify slot
		//!
		//! \param slot callback function to be called when any of properties change
		//! \param userSlotName user define name. This name must be unique across all callbacks connected to this property set
		//! \note if in debug mode it will assert and in release it will silently replace previous callback.
		//! \code boost::bind(&Foo::onSignal, this, _1) \endcode
		virtual void AddNotifySlot(NotifySlotDelegate^ slot) override
		{
			if (mNotifyPropertySlot != nullptr)
			{
				RemoveNotifySlot();
			}

			mNotifyPropertySlot = slot;
			mPropertyNotifyDelegate = gcnew local::PropertyNotifyDelegate(this, &PropertySetBridge::onNativeCallback);
			local::PropertyNotifyNative nativePtr = (local::PropertyNotifyNative)System::Runtime::InteropServices::Marshal::GetFunctionPointerForDelegate(mPropertyNotifyDelegate).ToPointer();

			mSlotId = sNextSlotId++;
			String^ slotName = System::Convert::ToString(mSlotId);
			mPropSet.AddNotifySlot(nativePtr, stos(slotName));
		}

		//! This will remove existing notify slot
		//!
		//! \param userSlotName slot to remove
		virtual void RemoveNotifySlot() override
		{
			String^ slotName = System::Convert::ToString(mSlotId);
			mPropSet.RemoveNotifySlot(stos(slotName));
			mNotifyPropertySlot = nullptr;
			mPropertyNotifyDelegate = nullptr;
		}

		void onNativeCallback(const eg::IProperty *pProperty)
		{
			if (mNotifyPropertySlot == nullptr)
			{
				wxLogError("Instance '%s' received callback from Property '%s' but there is no NotifyPropertySlot attached.", stos(mInstanceId.ToString()), pProperty->GetName());
				return;
			}
			Property^ prop = FindProperty(stos(pProperty->GetName()));
			mNotifyPropertySlot(prop);
		}
												

		//virtual bool operator ==(const IPropertySet& source) const = 0;
		//virtual bool operator !=(const IPropertySet& source) const = 0;
		//virtual IProperty& operator[](const std::string& propName) = 0;
		//virtual const IProperty& operator[](const std::string& propName) const = 0;

	private:
		static UInt32 sNextSlotId = 100;
		UInt32 mSlotId ;
		InstanceId mInstanceId;
		eg::IPropertySet& mPropSet;
		NotifySlotDelegate^ mNotifyPropertySlot;
		local::PropertyNotifyDelegate^ mPropertyNotifyDelegate;
	};


	public ref class prop sealed
	{
	public:
		static PropertySet^ cast(InstanceId iId)
		{
			using namespace eg;

			eg::component::InstanceId instanceId(iId.oId, iId.cId);
			if (IPropertySet *pPropSet = eg::casters::property_set_cast(instanceId))
			{
				PropertySet^ propertySetBridge = gcnew PropertySetBridge(iId, *pPropSet);
				return propertySetBridge;
			}

			return nullptr;
		}

		static Property^ cast(InstanceId iId, String^ name)
		{
			PropertySet^ propSet = prop::cast(iId);
			if (propSet != nullptr)
			{
				return propSet->FindProperty(name);
			}

			return nullptr;
		}

		static Property^ cast(UInt64 oId, String^ type, String^ name)
		{
			eg::component::ComponentId cId(stos(type).c_str());
			return cast(InstanceId(oId, cId.GetValue()), name);
		}
	};


	//! Facade for ObjectManager
	public ref class om sealed
	{
	public:
		//! Return sorted list of all valid object at this point in time
		static property List<InstanceId>^ Objects
		{
			List<InstanceId>^ get()
			{
				List<InstanceId>^ objectList = gcnew List<InstanceId>();
				eg::IObjectManager& mgr = eg::registrate::ref_cast<eg::IObjectManager>("IObjectManager");
				const std::vector<eg::component::ObjectId>& objects = mgr.EnumObjects();
				for (std::vector<eg::component::ObjectId>::const_iterator it = objects.begin(); it != objects.end(); ++it)
				{
					UInt64 oOd = *it;
					eg::ObjectListCallback componentListCallback(oOd);
					std::sort(componentListCallback.ComponentList.begin(), componentListCallback.ComponentList.end());
					for (eg::ObjectListCallback::List::const_iterator itc = componentListCallback.ComponentList.begin(); itc != componentListCallback.ComponentList.end(); ++itc)
					{
						const eg::IComponent *pComponent = *itc;
						UInt64 cId = pComponent->GetComponentTypeId();

						InstanceId iId(oOd, cId);
						objectList->Add(iId);
					}
				}

				//objectList->Sort();
				return objectList;
			}
		}

		//! Return list of available valid component for instance creation
		static property List<UInt64>^ Types
		{
			List<UInt64>^ get()
			{
				List<UInt64>^ objectList = gcnew List<UInt64>();
				eg::IObjectManager& mgr = eg::registrate::ref_cast<eg::IObjectManager>("IObjectManager");
				const std::set<eg::component::ComponentId>& compTypes = mgr.GetComponentTypes();
				for (std::set<eg::component::ComponentId>::const_iterator it = compTypes.begin(); it != compTypes.end(); ++it)
				{
					objectList->Add(*it);
				}

				return objectList;
			}
		}

		//! Return next valid burnable id (only valid for duration and unique to this process)
		static UInt64 getBurnableId()
		{
			return eg::idspace::get_burnable();
		}

		//! Return next valid persistent id. This needs to be used for any object
		//! which is persistent
		//! \note should we reserve high bit for this range?
		static UInt64 getPersistentId()
		{
			return eg::idspace::get_persistent();
		}

		//! create new instance (Component) 
		//! \param iId oId is unique id and cId is component type
		//! \param name name for this object. It is disregarded when 
		//! object already exists.
		//! \return true instance created, false did not
		// \note should we throw exception here?
		static bool newc(InstanceId iId, String^ name)
		{
			eg::IObjectManager& mgr = eg::registrate::ref_cast<eg::IObjectManager>("IObjectManager");
			return mgr.CreateComponent(eg::component::make(iId.oId, iId.cId), stos(name)) != 0;
		}

		//! create new instance (Component) 
		//! \param oId unique id for this instance
		//! \param type Component type
		//! \param name name for this object
		//! \return true instance created, false did not
		// \note should we throw exception here?
		static bool newc(UInt64 oId, String^ type, String^ name)
		{
			eg::component::ComponentId cId(stos(type).c_str());
			eg::IObjectManager& mgr = eg::registrate::ref_cast<eg::IObjectManager>("IObjectManager");
			return mgr.CreateComponent(eg::component::make(oId, cId), stos(name)) != 0;
		}

		//! create new instance (Component) 
		//! \param type Component type
		//! \param name name for this object
		//! \return new valid InstanceId. It will use burnable id for it
		static InstanceId newc(String^ type, String^ name)
		{
			eg::component::ObjectId oId(getBurnableId());
			eg::component::ComponentId cId(stos(type).c_str());
			eg::IObjectManager& mgr = eg::registrate::ref_cast<eg::IObjectManager>("IObjectManager");
			if (mgr.CreateComponent(eg::component::InstanceId(oId, cId), stos(name)) != 0)
			{
				InstanceId instanceId;
				instanceId.oId = oId;
				instanceId.cId = cId;
				return instanceId;
			}

			return InstanceId(0, 0);
		}

		//! Create new object composed of on or more components listed
		//! types, using name as an Object name
		//! \return Unique object id
		//! \note for now this is burnable
		static UInt64 newc(array<String^>^ types, String^ name)
		{
			UInt64 oId = getBurnableId();
			for each (String^ type in types)
			{
				if (!newc(oId, type, name))
				{
					wxLogError("Could not create Component [%s] for Object [%s].", stos(type), stos(name));
				}
			}

			return oId;
		}


		//! Delete specific Component instance
		//! \param oId id of component instance to delete
		static void deletec(InstanceId iId)
		{
			eg::IObjectManager& mgr = eg::registrate::ref_cast<eg::IObjectManager>("IObjectManager");
			if (iId.cId == 0)
			{
				mgr.DestroyInstance(eg::component::ObjectId(iId.oId));
			}
			else if (eg::IComponent *pComp = mgr.FindComponentInstance(eg::component::make(iId.oId, iId.cId)))
			{
				mgr.DestroyComponent(pComp);
			}
		}
		static void deletec(UInt64 oId)
		{
			deletec(InstanceId(oId, 0));
		}
	};

	//! Facade for VirtualFileSystem object
	public ref class vfs
	{
	public:
		static List<String^>^ GetFileList(String^ filter)
		{
			List<String^>^ fileList = gcnew List<String^>();

			std::vector<std::string> stlFileList = managed::vf::GetFileList(stos(filter));
			for (std::vector<std::string>::const_iterator it = stlFileList.begin(); it != stlFileList.end(); ++it)
			{
				fileList->Add(stos(*it));
			}

			return fileList;
		}

		static String^ GetFqn(String^ fileName)
		{
			return stos(managed::vf::GetFqn(stos(fileName)));
		}
	};

	//! Facade for IScript object
	public ref class script
	{
	public:
		static bool RegisterFunction(String^ functionSignature)
		{
			if (eg::IScript *pScript = eg::registrate::p_cast<eg::IScript>("IScript"))
			{
				return pScript->RegisterFunction(stos(functionSignature));
			}

			return false;
		}
	};


	//! This provides enumeration of all or any filtered objects/components
	//! and event for added, removed components/objects messages
	public ref class Enumerator
	{
	public:
		//! Triggered when any kind of component event happens, new delete
		//! type will represent message id and iId 
		//! which component was affected by operation
		delegate void eventComponent(UInt32 type, Object^ value);

		Enumerator()
		: mNewObjectEvent(gcnew yaget::Signal(gcnew yaget::DispatcherDelegate(this, &Enumerator::onComponent), eg::mtype::kComponentNew))
		, mDeleteObjectEvent(gcnew yaget::Signal(gcnew yaget::DispatcherDelegate(this, &Enumerator::onComponent), eg::mtype::kComponentDelete))
		, mIsDisposed(false)
		, m_Objects(om::Objects)
		{
		}

		~Enumerator()
		{
			if (!mIsDisposed) 
			{ 
				// release once only
				// clean up managed resources here.
				this->!Enumerator();
			}
			mIsDisposed = true;
		}

		//! checks to see if this instance already exist in this collection
		bool Has(InstanceId iId)
		{
			for each (InstanceId localId in m_Objects)
			{
				if (localId == iId)
				{
					return true;
				}
			}

			return false;
		}

        bool Has(UInt64 oId)
        {
            for each (InstanceId localId in m_Objects)
            {
                if (localId.oId == oId)
                {
                    return true;
                }
            }

            return false;
        }

		//! number of instances in this enumerator
		property int Count
		{
			int get()
			{
				return m_Objects->Count;
			}
		}

		property InstanceId default[int]
		{
			InstanceId get(int index)
			{
				if (index < 0 || index >= m_Objects->Count)
				{
					// handle bad index
					return InstanceId();
				}

				return m_Objects[index];
			}
		}

		void connect(eventComponent^ objectCallback)
		{
			mObjectCallback += objectCallback;

			// you only need to notify the new one
			for each (InstanceId iId in m_Objects)
			{
				objectCallback(eg::mtype::kComponentNew, iId);
			}
		}

		void disconnect(eventComponent^ objectCallback)
		{
			mObjectCallback -= objectCallback;
		}

		!Enumerator() // maps to finalizer
		{
			if (!mIsDisposed) 
			{
				// Clean up unmanaged resources here.
				delete mNewObjectEvent;
				mNewObjectEvent = nullptr;
				delete mDeleteObjectEvent;
				mDeleteObjectEvent = nullptr;
			}
		}

	private:
		yaget::Signal^ mNewObjectEvent;
		yaget::Signal^ mDeleteObjectEvent;
		event eventComponent^ mObjectCallback;

		void onComponent(UInt32 type, Object^ value)
		{
			InstanceId iId;
			// we need to do our housekeeping here
			// before we pass this event along
			switch(type)
			{
			case eg::mtype::kComponentNew:
				{
					iId = (InstanceId)value;
					m_Objects->Add(iId);
				}
				break;

			case eg::mtype::kComponentDelete:
				{
					iId = (InstanceId)value;
					m_Objects->Remove(iId);
				}
				break;

			default:
				// \todo we need to throw exception
				// "Message [type] is not supported in Enumerator!"
				break;
			}

			mObjectCallback(type, iId);
		}

		bool mIsDisposed;
		List<InstanceId>^ m_Objects;
	};


	//! Object Name
	String^ InstanceId::OName()
	{
		eg::component::InstanceId iId = eg::component::make(oId, cId);
		return stos(eg::component::objectName(iId));
	}

	//! Component Name
	String^ InstanceId::CName()
	{
		eg::component::InstanceId iId = eg::component::make(oId, cId);
		return stos(eg::component::componentName(iId));
	}

} // namespace component

//! Basic render methods exposed
public ref class Render
{
public:
	static int CreateViewToken(IntPtr winh)
	{
		using namespace eg;
		if (IRenderer *pRenderer = registrate::p_cast<IRenderer>("Renderer"))
		{
			void *window = (void *)winh.ToInt32();
			IRenderer::ViewToken viewToken = pRenderer->CreateViewToken(window);
			return viewToken.index;
		}

		return -1;
	}

	//! Render current scene using iId as an active camera if its valid
	//! otherwise use regular loop for rendering
	static void RenderView(yaget::component::InstanceId iId, int token, int w, int h)
	{
		//using namespace eg;
		if (token == -1)
		{
			// \note do we need to print log here?
			return;
		}

		eg::IRenderer::ViewToken viewToken;
		viewToken.index = token;
		if (eg::IRenderer *pRenderer = eg::registrate::p_cast<eg::IRenderer>("Renderer"))
		{
			if (pRenderer->UseViewToken(viewToken))
			{
				wxRect viewport(0, 0, w, h);
				if (iId.IsValid())
				{
					eg::component::InstanceId viewId(iId.oId, iId.cId);
					if (eg::IComponentView *pView = eg::component::get<eg::IComponentView>(viewId))
					{
						pView->Render(*pRenderer, 0, 0.16f, viewport);
					}
				}

				pRenderer->Flip();
			}
		}
	}
};

//-------------------------------------------------------------------------------------
//! Provide same log facility as we have it in C++ using wxWidgets stuff
void log::trace(System::String^ text)
{
	//std::string newText = stos(text);
	//wxLogTrace(TR_EDITOR, "%s", newText);
}

void log::message(System::String^ text)
{
	//std::string newText = stos(text);
	//wxLogMessage("%s", newText);
}

void log::warning(System::String^ text)
{
	//std::string newText = stos(text);
	//wxLogWarning("%s", newText);
}

void log::error(System::String^ text)
{
	//std::string newText = stos(text);
	//wxLogError("%s", newText);
}


} // namespace yaget

#endif // 0
