///////////////////////////////////////////////////////////////////////
// EditorFrame.h
//
//  Copyright 3/30/2006 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//      Concrete implentation of a EditorFrame
//
//
//  #include "EditorFrame.h"
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef EDITOR_FRAME_H
#define EDITOR_FRAME_H

#include "IEditorFrame.h"
//#include "IRenderer.h"
#include "EditorBase.h"
#include "MessageInterface.h"
#include "Input/InputManager.h"
//#include <wx/aui/aui.h>
#include <vector>
#include <boost/shared_ptr.hpp>

class wxPluginLibrary;

namespace eg
{
    //! forward declaration
    class IMetPlugin;

    class EditorFrame : public IEditorFrame//, public boost::signals::trackable
    {
        DECLARE_USER_EXPORTED_PLUGGABLE_CLASS(EditorFrame, DLLIMPEXP_EDITORFRAME)

    public:
        EditorFrame();
        virtual ~EditorFrame();

        //! from IPluginObject
        virtual const std::string& GetName() const;
        virtual const Version GetVersion() const;

    private:
        virtual eInitReturnCode OnInit(int32_t initPass);
        //! Called by any new loaded tool
        void onNewTool(Message& msg);

        //! Callback for input
        void onInput(const std::string& actionName, uint32_t timeStamp, uint32_t mouseX, uint32_t mouseY);
        //! Helper method to load all available tools (all file with *.met extension (Muck Editor Tools)
        void loadPluginTools();

        //! respond to kShutdownBegin event
        void onShutdown(Message& msg);
        boost::signals::connection mShutdownConnection;

        void onEditorClosed(Message& msg);
        boost::signals::connection mEditorClosedConnection;

        typedef std::vector<wxPluginLibrary *> PluginLibraryList_t;
        PluginLibraryList_t mPluginTools;

        //! Used to show and hide entire editor gui
        InputAction mActionShowEditor;
        bool mbPluginsLoaded;
		boost::signals::connection mKonsoleConnection;
    };

} // namespace eg

#endif // EDITOR_FRAME_H

