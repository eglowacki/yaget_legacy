//////////////////////////////////////////////////////////////////////
// ObjectManager.h
//
//  Copyright 12/29/2006 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//      Managed glue for IObjectManager between
//
//
//  #include "ObjectManager.h"
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef MANAGED_OBJECT_MANAGER_H
#define MANAGED_OBJECT_MANAGER_H
#pragma once

#include "Component.h"
#include "IComponent.h"
#include "IPlugin.h"


namespace eg
{
    namespace managed
    {
        //-------------------------------------------------------------------------------------
        public interface class EnumComponentCallback
        {
            //! This is called for evere enumerated component
            virtual bool OnComponent(Component^ pComponent);

            //! Called before first component is enumerated.
            //! If return FALSE, then do not enumerate any components.
            //! OnEnd() is always called.
            virtual bool OnBegin();
            //! Called after all component enumeration is done
            virtual void OnEnd();
        };


        //-------------------------------------------------------------------------------------
        /*!
        Interface for C# to use ObjectManager
        */
        public ref class ObjectManager
        {
        public:
            //! This will return component with cId belonging to object with oId, or nULL if it does not exist
            Component^ FindComponentInstance(ObjectId oId, Hash cId);
            Component^ FindComponentInstance(ComponentInstanceId);
            bool IsObjectExists(ObjectId oId);
            bool IsObjectExists(String^ name);
            bool CloneInstance(ObjectId sourceId, Hash cId, String^ targetName);

            //! Create component using specif name and load data from stream of the same name if one exist
            Component^ CreateComponent(String^ name, Hash cId);
            //! Create component based only on type and return interface to it
            Component^ CreateComponent(ObjectId oId, Hash cId);
            //! Destroy and remove this component from ObjectManager
            void DestroyComponent(Component^ pComp);

            void EnumObjectsByInterface(guid_t iid, EnumComponentCallback^ componentCallback);
            void EnumObjectsByType(Hash cid, EnumComponentCallback^ componentCallback);
            void EnumObjectsByObject(ObjectId oid, EnumComponentCallback^ componentCallback);
            void EnumObjectsByAll(EnumComponentCallback^ componentCallback);

            //@{
            //! Helper methods to reverse hash id and object id into text names
            String^ GetObjectName(ObjectId oId);
            String^ GetComponentName(Hash cId);
            //@}

            //@{
            //! Helper methods to reverse object and component and into object an hash id
            static ObjectId GetObjectId(String^ objectName);
            static Hash GetComponentId(String^ componentName);
            //@}

            //! Helper method to return PropertySet for specific component
            static PropertySet^ property_cast(ComponentInstanceId id)
            {
                if (Component^ pComponent = Get->FindComponentInstance(id))
                {
                    return pComponent->GetPropertyData();
                }
                return nullptr;
            }

			//! Helper method to return Property for specific component by name
			static Property^ property_cast(ComponentInstanceId id, String^ propName)
			{
				PropertySet^ propSet = ObjectManager::property_cast(id);
				if (propSet != nullptr)
				{
					Property^ prop = propSet->FindProperty(propName);
					return prop;
				}

				return nullptr;
			}

            //! Accessor
            static property ObjectManager^ Get
            {
                ObjectManager^ get()
                {
                    if (sInstance == nullptr)
                    {
                        sInstance = gcnew ObjectManager();
                    }
                    return sInstance;
                }
            }


        private:
            //! Our one and only instance (Factory pattern)
            static ObjectManager^ sInstance = nullptr;
        };




        // Internal namespace for this file
        namespace internal
        {
            ref class PropertyImpl : public Property
            {
            public:
                PropertyImpl(eg::IProperty *pProperty) : mpProperty(pProperty)
                {
                }

                virtual String^ GetName()
                {
                    if (mpProperty->GetName())
                    {
                        return SToS(mpProperty->GetName()->c_str());
                    }

                    return nullptr;
                }

                virtual String^ GetValue()
                {
                    return SToS(mpProperty->GetValue().c_str());
                }

                virtual void SetValue(String^ newValue)
                {
                    mpProperty->SetValue(SToS(newValue));
                }

                virtual bool IsReadOnly()
                {
                    return mpProperty->IsReadOnly();
                }

                virtual ComponentInstanceId GetInstanceId()
                {
                    ComponentInstanceId instId;
                    instId.objectId = mpProperty->GetInstanceId().first;
                    instId.typeId = mpProperty->GetInstanceId().second;

                    return instId;
                }

                virtual Type^ GetPropType();
                virtual bool IsPropType(Type^ type) {return GetPropType() == type;}

                virtual Type^ GetPropValue();
                virtual bool IsPropValue(Type^ value) {return GetPropValue() == value;}

                virtual String^ GetConverterName();

            private:
                eg::IProperty *mpProperty;
            };

