/////////////////////////////////////////////////////////////////////
// IObjectTemplates.h
//
//  Copyright 7/12/2008 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//      This interface provides facility to create full objects
//      based on predefined templates and generate events
//      and full object is constructed. It also when components
//      are created independently, it can keep track and do the same
//      with regard to generated event
//
//
//  #include "IObjectTemplates.h"
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef IOBJECT_TEMPLATES_H
#define IOBJECT_TEMPLATES_H
#pragma once

#include "ObjectsBase.h"
#include "ObjectID.h"

namespace eg
{
    /*!
     * When Object is fully created (all components based on template)
     * it will send mtype::kObjectConstructed message.
     *
     */
    class IObjectTemplates
    {
    public:
        //! List of all component id's making up one template object
        //typedef std::set<component::ComponentId> Components_t;
        typedef std::set<uint32_t> Components_t;

        //! Create full object based on templateId
        //!
        //! \param templateId which template id to use for creation of new object
        //! \param oId object id to assign to newly created object
        //! \param name assign this name to newly created object
        //!
        //! \return bool true all was OK, otherwise FALSE
        //! \note After creation of all components it will send mtype::kObjectConstructed message
        //!       before returning to caller.
        virtual bool Create(component::ObjectId templateId, component::ObjectId oId, const std::string& name) const = 0;

        //! Add new template
        //!
        //! \param templateId template id assign to this components set
        //! \param components list of components making up this template
        virtual void Add(component::ObjectId templateId, const Components_t& components) = 0;

        //! Remove template
        //!
        //! \param templateId id of a template to remove it
        virtual void Remove(component::ObjectId templateId) = 0;

        //! Load template data from stream
        //!
        //! \param dataStream valid input stream
        //! \note It will clear previous values
        virtual void Load(VirtualFileFactory::istream_t dataStream) = 0;

        //! Save template data to stream
        //!
        //! \param dataStream stream to save template data
        virtual void Save(VirtualFileFactory::ostream_t dataStream) const = 0;
    };
} // namespace eg

#endif // IOBJECT_TEMPLATES_H

