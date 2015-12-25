// briged methods to allow using boost
#include "Precompiled.h"
#include "Bridge.h"
#include "EditorBase.h"
#include "Config/Console.h"
#include "Message/Dispatcher.h"
#include "Property/PropertyData.h"
#include "IComponent.h"
#include "File/VirtualFileSystem.h"
#include <boost/signal.hpp>

namespace
{
	class Connection
	{
	public:
		Connection(yaget::managed::CallbackNative callback, guid_t type)
		: mCallback(callback)
		{
			eg::Dispatcher& disp = eg::registrate::ref_cast<eg::Dispatcher>("Dispatcher");
			mConnection = disp[type].connect(mCallback);
		}

		Connection::~Connection()
		{
			mConnection.disconnect();
		}

	private:
		yaget::managed::CallbackNative mCallback;
		boost::signals::connection mConnection;
	};

}  // namespace

namespace yaget	{
namespace managed {


//! Temporary here until I figure out how to expose classes with boost::signal
//! it crashes dll right now if boost::signal include is present in any clr files.
void konsoleExecute(const std::string& cmd)
{
	if (eg::config::Konsole *konsole = eg::registrate::p_cast<eg::config::Konsole>("Konsole"))
	{
		konsole->Execute(cmd);
	}
}


Channel::Channel(CallbackNative callback, guid_t type)
: mpConnection(new Connection(callback, type))
{
}


Channel::~Channel()
{
	delete mpConnection;
}


namespace vf
{

std::vector<std::string> GetFileList(const std::string& filter)
{
	eg::VirtualFileSystem& vfs = eg::registrate::ref_cast<eg::VirtualFileSystem>("VirtualFileSystem");
	std::vector<std::string> fileList = vfs.GetFileList(filter);
	return fileList;

}


std::string GetFqn(const std::string& fileName)
{
	eg::VirtualFileSystem& vfs = eg::registrate::ref_cast<eg::VirtualFileSystem>("VirtualFileSystem");
	return vfs.GetFqn(fileName);
}

} // namespace vf
} // namespace managed
} // namespace yaget