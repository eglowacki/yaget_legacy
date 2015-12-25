///////////////////////////////////////////////////////////////////////
// MessageEnums.h
//
//  Copyright 10/16/2005 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//
//
//  #include "MessageEnums.h"
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef MESSAGE_ENUMS_H
#define MESSAGE_ENUMS_H

#include "MessageInterface.h"

namespace eg
{

    /*!
    Message types which get send to components
    */
    namespace mtype
    {
        /*!
        This entire Property<...> are used mostly to pass data between
        editor and components and vice-versa.

        PropertyPair_t
            first - Property Name read only
            second - Property Value read/write

        PropertyInfo_t
            first - component unique instance id read only
            second - PropertyPair_t
        */
        typedef std::pair<std::string, std::string> PropertyPair_t;
        typedef std::pair<component::InstanceId, PropertyPair_t> PropertyInfo_t;

        inline bool IsInstance(const PropertyInfo_t& propertyInfo, component::InstanceId instanceId)
        {
            return propertyInfo.first == instanceId;
        }

        inline PropertyPair_t& PropertyPair(PropertyInfo_t& propertyInfo)
        {
            return propertyInfo.second;
        }

        inline const std::string& PropertyName(const PropertyInfo_t& propertyInfo)
        {
            return propertyInfo.second.first;
        }

        inline  std::string& PropertyValue(PropertyInfo_t& propertyInfo)
        {
            return propertyInfo.second.second;
        }

        inline PropertyInfo_t PropertyNew(component::InstanceId instanceId, const std::string& name)
        {
             return PropertyInfo_t(instanceId, PropertyPair_t(name, ""));
        }

        //! Update component internal data
        static const guid_t kUpdate = 0x506f40e6;
        //! Afetr all the objects are created.
        static const guid_t kAllObjectsCreated = 0x706f410e;
        //! This is used by editor to get all the controls needed to manipulate and display data
        static const guid_t kGetEditorProperties = 0xb0de8f28;

        /*!
        this uses PropertyInfo_t *
        */
        static const guid_t kGetResourcePath = 0x71ea7478;

        /*!
        This will return std::string with qualified filter file
        in aPropertyPair_t second
        this uses PropertyInfo_t *
        */
        static const guid_t kGetResourceFilter = 0x71ea747c;

        //typedef std::pair<std::string, std::vector<std::string> > ChoiceLabels

    } // namespace mtype

} // namespace eg

#endif //MESSAGE_ENUMS_H