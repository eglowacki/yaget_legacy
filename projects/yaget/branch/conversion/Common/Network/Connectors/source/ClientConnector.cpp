#include "ClientConnector.h"
#include "appframework/application.h"
#include "logging/logger.h"
//#include "BaseDLLs.h"
#include "ObjectID.h"
#include "Registrate.h"
#include "IObjectManager.h"
#include "IComponent.h"
#include "File/VirtualFileSystem.h"
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/format.hpp>



namespace
{
    // Helper function to update property with new value
    template <typename T>
    void ReplicateProp(omega::uint64 oId, omega::uint32 cId, const char *pName, const T& value)
    {
        using namespace eg;
        OMEGA_LOG(DEBUG, "Replication for " << component::instanceName(component::make(oId, cId)) << ", Prop '" << pName << "'.");

        if (IComponent *pComponent = component::get(component::make(oId, cId)))
        {
            IPropertySet& propSet = pComponent->GetPropertyData();
            propSet[pName] = (T)value;
        }
    }

} // namespace


namespace eg
{

ClientConnector::ClientConnector(const omega::OString& name, omega::ObjectManager& manager)
: ClientConnectorObjectBase(name, manager)
, mObserverId(0)
{
    OMEGA_LOG(DEBUG, "Name: " << name);
    // attach callback switch for getting both observer and replicate proxies
    mAttachSwitch.TriggerSwitch = boost::bind(&ClientConnector::onReplicatorReady, this);
}


ClientConnector::~ClientConnector()
{
}


void ClientConnector::Initialize(const omega::OString& init)
{
    OMEGA_LOG(DEBUG, "With '" << init << "'");
    /*
    OMEGA_LOG_0(LOG_DEBUG, "IDServer::Initialize", "Start.");

    omega::XMLNode* xml = handleInitString(init);
    if(xml != 0)
    {
      handleConfigValues(xml->FindChild("idserverConfig"));
    }
    delete xml;

    OMEGA_LOG_0(LOG_DEBUG, "IDServer::Initialize", "Complete.");
    */
}


void ClientConnector::HandleConnectFailure(omega::Object::ConnectionID /*connection*/)
{
    sigConnection(sFailed, "Simulator");
}


void ClientConnector::onReplicatorReady()
{
    // set observer radius as a 10000 units
    mAttachSwitch.mT1->SetObserverInfo(10000);
    // and attach replicator to observer
    mAttachSwitch.mT2->AttachToObserver(mObserverId);

    sigConnection(sConnected, "Simulator");
}


//-------------------------------------------------------------------------------------------
// from SimulatorObserverProtocol::ISimulatorObserverReply
void ClientConnector::HandleOpen(ISimulatorObserverRequestProxy& ref)
{
    OMEGA_LOG(DEBUG, "Opened connection to '" << ref.GetDestinationSupportedInterfaces() << "'.");
    mObserverId = 0;
}


void ClientConnector::HandleClose(ISimulatorObserverRequestProxy& ref)
{
    OMEGA_LOG(DEBUG, "Closed connection to '" << ref.GetDestinationSupportedInterfaces() << "'.");

    if (mObserverId)
    {
        sigConnection(sClosed, "Simulator");
    }

    mObserverId = 0;
}


void ClientConnector::ObserverId(ISimulatorObserverRequestProxy& reply, omega::uint64 observerId)
{
    OMEGA_LOG(DEBUG, "Proxy '" << reply.GetDestinationName() << "' assigned us Id '" << observerId << "'.");

    mObserverId = observerId;
    mAttachSwitch.Switch(reply);
}


void ClientConnector::Add(ISimulatorObserverRequestProxy& reply, omega::uint64 objectId)
{
    OMEGA_LOG(DEBUG, "Proxy '" << reply.GetDestinationName() << "' Added ObjectId '" << objectId << "'.");

    // at this point we do not have any data as to the name of this instance.
    // so how we determine what kind of components we need to create/load
    // do we take objectId, string-ize into file name with some extension
    std::string fileName = boost::str(boost::format("0x%016X") % objectId);
    fileName += ".odf"; // object definition file
    if (VirtualFileFactory::istream_t istream = REGISTRATE(VirtualFileSystem).GetFileStream(fileName))
    {
        loadInstance(objectId, istream);
    }
    else
    {
        OMEGA_LOG(WARNING, "Couldn't get stream for '" << fileName << "'.");
    }
}


void ClientConnector::Add(ISimulatorObserverRequestProxy& reply, omega::uint64 objectId, const omega::OString& streamName)
{
    OMEGA_LOG(DEBUG, "Proxy '" << reply.GetDestinationName() << "' Added ObjectId '" << objectId << "' using Stream '" << streamName << "'.");

    std::string fileName = streamName.c_str();
    if (!boost::iends_with(fileName, ".odf"))
    {
        fileName += ".odf";
    }
    if (VirtualFileFactory::istream_t istream = REGISTRATE(VirtualFileSystem).GetFileStream(fileName))
    {
        loadInstance(objectId, istream);
    }
    else
    {
        OMEGA_LOG(ERROR, "Couldn't get stream for '" << fileName << "'.");
    }
}


void ClientConnector::Add(ISimulatorObserverRequestProxy& reply, omega::uint64 objectId, const omega::Frame& frame)
{
    OMEGA_LOG(DEBUG, "Proxy '" << reply.GetDestinationName() << "' Added ObjectId '" << objectId << "' with Frame data.");

    std::string strFrame((const char *)frame.GetConstData(), frame.GetSize());
    eg::VirtualFileFactory::istream_t istream(new std::istringstream(strFrame));
    loadInstance(objectId, istream);
}


void ClientConnector::loadInstance(omega::uint64 objectId, VirtualFileFactory::istream_t istream)
{
    using namespace eg;
    component::ObjectId oId(static_cast<uint32_t>(objectId));
    // roll_back_marker
    REGISTRATE(IObjectManager).LoadObject(oId, istream);
    OMEGA_LOG(DEBUG, "Added instance '" << component::objectName(oId) << "'");
}


void ClientConnector::Remove(ISimulatorObserverRequestProxy& reply, omega::uint64 objectId)
{
    OMEGA_LOG(DEBUG, "Proxy '" << reply.GetDestinationName() << "' Removed ObjectId '" << objectId << "'.");

    using namespace eg;
    component::ObjectId oId(objectId);
    // roll_back_marker
    OMEGA_LOG(DEBUG, "Removed instance '" << component::objectName(oId) << "'");
    REGISTRATE(IObjectManager).DestroyInstance(oId);
}
//-------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------
// from ReplicateProtocol::IReplicateReply
void ClientConnector::HandleOpen(IReplicateRequestProxy& ref)
{
    OMEGA_LOG(DEBUG, "Opened connection to '" << ref.GetDestinationSupportedInterfaces() << "'.");

    mAttachSwitch.Switch(ref);
}


void ClientConnector::HandleClose(IReplicateRequestProxy& ref)
{
    OMEGA_LOG(DEBUG, "Closed connection to '" << ref.GetDestinationSupportedInterfaces() << "'.");

    if (mObserverId)
    {
        sigConnection(sClosed, "Simulator");
    }
}


void ClientConnector::Replicate(IReplicateRequestProxy& reply, omega::uint64 oId, omega::uint32 cId, const omega::OString& propName, bool value)
{
    reply;
    ReplicateProp<bool>(oId, cId, propName.c_str(), value);
}


void ClientConnector::Replicate(IReplicateRequestProxy& reply, omega::uint64 oId, omega::uint32 cId, const omega::OString& propName, omega::float32 value)
{
    reply;
    ReplicateProp<float>(oId, cId, propName.c_str(), value);
}


void ClientConnector::Replicate(IReplicateRequestProxy& reply, omega::uint64 oId, omega::uint32 cId, const omega::OString& propName, omega::int32 value)
{
    reply;
    ReplicateProp<int32_t>(oId, cId, propName.c_str(), value);
}


void ClientConnector::Replicate(IReplicateRequestProxy& reply, omega::uint64 oId, omega::uint32 cId, const omega::OString& propName, omega::uint32 value)
{
    reply;
    ReplicateProp<uint32_t>(oId, cId, propName.c_str(), value);
}


void ClientConnector::Replicate(IReplicateRequestProxy&reply, omega::uint64 oId, omega::uint32 cId, const omega::OString& propName, const eg::Vector2& value)
{
    reply;
    ReplicateProp<Vector2>(oId, cId, propName.c_str(), value);
}


void ClientConnector::Replicate(IReplicateRequestProxy& reply, omega::uint64 oId, omega::uint32 cId, const omega::OString& propName, const eg::Vector3& value)
{
    reply;
    ReplicateProp<eg::Vector3>(oId, cId, propName.c_str(), value);
}


void ClientConnector::Replicate(IReplicateRequestProxy& reply, omega::uint64 oId, omega::uint32 cId, const omega::OString& propName, const eg::Vector4& value)
{
    reply;
    ReplicateProp<Vector4>(oId, cId, propName.c_str(), value);
}


void ClientConnector::Replicate(IReplicateRequestProxy& reply, omega::uint64 oId, omega::uint32 cId, const omega::OString& propName, const omega::OString& value)
{
    OMEGA_LOG(DEBUG, "Replication from '" << reply.GetDestinationSupportedInterfaces() << "' for '" << oId << ":" << cId << "', Value '" << value << "' as String (Suboptimal).");

    if (IComponent *pComponent = component::get(component::make(oId, cId)))
    {
        const std::string kProp(propName.c_str());
        const std::string kValue(value.c_str());
        IPropertySet& propSet = pComponent->GetPropertyData();
        propSet[kProp] = kValue;
    }
}


//-------------------------------------------------------------------------------------------

} // namespace eg

