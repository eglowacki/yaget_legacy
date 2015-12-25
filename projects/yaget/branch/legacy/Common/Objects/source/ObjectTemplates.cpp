#include "Precompiled.h"
#include "IComponent.h"
#include "ObjectTemplates.h"
#include "MessageInterface.h"
#include "Message/Dispatcher.h"
#include "File/ArchiveHelpers.h"
#include <boost/bind.hpp>
//#include <boost/serialization/split_free.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>


namespace boost
{
    namespace serialization
    {
        using namespace eg;
        //BOOST_SERIALIZATION_SPLIT_FREE(Hash)

        /*
        template<class Archive>
        void save(Archive& ar, const Hash& hashValue, unsigned int version)
        {
            //...
        }

        template<class Archive>
        void load(Archive& ar, Hash& hashValue, unsigned int version)
        {
            //...
        }

        template<class Archive>
        void serialize(Archive& ar, Hash& hashValue, const unsigned int version)
        {
            ar & static_cast<unsigned int>(hashValue.GetValue());
            //split_free(ar, hashValue, version);
        }
        */

    } // namespace serialization
} // namespace boost

namespace eg {

/*
// compiler does not like this to be in anonymous namespace
std::istream& operator>>(std::istream& stream, ObjectTemplates::Templates_t& templateData)
{
    return stream;
}

std::ostream& operator<<(std::ostream& stream, const ObjectTemplates::Templates_t& templateData)
{
    stream << templateData.size();
    for (ObjectTemplates::Templates_t::const_iterator it = templateData.begin(); it != templateData.end(); ++it)
    {
        templateData << (*it).first;
        const ObjectTemplates::Components_t& components = (*it).second;
        stream << components.size();
        for (ObjectTemplates::Templates_t::const_iterator it = templateData.begin(); it != templateData.end(); ++it)
        {
        }
    }

    return stream;
}
*/


ObjectTemplates::ObjectTemplates()
{
    //Dispatcher& disp = REGISTRATE(Dispatcher);
    //mComponentNewSlot = disp[mtype::kComponentNew].connect(boost::bind(&ObjectTemplates::onComponentNew, this, _1));
}


ObjectTemplates::~ObjectTemplates()
{
    mComponentNewSlot.disconnect();
}


bool ObjectTemplates::Create(component::ObjectId templateId, component::ObjectId oId, const std::string& name) const
{
    Templates_t::const_iterator it = mTemplates.find(templateId);
    if (it != mTemplates.end())
    {
        const Components_t& components = (*it).second;
        for (Components_t::const_iterator it_c = components.begin(); it_c != components.end(); ++it_c)
        {
            component::InstanceId iId(oId, *it_c);
            component::newc(name, iId);
        }

        wxLogTrace(TR_OBJECTS, "Created new Object %d:%s based on Template %d.", oId.GetValue(), name.c_str(), templateId.GetValue());
        Message(mtype::kObjectConstructed, oId).Send();
        return true;
    }

    wxLogError("TemplateId %d does not exist, could not create Object %d:%s.", templateId.GetValue(), oId.GetValue(), name.c_str());
    return false;
}


void ObjectTemplates::Add(component::ObjectId templateId, const Components_t& components)
{
    if (mTemplates.find(templateId) == mTemplates.end())
    {
        mTemplates.insert(std::make_pair(templateId, components));
    }
    else
    {
        wxLogError("TemplateId %d already exist, could not add it.", templateId);
    }
}


void ObjectTemplates::Remove(component::ObjectId templateId)
{
    if (mTemplates.find(templateId) != mTemplates.end())
    {
        mTemplates.erase(templateId);
    }
    else
    {
        wxLogError("TemplateId %d does not exist, could not remove it.", templateId.GetValue());
    }
}


void ObjectTemplates::Load(VirtualFileFactory::istream_t dataStream)
{
    mTemplates.clear();
    if (!LoadFromArchive<boost::archive::text_iarchive>(mTemplates, *dataStream))
    {
        wxLogError("Could not load template data from stream.");
    }
}


void ObjectTemplates::Save(VirtualFileFactory::ostream_t dataStream) const
{
    if (!SaveToArchive<boost::archive::text_oarchive>(mTemplates, *dataStream))
    {
        wxLogError("Could not save template data to stream.");
    }
}


void ObjectTemplates::onComponentNew(Message& /*msg*/)
{
    wxLogError("ObjectTemplates::onComponentNew is not Implemented!!!");
    /*
    component::InstanceId iId = msg.GetValue<component::InstanceId>();
    Templates_t::const_iterator it = mTemplates.find(iId.first);
    if (it != mTemplates.end())
    {
        Components_t& components = mObjects[iId.first];
        components.insert(iId.second);

        const Components_t& templateComponents = (*it).second;
        if (templateComponents == components)
        {
            // object is fully constructed,
            // so let send message and remove it from mObjects
            Message(mtype::kObjectConstructed, iId.first).Send();

            mObjects.erase(iId.first);
        }
    }
    */
}


} // namespace eg
