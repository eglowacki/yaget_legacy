#include "Precompiled.h"
#pragma warning (disable : 4389) // '!=' : signed/unsigned mismatch
#include "ComponentScriptLua.h"
#pragma warning (default : 4389)
#include "Script.h"
#include "Config/ConfigHelper.h"
#include "Plugin/ModuleHelper.h"
#include "binds/BindPolicy.h"
#include "luacpp.h"
#include <wx/confbase.h>
#include <wx/statline.h>
#include <wx/thread.h>
#include <wx/filename.h>
#include <wx/regex.h>
#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>


namespace
{
    using namespace eg;
    AutoRegisterAssetFactory<ScriptAsset> myFactory;

    const std::string kExt(".lua");

    //! send after compilation of script code is done
    // and there is new request for creation of new Command
    // boost:any
    const guid_t kNewScriptCommand = 0x949a6f03;

    //! Used to lock any compiler operations
    wxCriticalSection CompilerCritSect;

    //! called when calling lua script chunk and any error occurred
    //! This will format error string full_path_file_name(line_number) : error blah blah
    int luaError(lua_State *L)
    {
        lua_Debug d;
        lua_getstack(L, 1, &d);
        lua_getinfo(L, "Sln", &d);
        std::string err = lua_tostring(L, -1);
        lua_pop(L, 1);
        std::stringstream msg;
        msg << d.source << "(" << d.currentline << ") : error";
        //msg << d.short_src << ":" << d.currentline;

        if (d.name != 0)
        {
            msg << "(" << d.namewhat << " " << d.name << ")";
        }
        msg << " " << err;
        lua_pushstring(L, msg.str().c_str());
        return 1;
    }

} // namespace

