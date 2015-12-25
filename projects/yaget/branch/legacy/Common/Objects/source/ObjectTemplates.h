/////////////////////////////////////////////////////////////////////
// ObjectTemplates.h
//
//  Copyright 7/12/2008 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//      Concrete implementation of IObjectTemplates
//      interface.
//
//
//  #include "ObjectTemplates.h"
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef OBJECT_TEMPLATES_H
#define OBJECT_TEMPLATES_H
#pragma once

#include "IObjectTemplates.h"
#include <boost/signal.hpp>
#include <boost/serialization/set.hpp>
#include <boost/serialization/map.hpp>
//#include <set>


namespace eg
{
    class Message;

    class ObjectTemplates : public IObjectTemplates
    {
    public:
        //! This provides map to hold all components belonging to template
        //typedef std::map<component::ObjectId, Components_t> Templates_t;
        typedef std::map<uint32_t, Components_t> Templates_t;

        ObjectTemplates();
        ~ObjectTemplates();

        //! from IObjectTemplates
        virtual bool Create(component::ObjectId templateId, component::ObjectId oId, const std::string& name) const;
        virtual void Load(VirtualFileFactory::istream_t dataStream);
        virtual void Save(VirtualFileFactory::ostream_t dataStream) const;
        virtual void Add(component::ObjectId templateId, const Components_t& components);
        virtual void Remove(component::ObjectId templateId);

    private:
        void objectCreated(component::ObjectId oId);

        //! Called for every created component
        void onComponentNew(Message& msg);
        boost::signals::connection mComponentNewSlot;

        //! data representing templates
        //! This is loaded at runtime from persistent storage
        Templates_t mTemplates;

        //! Hold current objects being created.
        //! When object is fully constructed, it will send message
        //! and remove from map
        Templates_t mObjects;
    };

} // namespace eg

#endif // OBJECT_TEMPLATES_H

