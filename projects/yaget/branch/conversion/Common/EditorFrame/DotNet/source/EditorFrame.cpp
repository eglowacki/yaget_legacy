#include "Precompiled.h"
#include "EditorFrame.h"
#include "IRenderer.h"
#include "Config/Console.h"
#include <wx/dynload.h>
#include <wx/dir.h>
#include <sstream>
#include <boost/bind.hpp>


namespace
{
    const wxChar *kToggleEditorView = wxT("Toggle View");
    const std::string kEditorPackageName("EditorPlugins");

	void PrintKonsoleOutput(eg::config::KosoleResult /*result*/, const std::string& /*cmd*/, const std::string& text)
	{
		eg::Message(eg::mtype::kKonsolePrint, text).Send();
	}

} //  namespace


namespace eg 
{


IMPLEMENT_USER_EXPORTED_PLUGGABLE_CLASS(EditorFrame, IEditorFrame)


EditorFrame::EditorFrame() 
: mActionShowEditor(kToggleEditorView,
                  new InputManager::Key(InputTypes::kButtonDown|InputTypes::kButtonCtrl, '~'),
                  0,
                  boost::bind(&EditorFrame::onInput, this, _1, _2, _3, _4),
                  true)
, mbPluginsLoaded(false)
{
    Dispatcher& disp = REGISTRATE(Dispatcher);
    mShutdownConnection = disp[mtype::kShutdownBegin].connect(boost::bind(&EditorFrame::onShutdown, this, _1));
    mEditorClosedConnection = disp[mtype::kYepClosed].connect(boost::bind(&EditorFrame::onEditorClosed, this, _1));
    
    wxLogTrace(TR_EDITOR, "EditorFrame Plugin Created");
}


EditorFrame::~EditorFrame()
{
    mShutdownConnection.disconnect();
    mEditorClosedConnection.disconnect();
    std::for_each(mPluginTools.rbegin(), mPluginTools.rend(), std::mem_fun(&wxPluginLibrary::Unload));

    wxLogTrace(TR_EDITOR, "EditorFrame Plugin Destroyed");
}


void EditorFrame::onShutdown(Message& /*msg*/)
{
    if (mbPluginsLoaded)
    {
        mbPluginsLoaded = false;
        //yaget::managed::UnloadPlugins(kEditorPackageName);
    }

	mKonsoleConnection.disconnect();
	//yaget::log::Deactivate();
}


void EditorFrame::onEditorClosed(Message& /*msg*/)
{
    if (mbPluginsLoaded)
    {
        mbPluginsLoaded = false;
        //yaget::managed::UnloadPlugins(kEditorPackageName);
    }
}


IPluginObject::eInitReturnCode EditorFrame::OnInit(int32_t initPass)
{
    if (initPass == -1)
    {
        Message msg(mtype::kAllowTools);
        msg.Send();
        if (msg.GetValue<bool>() == false)
        {
            // we are forbidden to operate
            return IPluginObject::ircUnload;
        }
									   
		//yaget::log::Activate();
		//! This allows us to communicate between c++ and csharp for konsole
		if (eg::config::Konsole *konsole = eg::registrate::p_cast<eg::config::Konsole>("Konsole"))
		{
			mKonsoleConnection = konsole->sigCommandEcho.connect(&PrintKonsoleOutput);
		}

        wxLogTrace(TR_EDITOR, "EditorFrame Plugin Initialized at Pass %d.", initPass);
    }

    return IPluginObject::ircDone;
}


void EditorFrame::loadPluginTools()
{
    std::string pluginPath = registrate::GetExecutablePath();
    std::vector<std::string> pluginFileNames;

    wxArrayString fileNames;
    // yep - (y)aget(e)ditor(p)lugin
    wxDir::GetAllFiles(pluginPath, &fileNames, wxT("*.yep"), wxDIR_FILES);
    for (size_t i = 0; i < fileNames.size(); i++)
    {
        if (wxPluginLibrary *pPlugin = wxPluginManager::LoadLibrary(fileNames[i], wxDL_VERBATIM|wxDL_NOW))
        {
            pluginFileNames.push_back((const char *)fileNames[i].c_str());
            mPluginTools.push_back(pPlugin);
        }
    }

    //yaget::managed::LoadPlugins(pluginFileNames, kEditorPackageName);
}


void EditorFrame::onInput(const std::string& actionName, uint32_t /*timeStamp*/, uint32_t /*mouseX*/, uint32_t /*mouseY*/)
{
    if (actionName == kToggleEditorView)
    {
        //mActionShowEditor.Clear();
        // only load first time
        // load all C# plugins, where loading plugin dll is controlled by file extension '.met'
        if (!mbPluginsLoaded)
        {
            loadPluginTools();
        }
        else
        {
            //yaget::managed::UnloadPlugins(kEditorPackageName);
        }

        mbPluginsLoaded = !mbPluginsLoaded;
    }
}


const std::string& EditorFrame::GetName() const
{
    static std::string staticName("EditorFrame");
    return staticName;
}


const Version EditorFrame::GetVersion() const
{
    return YagetVersion;
}

} // namespace eg

