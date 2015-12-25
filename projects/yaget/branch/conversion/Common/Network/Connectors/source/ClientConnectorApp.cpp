//#include "vld.h"
#pragma warning(push)
#pragma warning (disable : 4244)  // '' : conversion from '' to '', possible loss of data
#include "ClientConnectorApp.h"
#include "ClientConnector.h"
#include "Message/Dispatcher.h"
#include "Math/Vector.h"
#include "utilities/commandlineparser.h"
#include "objectsystem/objectaddress.h"
#include "threading/thread.h"
#include "logging/logger.h"
#include "sockets/socketfactory.h"
#include "sockets/socketmanager.h"
#include "Hash/Hash.h"
#include "wx/wx.h"

#include <boost/algorithm/string.hpp>
#include <boost/random.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>
#pragma warning(pop)

#include <ctime>
#include <sstream>
#include <vector>



omega::Frame& operator <<(omega::Frame& f, const eg::Vector2& Ref)
{
    f << Ref.x << Ref.y;
    return f;
}


omega::Frame& eg::operator >>(omega::Frame& f, eg::Vector2& Ref)
{
    f >> Ref.x >> Ref.y;
    return f;
}


omega::Frame& operator <<(omega::Frame& f, const eg::Vector3& Ref)
{
    f << Ref.x << Ref.y << Ref.z;
    return f;
}


omega::Frame& eg::operator >>(omega::Frame& f, eg::Vector3& Ref)
{
    f >> Ref.x >> Ref.y >> Ref.z;
    return f;
}


omega::Frame& operator <<(omega::Frame& f, const eg::Vector4& Ref)
{
    f << Ref.x << Ref.y << Ref.z << Ref.w;
    return f;
}


omega::Frame& eg::operator >>(omega::Frame& f, eg::Vector4& Ref)
{
    f >> Ref.x >> Ref.y >> Ref.z << Ref.w;
    return f;
}

namespace eg
{