            ref class PropertySetImpl : public PropertySet
            {
            public:
                PropertySetImpl(eg::IPropertySet& propSet) : mPropertySet(propSet)
                {
                }

                virtual bool IsValid()
                {
                    return mPropertySet.IsValid();
                }

                virtual String^ GetGroupName()
                {
                    if (mPropertySet.GetGroupName())
                    {
                        return SToS(mPropertySet.GetGroupName()->c_str());
                    }
                    return nullptr;
                }

                virtual Property^ GetFirstProperty()
                {
                    if (eg::IProperty *pProp = mPropertySet.GetFirstProperty())
                    {
                        return gcnew PropertyImpl(pProp);
                    }

                    return nullptr;
                }

                virtual Property^ GetNextProperty()
                {
                    if (eg::IProperty *pProp = mPropertySet.GetNextProperty())
                    {
                        return gcnew PropertyImpl(pProp);
                    }

                    return nullptr;
                }

                virtual Property^ FindProperty(String^ propName)
                {
                    if (eg::IProperty *pProp = mPropertySet.FindProperty(SToS(propName)))
                    {
                        return gcnew PropertyImpl(pProp);
                    }

                    return nullptr;
                }

                virtual bool SetProperty(String^ propName, String^ newValue)
                {
                    return mPropertySet.SetProperty(SToS(propName), SToS(newValue));
                }

                virtual ComponentInstanceId GetInstanceId()
                {
                    ComponentInstanceId instId;
                    instId.objectId = mPropertySet.GetInstanceId().first;
                    instId.typeId = mPropertySet.GetInstanceId().second;

                    return instId;
                }

            private:
                eg::IPropertySet& mPropertySet;
            };

            //-------------------------------------------------------------------------------------
            //! Internal class to just hold IComponent pointer
            ref class ComponentImpl : public Component
            {
            public:
                ComponentImpl(eg::IComponent *pComponent) :
                    mpComponent(pComponent),
                    mPropSet(nullptr)
                {
                }

                virtual Hash GetComponentTypeId()
                {
                    eg::Hash cId = mpComponent->GetComponentTypeId();;
                    return cId.GetValue();
                }

                virtual ObjectId GetObjectId()
                {
                    eg::ObjectId oId = mpComponent->GetObjectId();;
                    return oId.GetValue();
                }

                virtual PropertySet^ GetPropertyData()
                {
                    if (mPropSet == nullptr)
                    {
                        mPropSet = gcnew PropertySetImpl(mpComponent->GetPropertyData());
                    }

                    return mPropSet;
                }

                virtual void HandleMessage(Message^ msg)
                {
                    eg::Message cMsg = Marshal(msg);
                    mpComponent->HandleMessage(cMsg);
                    Message^ tempMsg = Marshal(cMsg);
                    msg->mAnyData = tempMsg->mAnyData;
                }


                eg::IComponent *GetComponent() {return mpComponent;}

            private:
                eg::IComponent *mpComponent;
                PropertySetImpl^ mPropSet;
            };

            //-------------------------------------------------------------------------------------
            //! Helper method to execute .NET callback
            inline void ExecuteCallback(const std::vector<eg::IComponent *>& list, EnumComponentCallback^ componentCallback)
            {
                if (componentCallback->OnBegin())
                {
                    for (std::vector<eg::IComponent *>::const_iterator it = list.begin(); it != list.end(); ++it)
                    {
                        Component^ component = gcnew internal::ComponentImpl(*it);
                        if (!componentCallback->OnComponent(component))
                        {
                            break;
                        }
                    }
                }
                // This is always called regardless
                componentCallback->OnEnd();
            }

            //-------------------------------------------------------------------------------------
            //! C++ structure to get requested components from ObjectManager
            struct Components : public eg::EnumComponentCallback
            {
                //! This is called for every enumerated component
                virtual bool OnComponent(IComponent *pComponent)
                {
                    mList.push_back(pComponent);
                    return true;
                }

                virtual bool OnBegin() {return true;}
                const std::vector<eg::IComponent *>& List() const {return mList;}

