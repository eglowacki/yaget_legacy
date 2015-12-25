//////////////////////////////////////////////////////////////////////
// IClientConnector.h
//
//  Copyright 12/22/2007 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//      Interface to start client connection
//      which provides observation and replication of objects
//
//
//  #include "IClientConnector.h"
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef _I_CLIENT_CONNECTOR_H
#define _I_CLIENT_CONNECTOR_H
#pragma once

#include <string>
#include <boost/shared_ptr.hpp>
#pragma warning(push)
#pragma warning (disable : 4512)  // '' : assignment operator could not be generated
#include <boost/signals.hpp>
#pragma warning(pop)

namespace eg
{

    class IClientConnector
    {
    public:
        //! Validation ticket used in connection to other processes.
        typedef uint64_t Ticket;
        static const Ticket Invalid = 0;

        virtual ~IClientConnector() = 0;

        //! Initiate login procedure
        //!
        //! \param userName - unique user name for this account
        //! \param password - password for this user
        //! \param loginAddress - address:port:process_name,
        //!                       @stldude:8995:castlehilltest
        //!
        //! \return ValidationTicket
        virtual Ticket Login(const std::string& userName, const std::string& password, const std::string& loginAddress) = 0;

        //! Connect this object to processName
        virtual void Connect(const std::string& processName, const Ticket& ticket) = 0;

        //! This is triggered for actual login is validated
        //! \param bool true login was valid, false login failed or got closed
        //! \param string This will contain error message if login failed or got closed
        boost::signal<void (bool, const std::string&)> sigLogin;

        //! This is triggered when there is actual connection to Shard
        //! \param bool true connection was valid, false connection failed or got closed
        //! \param string This will contain error message if login failed or got closed
        boost::signal<void (bool, const std::string&)> sigConnected;
    };

    inline IClientConnector::~IClientConnector()
    {}

    namespace connectors
    {
        typedef boost::shared_ptr<IClientConnector> ClientConnector_t;

        //! Returns client connector object. When last reference to this object
        //! is gone, it will delete itself
        ClientConnector_t GetClient();

    } // namespace connectors


} // namespace eg

#endif // _I_CLIENT_CONNECTOR_H