namespace eg {

EG_MODULE_REGISTER_COMPONENT(ComponentScriptLua);


ComponentScriptLua::ComponentScriptLua(component::ObjectId oId)
: ComponentHelper(oId, script::kGroup, ComponentHelper::ccAttached)
, mPackageName(get_prop<File_Tag, std::string>(mProperty, script::kPropPackageName, "", "{ext = *.lua; type = lua; multi = true}"))
, mCommandNames(get_propV<std::vector<std::string> >(mProperty, script::kPropCommandNames, std::vector<std::string>(), ""))
, mLuaThread(0)
, mCurrentExecutionState(sesEmpty)
, mScriptRef(0)
{
    Dispatcher& disp = REGISTRATE(Dispatcher);
    mNewScriptCommandSlot = disp[kNewScriptCommand].connect(boost::bind(&ComponentScriptLua::newScriptCommand, this, _1));
}


ComponentScriptLua::~ComponentScriptLua()
{
    mPackageLoadedSlot.disconnect();
    mPackage.Sync();

    mNewScriptCommandSlot.disconnect();
}


void ComponentScriptLua::DeleteThis()
{
    delete this;
}


void ComponentScriptLua::Shutdown()
{
    clearScriptData();
}


Dispatcher::Result_t ComponentScriptLua::HandleMessage(Message& msg)
{
    // we just pass all messages to all registered components
    for (std::vector<script::Component *>::iterator it = mScriptComponents.begin(); it != mScriptComponents.end(); ++it)
    {
        (*it)->onMessage(msg);
    }
}


void ComponentScriptLua::createCommand(const std::string& commandName, Parameters_t parameters)
{
	asset::TryLocker<ScriptAsset> assetLocker(mPackage());

	if (assetLocker)
	{
		wxCriticalSectionLocker CompilerLocker(CompilerCritSect);
		if (mLuaThread && mScriptRef)
		{
			int status = 0;
			ScriptExecutionState sesState = loadScript(commandName);
			if (sesState == sesLoaded)
			{
				int argc = 0;
				//! allow to add any parameters to function call
				if (!commandName.empty() && parameters)
				{
					argc = parameters(mLuaThread, commandName);
				}
				try
				{
					status = lua_pcall(mLuaThread, argc, 0, 0);
				}
				catch (const luabind::error& ex)
				{
					status = -1;
					wxLogError("Calling createCommand [%s] failed. Exception: '%s'", commandName, ex.what());
				}
			}
			else if (sesState == sesYelding)
			{
				status = lua_resume(mLuaThread, 0);
			}
			else if (sesState == sesError)
			{
				// there was an error executing script
				// so the only way to fix it is to recompile
				mScriptRef = 0;
			}

			// this value is set from within script
			// and if it's yield don't change it,
			// otherwise reset to empty so we reload the script
			// on the next iteration if needed
			if (mCurrentExecutionState != sesYelding)
			{
				mCurrentExecutionState = sesEmpty;
			}
			if (status)
			{
				// let's remove any local created components if there is any kind of script error
				clearScriptData();
				mScriptRef = 0;

				const char* msg = lua_tostring(mLuaThread, -1);
				lua_pop(mLuaThread, 1);
				std::string packageFileName = mPackageName;
				wxLogError("Script '%s' executing command '%s' encountered error: '%s'.", packageFileName.c_str(), commandName.c_str(), msg);
			}
		}
	}
}


bool ComponentScriptLua::activateCommand(const std::string& commandName)
{
    if (!commandName.empty())
    {
		std::string funcName = IScript::FindFunctionName(commandName);

        lua_getglobal(mLuaThread, funcName.c_str());
        if (lua_isfunction(mLuaThread, -1))
        {
            mCurrentExecutionState = sesLoaded;
            return true;
        }
    }

    return false;
}


ComponentScriptLua::ScriptExecutionState ComponentScriptLua::loadScript(const std::string& commandName)
{
    if (activateCommand(commandName))
    {
        return mCurrentExecutionState;
    }

    // if current execution of state is yielding, then we do not need to do anything
    // if we are sesEmpty, then reload the script
    if (mCurrentExecutionState != sesEmpty)
    {
        return mCurrentExecutionState;
    }

    lua_rawgeti(mLuaThread, LUA_REGISTRYINDEX, mScriptRef);
    mCurrentExecutionState = sesLoaded;

    // this is might be first load so we again need to check for function execution
    bool result = activateCommand(commandName);
    if (!commandName.empty() && !result)
    {
        // we could not execute command
        mCurrentExecutionState = sesError;
    }
    return mCurrentExecutionState;
}


void ComponentScriptLua::newScriptCommand(Message& msg)
{
    using namespace component;
    if (msg.Is<ObjectId>())
    {
        ObjectId oId = msg.GetValue<ObjectId>();
        if (oId == OI().Id)
        {
            if (initializeScripts())
            {
                std::vector<std::string> commands = mCommandNames;
                for (std::vector<std::string>::const_iterator it = commands.begin(); it != commands.end(); ++it)
                {
					// let's get Parameters structure for this function
					IScript& script = registrate::ref_cast<IScript>("IScript");
					Parameters_t params = (dynamic_cast<Script&>(script)).GetFunctionParameters(*it);

                    createCommand(*it, params);
                }
            }
        }
    }
}


bool ComponentScriptLua::initializeScripts()
{
    int result = 0;

    asset::TryLocker<ScriptAsset> scriptAsset(mPackage());
    if (scriptAsset)
    {
        if (scriptAsset->empty())
        {
            // There is no script source associated with
            // this asset, so just skip it
            mScriptRef = 0;
            return false;
        }
        // now load file into our lua state thread
        bool bLoadError = false;
        ScriptAsset::Data_t scriptData;
        if (config::ReadBool("Debug/Script/Debug"))
        {
            // this is used in debugger so we can load our source file into
            for (ScriptAsset::const_iterator it = scriptAsset->begin(); it != scriptAsset->end(); ++it)
            {
                ScriptAsset::Data_t scriptData = *it;
                result = luaL_loadfile(mLuaThread, scriptData.second.c_str());
                if (result)
                {
                    bLoadError = true;
                    break;
                }

                int base = lua_gettop(mLuaThread);
                lua_pushcfunction(mLuaThread, luaError);
                lua_insert(mLuaThread, base);
                result = lua_pcall(mLuaThread, 0, 0, base);
                lua_remove(mLuaThread, base);

                if (result)
                {
                    break;
                }
            }
        }
        else
        {
            for (ScriptAsset::const_iterator it = scriptAsset->begin(); it != scriptAsset->end(); ++it)
            {
                scriptData = *it;
                wxFileName fileObject(scriptData.second.c_str());
                std::string fileName = scriptData.second;
                result = luaL_loadbuffer(mLuaThread, scriptData.first.c_str(), scriptData.first.size(), fileName.c_str());//OI().Name.c_str());
                if (result)
                {
                    bLoadError = true;
                    break;
                }

                int base = lua_gettop(mLuaThread);
                lua_pushcfunction(mLuaThread, luaError);
                lua_insert(mLuaThread, base);
                result = lua_pcall(mLuaThread, 0, 0, base);
                lua_remove(mLuaThread, base);

                if (result)
                {
                    break;
                }
            }
        }

        if (result == 0)
        {
            // let's save our compiled script, so we can re-use it
            mScriptRef = luaL_ref(mLuaThread, LUA_REGISTRYINDEX);
            lua_settop(mLuaThread, 0);
        }
        else
        {
            const char* msg = lua_tostring(mLuaThread, -1);
            lua_pop(mLuaThread, 1);
            std::string packageFileName = mPackageName;
            // if error scriptData will contain the last compiled script
            std::string fqn = scriptData.second;
            if (bLoadError)
            {
                int lineNumber = 1;
                wxString convertedtextData = msg;
                wxRegEx regKey = "\\[.+\\]:";
                if (regKey.Matches(convertedtextData))
                {
                    size_t start, len;
                    if (regKey.GetMatch(&start, &len))
                    {
                        wxRegEx regNumber = "[[:digit:]]+";
                        wxString number = &msg[len];
                        if (regNumber.Matches(number))
                        {
                            wxString lineName = regNumber.GetMatch(number);
                            try
                            {
                                lineNumber = boost::lexical_cast<int>(lineName.c_str());
                            }
                            catch (const boost::bad_lexical_cast& ex)
                            {
                                wxLogWarning("Could not convert line number in Package '%s'. Error: %s", fqn.c_str(), ex.what());
                            }
                        }
                    }
                }

                wxLogError("Compile error for Package '%s' Script '%s':\n%s(%d) : %s", packageFileName.c_str(), fqn.c_str(), fqn.c_str(), lineNumber, msg);
            }
            else
            {
                wxLogError("Compile error for Package '%s' Script '%s':\n%s", packageFileName.c_str(), fqn.c_str(), msg);
            }
        }
    }
    else
    {
        std::string packageName = mPackageName;
        wxLogError("Could not lock Package Asset '%s' to initialize scripts.", packageName.c_str());
    }

    return result == 0;
}


void ComponentScriptLua::onPropertyChanged(const IProperty *pProperty)
{
    if (pProperty->GetPropId() == mPackageName.GetPropId())
    {
        std::string packageFileName = mPackageName;
        // let's assure that we have correct extension supplied
        if (!boost::iends_with(packageFileName, kExt))
        {
            packageFileName += kExt;
        }

        // remove load finish callback, since we are going to add again below
        mPackageLoadedSlot.disconnect();

        // This will make sure that if there is any outstanding
        // script to compile, it will be finished before we proceed any further.
        // What that means is in some circumstances, new compile request
        // will wait until previews operation is completely finishes.
        // This potentially can introduce a wait situation here, but to try to interrupt
        // and remove previous script object which is still loading produces some architectural
        // and complexity difficulties which are north worth to tackle.
        mPackage.Sync();

        // it is useful to know when old dat is going away

        // this will trigger loading of script file and compile.
        mPackage = packageFileName;
        if (ScriptAsset::Asset_t asset = mPackage())
        {
            mPackageLoadedSlot = asset->AddLoadFinishSlot(boost::bind(&ComponentScriptLua::onScriptLoaded, this, _1, _2));
        }
    }
    else if (pProperty->GetPropId() == mCommandNames.GetPropId())
    {
        if (mScriptRef)
        {
            std::vector<std::string> commands = mCommandNames;
            for (std::vector<std::string>::const_iterator it = commands.begin(); it != commands.end(); ++it)
            {
				// let's get Parameters structure for this function
				IScript& script = registrate::ref_cast<IScript>("IScript");
				Parameters_t params = (dynamic_cast<Script&>(script)).GetFunctionParameters(*it);

                createCommand(*it, params);
            }
        }
    }
}


void ComponentScriptLua::onScriptLoaded(ScriptAsset::Asset_t asset, const std::string& /*name*/)
{
    if (ScriptAsset::Asset_t scriptAsset = mPackage())
    {
        // scriptAsset should be the same as asset
        wxASSERT(scriptAsset == asset);
        if (IAsset::Locker_t locker = scriptAsset->Lock())
        {
            wxCriticalSectionLocker CompilerLocker(CompilerCritSect);
            mCurrentExecutionState = sesEmpty;
            // \note How about compiling lua script here and load into lua state.
            // We need to have critical sections here as to only allow one thread to access lua run-time
            if (!mLuaThread)
            {
                mLuaThread = lua_newthread(Script::L);
                // save a pointer to the thread manager object in the global table
                // using the new thread's vm pointer as a key
                lua_pushlightuserdata(Script::L, mLuaThread);
                lua_pushlightuserdata(Script::L, this);
                lua_settable(Script::L, LUA_GLOBALSINDEX);
            }

            // let's remove any local created components
            clearScriptData();

            if (mScriptRef)
            {
                // unreferenced older chunk ref
                luaL_unref(mLuaThread, LUA_REGISTRYINDEX, mScriptRef);
                mScriptRef = 0;
            }

            // clear stack
            lua_settop(mLuaThread, 0);
        }
    }

    // here we need to trigger one time callback so we can create new commands
    // in mCmdName property
    // and simplest think could be triggering onPropertyChanged callback on main thread
    Message(kNewScriptCommand, OI().Id).Post();
}


Hash ComponentScriptLua::GetComponentTypeId() const
{
    return ComponentType::kScript;
}


bool ComponentScriptLua::RegisterComponentType()
{
    InitComponentType(ComponentType::kScript, wxT("Script"));
    return true;
}


bool ComponentScriptLua::script_newc(const component::ComponentId& cId)
{
    component::InstanceId iId(OI().Id, cId);
    if (IComponent *pComponent = component::newc("", iId))
    {
        mNewedObjects.insert(iId);
        return true;
    }

    wxLogError("Could not create component %s.", component::instanceName(iId).c_str());
    return false;
}


bool ComponentScriptLua::script_newc(const component::InstanceId& iId, const std::string& name)
{
    if (IComponent *pComponent = component::newc(name, iId))
    {
        mNewedObjects.insert(iId);
        return true;
    }

    wxLogError("Could not create component %s.", component::instanceName(iId).c_str());
    return false;
}


void ComponentScriptLua::script_freec(const component::ComponentId& cId)
{
    component::InstanceId iId(OI().Id, cId);
    script_freec(iId);
}


void ComponentScriptLua::script_freec(const component::InstanceId& iId)
{
    std::set<component::InstanceId>::iterator it = mNewedObjects.find(iId);
    if (it != mNewedObjects.end())
    {
        component::freec(iId);
        mNewedObjects.erase(it);
    }
    else
    {
        wxLogError("Instance %s not owned by this ScriptComponent '%s'.", component::instanceName(iId).c_str(), OI().Name.c_str());
    }
}


void ComponentScriptLua::clearScriptData()
{
    std::for_each(mScriptComponents.begin(), mScriptComponents.end(), boost::bind(&script::Component::Finalize, _1));
    mScriptComponents.clear();

    std::for_each(mNewedObjects.begin(), mNewedObjects.end(), component::freec);
    mNewedObjects.clear();
}


void ComponentScriptLua::script_AddComponent(script::Component *pScriptComponent)
{
    mScriptComponents.push_back(pScriptComponent);
}

void ComponentScriptLua::script_RemoveComponent(script::Component *pScriptComponent)
{
    std::vector<script::Component *>::iterator it = std::find(mScriptComponents.begin(), mScriptComponents.end(), pScriptComponent);
    if (it != mScriptComponents.end())
    {
        (*it)->Finalize();
        mScriptComponents.erase(it);
    }
}

} // namespace eg