    // the constructor
ClientConnectorApp::ClientConnectorApp()
: ClientApplication()
, mThread(0)
, mObjectState(sDefault)
, mpConnector(0)
{
    Dispatcher& disp = REGISTRATE(Dispatcher);
    mOnShutdownConnection = disp[mtype::kShutdownBegin].connect(boost::bind(&ClientConnectorApp::onShutdown, this, _1));
    mOnTickConnection = disp[mtype::kFrameTick].connect(boost::bind(&ClientConnectorApp::onTick, this, _1));

    GetSocketManager().GetSocketFactory().LoadTransportFile("transports.xml");

    Initialize();
}


ClientConnectorApp::~ClientConnectorApp()
{
    Quit();
}


// override of Application::Run()
unsigned int ClientConnectorApp::Run(omega::Thread *thread)
{
    wxLogTrace("Connector", "Thread started.");

    // capture the value of the thread for later use in the shut down sequence
    mThread = thread;

    // invoke the base method and return its results.
    return ClientApplication::Run(thread);
}


// the method is invoked by the framework when the login sequence has been
// successfully completed.
void ClientConnectorApp::HandleInitialized()
{
    wxLogTrace("Connector", "Got response from Server, creating connection object to Simulator.");

    mObjectState = sConnected;
    sigLogin(true, "");
    if (!mUserConnectionInfo.processName.empty())
    {
        createConnector(mUserConnectionInfo.userName, mUserConnectionInfo.processName);
    }
}


// the method is invoked by the framework when the application is about to shut down
void ClientConnectorApp::HandleShutdown()
{
    wxLogTrace("Connector", "We are finished!");

}


// the method is invoked by the framework when the login has failed
void ClientConnectorApp::HandleLoginFailure(const OString& message)
{
    wxLogTrace("Connector", "Login failure. Error: '%s'.", message.c_str());

    sigLogin(false, "Login failure");
    resetConnector();
    // shut down the app
    Quit();
}


// the method is invoked by the framework when the connection to the server has failed
// after login.
void ClientConnectorApp::HandleLaunchFailure(omega::ClientApplication::FailureReason messageID)
{
    wxLogTrace("Connector", "Connection to Server failure. Error ID '%d'.", messageID);

    sigLogin(false, "Login failure");
    resetConnector();
    // shut down the app
    //Quit();
    //REGISTRATE(InputManager).TriggerAction("Quit App");
}


// the method is invoked by the framework when the connection to the server is
// lost unexpectedly
void ClientConnectorApp::HandleDisconnect()
{
    wxLogError("Lost connection to Server.");

    sigLogin(false, "User disconnected");
    resetConnector();
    // shut down the app
    //Quit();
    //REGISTRATE(InputManager).TriggerAction("Quit App");
}


// the method is invoked by the framework when there is an announcement from
// the server operators
void ClientConnectorApp::HandleAnnouncement(const OString& announcement)
{
    wxLogWarning("Not Implemented yet, announcement message '%s'", announcement.c_str());
}


// the method is invoked by the framework when there is an announcement from
// the server operators
void ClientConnectorApp::HandleAnnouncement(uint32 announcementID)
{
    wxLogWarning("Not Implemented yet, announcement ID '%d'", announcementID);
}


// shut down the app, closing all connections and stopping the thread if there is one.
void ClientConnectorApp::Quit()
{
    mOnShutdownConnection.disconnect();
    mOnTickConnection.disconnect();

    // Block multiple calls.
    if (mObjectState == sQuiting)
    {
        return;
    }
    mObjectState = sQuiting;
    if (mpConnector)
    {
        mpConnector->Release();
        mpConnector = 0;
    }

    // tell the OMEGA application to stop
    Stop();

    // if we own a thread, stop it
    if (mThread)
    {
        mThread->Join();
    }

    // Shutdown when we're done.
    Shutdown();
}


// tell the server that we want to log out
void ClientConnectorApp::Logout()
{
}


void ClientConnectorApp::onShutdown(Message& /*msg*/)
{
    Quit();
}


void ClientConnectorApp::onTick(Message& /*msg*/)
{
    if (!mThread)
    {
        Service();
    }
}

IClientConnector::Ticket ClientConnectorApp::Login(const std::string& userName, const std::string& password, const std::string& loginAddress)
{
    wxLogTrace("Connector", "Login for '%s' to '%s'.", userName.c_str(), loginAddress.c_str());

    if (StartLogin(userName.c_str(), password.c_str(), loginAddress.c_str()))
    {
        mUserConnectionInfo.userName = userName;
        mUserConnectionInfo.password = password;
        mUserConnectionInfo.address = loginAddress;

        Hash nameTicket(userName.c_str());
        return Ticket(nameTicket.GetValue());
    }

    return Ticket(0);
}


void ClientConnectorApp::Connect(const std::string& processName, const Ticket& ticket)
{
    wxLogTrace("Connector", "Connecting to '%s'.", processName.c_str());

    if (ticket != IClientConnector::Invalid)
    {
        switch (mObjectState)
        {
            case sConnected:
                // we are in connected state, so initiate connector to user supplied process name
                createConnector(mUserConnectionInfo.userName, processName);
                break;

            case sDefault:
                // we are still in non connected state, just defer connection until
                // HandleInitialized is called
                mUserConnectionInfo.processName = processName;
                break;

            default:
                // \todo notify of invalid ticket
                break;
        }
    }
}


void ClientConnectorApp::createConnector(const std::string& userName, const std::string& processName)
{
    wxLogTrace("Connector", "Connector Name '%s' for user '%s'.", processName.c_str(), userName.c_str());
    wxASSERT(mpConnector == 0);

    mpConnector = new ClientConnector(userName.c_str(), GetObjectManager());
    mpConnector->sigConnection.connect(boost::bind(&ClientConnectorApp::onConnectorStatus, this, _1, _2));
    mpConnector->Connect(processName.c_str());
}


void ClientConnectorApp::resetConnector()
{
    mObjectState = sDefault;
    mUserConnectionInfo.processName = "";
    if (mpConnector)
    {
        mpConnector->Release();
        mpConnector = 0;
    }
}


void ClientConnectorApp::onConnectorStatus(ClientConnector::Status status, const std::string& /*name*/)
{
    if (status == ClientConnector::sConnected)
    {
        sigConnected(true, "");
    }
    else if (status == ClientConnector::sClosed)
    {
        sigConnected(false, "Shard connection closed");
    }
    else if (status == ClientConnector::sFailed)
    {
        sigConnected(false, "Shard connection failed");
    }
}


connectors::ClientConnector_t connectors::GetClient()
{
    connectors::ClientConnector_t connector(new ClientConnectorApp);

    return connector;
}

} // namespace eg

