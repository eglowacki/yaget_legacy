/* File : MessageBinds.i */
%module (imclassname = "ManagedMessage") EditorFrame 
%{
	#include "MessageInterface.h"
	#include "IEditorMessages.h"
	#include "Config/ConfigHelper.h"
    #include "Config/LogHelper.h"
	#include "Registrate.h"
    #include "LogDispatcher.h"
%}

%ignore eg::mtype::kShutdownBegin;
%ignore eg::mtype::kFrameTick;
%ignore eg::mtype::kKonsolePrint;
%ignore eg::mtype::kObjectNew;
%ignore eg::mtype::kObjectDelete;
%ignore eg::mtype::kComponentNew;
%ignore eg::mtype::kComponentDelete;
%ignore eg::mtype::kComponentSelected;
%ignore eg::mtype::kComponentDeselected;
%ignore eg::mtype::kViewSelectedObject;
%ignore eg::mtype::kFrameTickRefresh;
%ignore eg::mtype::kYepClosed;


%ignore yaget::mtype::kLogDispatcher;

%include "std_string.i"
%include "MessageInterface.h"
%include "IEditorMessages.h"
%include "../LogDispatcher.h"

%inline 
%{
	namespace yaget
	{
		std::string ReadPath(const std::string& keyName)
		{
			return eg::config::ReadFullPath(keyName);
		}

		bool ReadBool(const std::string& keyName, bool defaultValue)
		{
			return eg::config::ReadBool(keyName.c_str(), defaultValue);
		}
		
        std::string GetExecutablePath()
        {
			return eg::registrate::GetExecutablePath();
        }
        
        std::string GetAppName()
        {
			return eg::registrate::GetAppName();
        }
        
        std::string GetLogBufferText()
        {
			return eg::LogBufferText();
        }
	}
%}

%constant unsigned int kShutdownBegin = eg::mtype::kShutdownBegin;
%constant unsigned int kFrameTick = eg::mtype::kFrameTick;
%constant unsigned int kLogDispatcher = yaget::mtype::kLogDispatcher;
%constant unsigned int kKonsolePrint = eg::mtype::kKonsolePrint;
%constant unsigned int kObjectNew = eg::mtype::kObjectNew;
%constant unsigned int kObjectDelete = eg::mtype::kObjectDelete;
%constant unsigned int kComponentNew = eg::mtype::kComponentNew;
%constant unsigned int kComponentDelete = eg::mtype::kComponentDelete;
%constant unsigned int kComponentSelected = eg::mtype::kComponentSelected;
%constant unsigned int kComponentDeselected = eg::mtype::kComponentDeselected;
%constant unsigned int kViewSelectedObject = eg::mtype::kViewSelectedObject;
%constant unsigned int kFrameTickRefresh = eg::mtype::kFrameTickRefresh;
%constant unsigned int kYepClosed = eg::mtype::kYepClosed;

