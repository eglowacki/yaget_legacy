//////////////////////////////////////////////////////////////////////
// ClientConnectorApp.h
//
//  Copyright 12/5/2007 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//
//
//  #include "ClientConnectorApp.h"
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef _CLIENT_CONNECTOR_APP_H
#define _CLIENT_CONNECTOR_APP_H
#pragma once

#include "Base.h"
#include "IClientConnector.h"
#include "ClientConnector.h"
#include "utilities/usetypes.h"
#include "utilities/observers.h"
#include "appframework/clientapplication.h"
#include "utilities/callbacks.h"
#include "threading/thread.h"


namespace eg
{
    //! Forward declarations
    class Message;
    //class ClientConnector;

    class ClientConnectorApp : public IClientConnector, public omega::ClientApplication, public boost::signals::trackable
    {
    public:
        //! Constructor
        ClientConnectorApp();
        virtual ~ClientConnectorApp();

        virtual void HandleInitialized();
        //!< This method is invoked when the application has completed initialization.
        /*!< This also means it has established its presence within the runtime
        environment.  All Application methods are valid after this is received.
        */

        virtual void HandleShutdown();
        //!< This method is invoked when the application is about to shut down.
        /*!< Application methods are still valid before this method returns.
        */

        virtual void HandleAnnouncement(const OString& announcement);
        //!< This method is invoked to request a server-originated announcement be displayed to the user.
        /*!< A announcement as text is given, and it is the application's responsibility to display it.
        \param announcement The announcement.
        */

        virtual void HandleAnnouncement(uint32 announcementID);
        //!< This method is invoked to request a server-originated message be displayed to the user.
        /*!< A message id is given, and it is the application's responsibility to translate it to a string and display it.
        \param announcementID The message id.
        */

        virtual void HandleLoginFailure(const OString& message);
        //!< This method is invoked if the "login" fails.
        /*!< A message is given, and it is the application's
        responsibility to display it.
        \param message The message.
        */

        virtual void HandleLaunchFailure(omega::ClientApplication::FailureReason messageID);
        //!< This method is invoked if the launch fails.
        /*!< A message id is given, and it is the application's
        responsibility to translate it to a string and display it.
        \param messageID The message id.
        */

        virtual void HandleDisconnect();
        //!< This method is invoked if the connection to the server is broken unexpectedly.

        void Logout();
        //!< Tell the server I'm leaving

        void Quit();
        //!< Shut down cleanly

        virtual unsigned int Run(omega::Thread* thread=0);
        //!< Starts the main execution loop of the Application.
        /*!< This method overrides the Application::Run() method so that the
        thread can be captured. After doing so, it invokes Application::Run().
        \see Thread::Start()
        \see Application::Run()
        */

        //! From IClientConnector
        virtual Ticket Login(const std::string& userName, const std::string& password, const std::string& loginAddress);
        virtual void Connect(const std::string& processName, const Ticket& ticket);

    private:
        void onConnectorStatus(ClientConnector::Status status, const std::string& name);

        //! Helper method to create connection to process after logging was successful
        void createConnector(const std::string& userName, const std::string& processName);
        void resetConnector();

        enum State {sDefault, sConnected, sQuiting};

        //! Hold user login data
        struct UserConnectionInfo
        {
            std::string address;//"@stldude:8995:castlehilltest";
            std::string userName;//"eglowacki";
            std::string password;//"kremena";
            // when state goes from sDefault to sConnected
            // and this is set to non-empty, create ClientConnector object
            std::string processName;//"Simulator:simulator"
        };

        UserConnectionInfo mUserConnectionInfo;

        omega::Thread *mThread;
        //!< The Thread in use

        bool mbQuitting;
        //!< Flag indicating that the process is quitting.

        State mObjectState;

        ClientConnector *mpConnector;

        void onShutdown(Message& msg);
        void onTick(Message& msg);

        boost::signals::connection mOnShutdownConnection;
        boost::signals::connection mOnTickConnection;
    };

} // namespace eg

#endif // _CLIENT_CONNECTOR_APP_H
