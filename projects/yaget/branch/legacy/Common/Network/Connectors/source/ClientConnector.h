//////////////////////////////////////////////////////////////////////
// ClientConnector.h
//
//  Copyright 12/5/2007 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//
//
//  #include "ClientConnector.h"
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef _CLIENT_CONNECTOR_H
#define _CLIENT_CONNECTOR_H
#pragma once

#include "File/VirtualFileFactory.h"
#include "generated/ClientConnectorObject.h"
#include "utilities/servicescheduler.h"
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>

#pragma warning(push)
#pragma warning (disable : 4512)  // '' : assignment operator could not be generated
#include <boost/signals.hpp>
#pragma warning(pop)

// forward declarations
namespace {class Client;}

namespace eg
{
    template <typename T1, typename T2>
    class MultiSwitch
    {
    public:
        typedef T1 Switch1_t;
        typedef T2 Switch2_t;
        typedef MultiSwitch<Switch1_t, Switch2_t> Type_t;

        typedef boost::function<void()> Trigger;

        MultiSwitch()
            : mSwitchBits(0)
        {
        }

        void Switch(Switch1_t& s)
        {
            if (!(mSwitchBits & 1))
            {
                mSwitchBits |= 1;
                mT1.reset(new T1(s));
                checkForOn();
            }
        }

        void Switch(Switch2_t& s)
        {
            if (!(mSwitchBits & 2))
            {
                mSwitchBits |= 2;
                mT2.reset(new T2(s));
                checkForOn();
            }
        }

        Trigger TriggerSwitch;

        boost::shared_ptr<Switch1_t> mT1;
        boost::shared_ptr<Switch2_t> mT2;

    private:
        void checkForOn() const
        {
            if (mSwitchBits == 0x3 && TriggerSwitch)
            {
                TriggerSwitch();
            }
        }

        uint32_t mSwitchBits;
    };


    class ClientConnector : public ClientConnectorObjectBase
    {
    public:
        ClientConnector(const omega::OString& name, omega::ObjectManager& manager);
        virtual ~ClientConnector();

        //! This is used to pass current status of connection to sigConnection signal
        enum Status
        {
            sConnected,     ///< connection is valid
            sClosed,        ///< Connection is closed
            sFailed         ///< Connection failed
        };

        //! This is triggered for connection open, close or failure
        //! \param Status one of then Status enums
        //! \param string connection name
        boost::signal<void (Status, const std::string&)> sigConnection;

        virtual void Initialize(const omega::OString& init);
        virtual void HandleConnectFailure(omega::Object::ConnectionID connection);

        // from SimulatorObserverProtocol::ISimulatorObserverReply
        virtual void HandleOpen(ISimulatorObserverRequestProxy& ref);
        virtual void HandleClose(ISimulatorObserverRequestProxy& ref);
        virtual void Add(ISimulatorObserverRequestProxy& reply, omega::uint64 objectId);
		virtual void Add(ISimulatorObserverRequestProxy& reply, omega::uint64 objectId, const omega::OString& streamName);
        virtual void Add(ISimulatorObserverRequestProxy& reply, omega::uint64 objectId, const omega::Frame& frame);
        virtual void Remove(ISimulatorObserverRequestProxy& reply, omega::uint64 objectId);
        virtual void ObserverId(ISimulatorObserverRequestProxy& reply, omega::uint64 observerId);

        // from ReplicateProtocol::IReplicateReply
        virtual void HandleOpen(IReplicateRequestProxy& ref);
        virtual void HandleClose(IReplicateRequestProxy& ref);
        virtual void Replicate(IReplicateRequestProxy& reply, omega::uint64 oId, omega::uint32 cId, const omega::OString& propName, bool value);
        virtual void Replicate(IReplicateRequestProxy& reply, omega::uint64 oId, omega::uint32 cId, const omega::OString& propName, omega::float32 value);
        virtual void Replicate(IReplicateRequestProxy& reply, omega::uint64 oId, omega::uint32 cId, const omega::OString& propName, omega::int32 value);
        virtual void Replicate(IReplicateRequestProxy& reply, omega::uint64 oId, omega::uint32 cId, const omega::OString& propName, omega::uint32 value);
        virtual void Replicate(IReplicateRequestProxy& reply, omega::uint64 oId, omega::uint32 cId, const omega::OString& propName, const eg::Vector2& value);
        virtual void Replicate(IReplicateRequestProxy& reply, omega::uint64 oId, omega::uint32 cId, const omega::OString& propName, const eg::Vector3& value);
        virtual void Replicate(IReplicateRequestProxy& reply, omega::uint64 oId, omega::uint32 cId, const omega::OString& propName, const eg::Vector4& value);
        virtual void Replicate(IReplicateRequestProxy& reply, omega::uint64 oId, omega::uint32 cId, const omega::OString& propName, const omega::OString& value);

    private:
        //! Called when both proxies, replicate and observer are connected.
        void onReplicatorReady();

        // helper method to load new instance from stream
        void loadInstance(omega::uint64 objectId, VirtualFileFactory::istream_t istream);

        //! This is assigned to us by Simulator Object and will retain this id thought
        //! of a session. There is no guarantee as to uniqueness between sessions. IOW,
        //! client may get the same id in future sessions. 0 is invalid.
        omega::uint64 mObserverId;

        typedef MultiSwitch<ISimulatorObserverRequestProxy, IReplicateRequestProxy> AttachSwitch_t;

        AttachSwitch_t mAttachSwitch;
    };

} // namespace eg

#endif // _CLIENT_CONNECTOR_H