            private:
                std::vector<eg::IComponent *> mList;
            };

        } // namespace internal


        //-------------------------------------------------------------------------------------
        inline Component^ ObjectManager::FindComponentInstance(ObjectId oId, Hash cId)
        {
            if (eg::IComponent *pComponent = eg::GetObjectManager().FindComponentInstance(eg::ObjectId(oId), eg::Hash(cId)))
            {
                Component^ component = gcnew internal::ComponentImpl(pComponent);
                return component;
            }
            return nullptr;
        }

        inline Component^  ObjectManager::FindComponentInstance(ComponentInstanceId id)
        {
            return FindComponentInstance(id.objectId, id.typeId);
        }

        //-------------------------------------------------------------------------------------
        inline Component^ ObjectManager::CreateComponent(String^ name, Hash cId)
        {
            if (eg::IComponent *pComponent = eg::GetObjectManager().CreateComponent(SToS(name), eg::Hash(cId)))
            {
                Component^ component = gcnew internal::ComponentImpl(pComponent);
                return component;
            }
            return nullptr;
        }

        //-------------------------------------------------------------------------------------
        inline Component^ ObjectManager::CreateComponent(ObjectId oId, Hash cId)
        {
            if (eg::IComponent *pComponent = eg::GetObjectManager().CreateComponent(eg::ObjectId(oId), eg::Hash(cId)))
            {
                Component^ component = gcnew internal::ComponentImpl(pComponent);
                return component;
            }
            return nullptr;
        }

        //-------------------------------------------------------------------------------------
        inline bool ObjectManager::IsObjectExists(ObjectId oId)
        {
            return eg::GetObjectManager().IsObjectExists(eg::ObjectId(oId));
        }

        //-------------------------------------------------------------------------------------
        inline bool ObjectManager::IsObjectExists(String^ name)
        {
            return eg::GetObjectManager().IsObjectExists(SToS(name));
        }

        inline bool ObjectManager::CloneInstance(ObjectId sourceId, Hash cId, String^ targetName)
        {
            return eg::GetObjectManager().CloneInstance(eg::ObjectId(sourceId), eg::Hash(cId), SToS(targetName));
        }

        //-------------------------------------------------------------------------------------
        inline void ObjectManager::DestroyComponent(Component^ pComp)
        {
            internal::ComponentImpl^ compImpl = (internal::ComponentImpl^)pComp;
            eg::GetObjectManager().DestroyComponent(compImpl->GetComponent());
        }

        //-------------------------------------------------------------------------------------
        void ObjectManager::EnumObjectsByInterface(guid_t iid, EnumComponentCallback^ componentCallback)
        {
            internal::Components components;
            eg::GetObjectManager().EnumObjectsByInterface(iid, components);
            internal::ExecuteCallback(components.List(), componentCallback);
        }

        //-------------------------------------------------------------------------------------
        void ObjectManager::EnumObjectsByType(Hash cid, EnumComponentCallback^ componentCallback)
        {
            internal::Components components;
            eg::GetObjectManager().EnumObjectsByType(eg::Hash(cid), components);
            internal::ExecuteCallback(components.List(), componentCallback);
        }

        //-------------------------------------------------------------------------------------
        void ObjectManager::EnumObjectsByObject(ObjectId oid, EnumComponentCallback^ componentCallback)
        {
            internal::Components components;
            eg::GetObjectManager().EnumObjectsByObject(eg::ObjectId(oid), components);
            internal::ExecuteCallback(components.List(), componentCallback);
        }

        //-------------------------------------------------------------------------------------
        void ObjectManager::EnumObjectsByAll(EnumComponentCallback^ componentCallback)
        {
            internal::Components components;
            eg::GetObjectManager().EnumObjectsByAll(components);
            internal::ExecuteCallback(components.List(), componentCallback);
        }

        //-------------------------------------------------------------------------------------
        inline String^ ObjectManager::GetObjectName(ObjectId oId)
        {
            std::string name = eg::GetObjectManager().GetObjectName(eg::ObjectId(oId));
            return SToS(name);
        }

        //-------------------------------------------------------------------------------------
        inline String^ ObjectManager::GetComponentName(Hash cId)
        {
            std::string name = eg::GetObjectManager().GetComponentName(eg::Hash(cId));
            return SToS(name);
        }


    } // namespace managed


} // namespace eg

#endif // MANAGED_OBJECT_MANAGER_H

