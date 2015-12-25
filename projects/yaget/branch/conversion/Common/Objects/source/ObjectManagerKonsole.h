/////////////////////////////////////////////////////////////////////
// ObjectManagerKonsole.h
//
//  Copyright 4/1/2008 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//      All Konsole Commands supported by ObjectManager
//
//
//  #include "ObjectManagerKonsole.h"
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef OBJECT_MANAGER_KONSOLE_H
#define OBJECT_MANAGER_KONSOLE_H
#pragma once

#include "ObjectsBase.h"
 #include "ObjectID.h"
#include <boost/tuple/tuple.hpp>

namespace eg
{
    namespace config
    {
        //! Usage: new <component_type> <object_id> [object_name]
        //!        new Entity 0xA Warrior
        //! It will return InstanceId Warrior
        typedef boost::tuple<component::InstanceId, std::string> NewComponent_t;

        //! This will parse strings into appropriate types and return NewComponent_t
        //!
        //! \param componentType Type of component
        //! \param objectId Object id which this component will be assigned
        //! \param objectName Optional object name
        //!
        //! \return NewComponent_t appropriate types based on passed paramters
        NewComponent_t ParseNewComponent(const std::string& componentType, const std::string& objectId, const std::string& objectName);

        //! Resolve object Id for objectName
        //!
        //! \param objectName Object name to find Id for it.
        //!                   This parameter can also be object id in a form of string (decimal or hex)
        //!
        //! \return component::ObjectId If object exist return id for it, otherwise 0.
        //!                             If there are several objects with same name, return first one
        component::ObjectId ResolveObjectId(const std::string& objectName);

        //! Return component bases on component type and object id if one exist
        //!
        //! \param componentType Type of Component
        //! \param objectId ObjectId (as dec or hex) or object name
        //!
        //! \return IComponent if Component found return instance, otherwise NULL
        IComponent *FindComponent(const std::string& componentType, const std::string& objectId);


    } // namespace config
} // namespace eg

#endif // OBJECT_MANAGER_KONSOLE_H

