#include "Precompiled.h"
#include "Script.h"
#include "ComponentEnumsScript.h"
//#include "Config/ConfigHelper.h"
//#include "IObjectManager.h"
#include "MessageInterface.h"
//#include "Input/InputManager.h"
//#include "File/VirtualFileSystem.h"
#include "YagetVersion.h"
#include <wx/confbase.h>
#include <wx/dynload.h>
//#include <wx/dir.h>
//#include <wx/filename.h>
//#include <boost/bind.hpp>
//#include <boost/algorithm/string.hpp>
#include "luacpp.h"
#include <luabind/luabind.hpp>


namespace
{

	//! Helper function to return just parameters portion (if one exists)
	//! of a function signature
	std::string findParameters(const std::string& functionSignature)
	{
		size_t pos = functionSignature.find_first_of('(');
		if (pos != std::string::npos)
		{
			// move pass '('
			pos++;
			// there is opening parenthesis, which means
			// separation between function name and parameters
			size_t size = functionSignature.size();
			std::string parameters = functionSignature.substr(pos, (size-1)-pos);
			return parameters;
		}

		return "";
	}

	//! Parameter signature which houses methods
	//! to convert
	struct ParamSigBool
	{
		int operator()(lua_State *L, const std::string& functionName)
		{
			std::string parameters = findParameters(functionName);
			if (!parameters.empty())
			{
				try
				{
					bool value = boost::lexical_cast<bool>(parameters);
					lua_pushboolean(L, value);
					return 1;
				}
				catch (const boost::bad_any_cast& /*ex*/)
				{
					wxLogError("Could not cast function name [%s] to bool type", functionName);
				}
			}

			return 0;
		}
	};

	struct ParamSigNumber
	{
		int operator()(lua_State *L, const std::string& functionName)
		{
			std::string parameters = findParameters(functionName);
			if (!parameters.empty())
			{
				try
				{
					double value = boost::lexical_cast<double>(parameters);
					lua_pushnumber(L, value);
					return 1;
				}
				catch (const boost::bad_any_cast& /*ex*/)
				{
					wxLogError("Could not cast function name [%s] to double type", functionName);
				}
			}

			return 0;
		}
	};

	struct ParamSigString
	{
		int operator()(lua_State *L, const std::string& functionName)
		{
			std::string parameters = findParameters(functionName);
			lua_pushstring(L, parameters.c_str());
			return 1;
		}
	};

} // namespace


namespace eg {
namespace script
{
    extern void ComponentBinds(lua_State *L);
} //namespace script


lua_State *Script::L = 0;



IMPLEMENT_USER_EXPORTED_PLUGGABLE_CLASS(Script, IScript)


Script::Script()
{
    wxLogTrace(TR_SCRIPT, "ScriptObject Created.");
	Dispatcher& disp = REGISTRATE(Dispatcher);
	disp[mtype::kShutdownBegin].connect(boost::bind(&Script::onShutdown, this, _1));
}


Script::~Script()
{
    if (Script::L)
    {
        lua_close(Script::L);
    }
    wxLogTrace(TR_SCRIPT, "ScriptObject Deleted.");
}


const std::string& Script::GetName() const
{
    static std::string staticName("Script");
    return staticName;
}


namespace script
{
    extern void RegisterAllDeferedModules();
} // namespace script

IPluginObject::eInitReturnCode Script::OnInit(int32_t initPass)
{
    if (initPass == -1)
    {
        // create lua state here,
        Script::L = luaL_newstate();
        if (!Script::L)
        {
            // there is no lua state, so we can not run any scripts,
            // so, we will not activate any of our components
            // but still return true, since there seem to be a bug
            // if we try to unload this dll right now
            return IPluginObject::ircError;
        }

        luabind::open(Script::L);
        luaL_openlibs(Script::L);
        script::ComponentBinds(L);

		registrate::RegisterObject(dynamic_cast<IScript *>(this), "IScript");
    }

    return IPluginObject::ircDone;
}


void Script::onShutdown(Message& /*msg*/)
{
	Dispatcher& disp = REGISTRATE(Dispatcher);
	disp[mtype::kShutdownBegin].disconnect(boost::bind(&Script::onShutdown, this, _1));
	registrate::RegisterObject(boost::any(), "IScript");
}


bool Script::RegisterFunction(const std::string& functionSignature)
{
	std::string funcName = FindFunctionName(functionSignature);
	std::string parameters = findParameters(functionSignature);
	if (!funcName.empty() && !parameters.empty())
	{
		Hash hash(funcName.c_str());
		// let's see if we already have this signature registered
		FunctionSignatures_t::const_iterator it = mFunctionSignatures.find(hash.GetValue());
		if (it != mFunctionSignatures.end())
		{
			// we already have it return it's id
			return true;
		}

		// now get parameters
		if (boost::iequals(parameters, "bool"))
		{
			mFunctionSignatures.insert(std::make_pair(hash.GetValue(), ParamSigBool()));
		}
		else if (boost::iequals(parameters, "int")
			|| boost::iequals(parameters, "float")
			|| boost::iequals(parameters, "number"))
		{
			mFunctionSignatures.insert(std::make_pair(hash.GetValue(), ParamSigNumber()));
		}
		else if (boost::iequals(parameters, "string"))
		{
			mFunctionSignatures.insert(std::make_pair(hash.GetValue(), ParamSigString()));
		}
		else
		{
			wxLogError("Function Signature [%s] registration is not supported", functionSignature);
			return false;
		}

		wxLogTrace("Function Signature [%s] with parameters [%s] registered.", funcName.c_str(), parameters.c_str());
		return true;
	}

	wxLogWarning("Function Signature [%s] does not have any valid parameters for registration.", functionSignature);
	return false;
}


ComponentScriptLua::Parameters_t Script::GetFunctionParameters(const std::string& functionSignature) const
{
	std::string funcName = FindFunctionName(functionSignature);
	if (!funcName.empty())
	{
		Hash hash(funcName.c_str());
		FunctionSignatures_t::const_iterator it = mFunctionSignatures.find(hash.GetValue());
		if (it != mFunctionSignatures.end())
		{
			// we have this function registered, return function object
			// which will handle conversions
			return (*it).second;
		}
	}

	return ComponentScriptLua::Parameters_t();
}


const Version Script::GetVersion() const
{
    return YagetVersion;
}


} // namespace eg

