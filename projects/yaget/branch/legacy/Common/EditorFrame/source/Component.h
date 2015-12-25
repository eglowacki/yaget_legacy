//////////////////////////////////////////////////////////////////////
// Component.h
//
//  Copyright 12/29/2006 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//      Managed glue for component
//
//
//  #include "Component.h"
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef MANAGED_COMPONENT_H
#define MANAGED_COMPONENT_H
#pragma once

#include <Base.h>


namespace eg
{
    namespace managed
    {
        interface class PropertySet;

        typedef uint32_t ObjectId;
        typedef uint32_t Hash;

        const uint32_t kInvalidHash = 0;//0xffffffff;

        /*!
        Glue for IComponent
        */
        public interface class Component
        {
        public:
            Hash GetComponentTypeId();
            ObjectId GetObjectId();
            PropertySet^ GetPropertyData();
            void HandleMessage(Message^ msg);
        };


        /*!
        Replicates std::pair<ObjectId, Hash> into C#
        */
        public value class ComponentInstanceId
        {
        public:
            ComponentInstanceId(int value)
            {
                objectId = value;
                typeId = value;
            }

            bool IsObjectValid() {return objectId != kInvalidHash;}
            bool IsTypeValid() {return typeId != kInvalidHash;}
			bool IsFolder() {return IsObjectValid() && !IsTypeValid();}

            //! Checks to see if this instance is valid, which means that
            //! both objectId and typeId must be valid
            inline bool IsInstanceIdValid() {return IsObjectValid() && IsTypeValid();}

            //! Checks to see if this object is valid, which means that at lease one
            //! of the id's (object or type) must be valid.
            bool IsValid() {return (IsObjectValid() == false && IsTypeValid() == false) != true;}

            //! Unique object id of this component
            ObjectId objectId;
            //! Type of components. If this is not valid, then this represents virtual folder (container)
            //! for displaying and editing purpose.
            Hash typeId;

            //! Return name of this instance in a format 'Name: <name>, Type: <type>'
            String^ GetInstanceName();
            //! Return type name in readable string
            String^ GetTypeName();
            //! Return object name in readable string
            String^ GetObjectName();

            //! This is helper method to send message directly to component represented by this instance id
            void SendMessage(Message^ msg);

            //! Convert object id and type id into string format 'objectId.TypeId'
            virtual String^ ToString() override;

            //! Returned initialized id based on Component
            //! if Component nullptr, then initialize to invalid values
            static ComponentInstanceId Get(Component^ component)
            {
                ComponentInstanceId instanceId;
                if (component == nullptr)
                {
                    instanceId.objectId = kInvalidHash;
                    instanceId.typeId = kInvalidHash;
                }
                else
                {
                    instanceId.objectId = component->GetObjectId();
                    instanceId.typeId = component->GetComponentTypeId();
                }
                return instanceId;
            }

            static bool operator ==(ComponentInstanceId itemOne, ComponentInstanceId itemTwo)
            {
                return itemOne.objectId ==  itemTwo.objectId && itemOne.typeId ==  itemTwo.typeId;
            }
        };

        public interface class Property
        {
        public:
            //! return name of this property.
            String^ GetName();
            //! Return current value of this property in a form of std::string. Conversion might occur from type T to std::string
            String^ GetValue();

            //! Set property value to newValue. Conversion might occur from std::string to type T
            void SetValue(String^ newValue);

            //! Is this property read only, then return TRUE, otherwise FALSE
            bool IsReadOnly();

            //! Get what kind of type this property exposes
            Type^ GetPropType();

            //! Return true if this is of type, otherwise false
            bool IsPropType(Type^ type);

            //! Get what kind of value this property exposes
            Type^ GetPropValue();

            //! Checks if actual property is of specific value
            bool IsPropValue(Type^ value);

            //! Helper method to return name of the converter used for this type and value property
            String^ GetConverterName();

            //! Return instance id of owner of this property
            ComponentInstanceId GetInstanceId();
        };

        public interface class PropertySet
        {
        public:
            //! Is this proerty set is valid, or just maybe placeholder
            bool IsValid();

            //! Get group name for this set of properties
            String^ GetGroupName();

            //! Returns first property or null if none exist. Needs to be called first before using GetNextProperty(...)
            Property^ GetFirstProperty();

            //! Returns next iterated property, or nULL if no more. GetFirstProperty(...) must be called before using this.
            Property^ GetNextProperty();

            //! Find property with specific name and return it, or NULL if does not exists
            Property^ FindProperty(String^ propName);

            //! Set value on specific property. Return TRUE if was set, otherwise FALSE (property did not exist)
            bool SetProperty(String^ propName, String^ newValue);

            //! get instance id of owner of this property set
            ComponentInstanceId GetInstanceId();
        };

    } // namespace managed

} // namespace eg

#endif // MANAGED_COMPONENT_H

