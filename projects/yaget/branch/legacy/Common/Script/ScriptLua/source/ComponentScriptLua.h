///////////////////////////////////////////////////////////////////////
// ComponentScriptLua.h
//
//  Copyright 05/30/2008 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//      Private implementation of ComponentScript model
//
//
//  #include "ComponentScriptLua.h"
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef COMPONENT_SCRIPT_LUA_H
#define COMPONENT_SCRIPT_LUA_H

#include "IComponentScript.h"
#include "Property/PropertyData.h"
#include "Message/Dispatcher.h"
#include "ScriptAsset.h"
#include "Input/InputManager.h"
#include "binds/ComponentBinds.h"
#include <set>

// forward declarations
struct lua_State;


namespace eg
{
    //! Component for wrapping Lua scrips
    class ComponentScriptLua : public IComponentScript, public ComponentHelper<ComponentScriptLua, PropertySet>
    {
    public:
        ComponentScriptLua(component::ObjectId oId);
        virtual ~ComponentScriptLua();

		//! Used to get function object from Script per specific function signature
		typedef boost::function<int (lua_State * /*L*/, const std::string& /*functionName*/)> Parameters_t;

        //! These methods are used by script
        //! Add new component to this object,
        //! \note any added components will be removed when
        //! this object goes away, or script gets reloaded.
        //! \return bool true if component was created otherwise false
        bool script_newc(const component::ComponentId& cId);
        void script_freec(const component::ComponentId& cId);
        void script_freec(const component::InstanceId& iId);

        //! Add new component to object id with name. If name == ""
        //! then object retains existing name
        //! \note any added components will be removed when
        //! this object goes away, or script gets reloaded.
        //! \return bool true if component was created otherwise false
        bool script_newc(const component::InstanceId& iId, const std::string& name);

        //! Add created script component
        //! This object does not have an ownership of it, only it
        //! notifies pScriptComponent when script file is deleted
        void script_AddComponent(script::Component *pScriptComponent);
        void script_RemoveComponent(script::Component *pScriptComponent);

        // from IComponent
        virtual void DeleteThis();
        virtual Hash GetComponentTypeId() const;
        virtual Dispatcher::Result_t HandleMessage(Message& msg);
        virtual IPropertySet& GetPropertyData() {return CH_t::GetPropertyData();}
        virtual const ObjectInstance& OI() const {return CH_t::OI();}

        // Static methods
        static bool RegisterComponentType();

    private:
        // from ComponentHelper
        virtual void onPropertyChanged(const IProperty *pProperty);

        void onScriptLoaded(ScriptAsset::Asset_t asset, const std::string& name);
        void newScriptCommand(Message& msg);
        void createCommand(const std::string& commandName, Parameters_t parameters);
        //! Called when all scripts are loaded
        bool initializeScripts();

        //! This will unregister messages and remove any newed object
        void clearScriptData();

        // from IComponent
        virtual void Shutdown();

        enum ScriptExecutionState
        {
            sesEmpty,   ///< no chunk on the stack, we need to reload it
            sesLoaded,  ///< chunk is ready to go, use lua_pcall
            sesYelding, ///< script is in yield mode, use lua_resume
            sesError    ///< there was an script error.
        };

        ScriptExecutionState loadScript(const std::string& commnad);
        bool activateCommand(const std::string& commnad);

        ScriptExecutionState mCurrentExecutionState;

        //! this can represent one script file or include filter characters
        Property<std::string>& mPackageName;
        Property<std::vector<std::string> >& mCommandNames;

        ScriptAsset_t mPackage;
        boost::signals::connection mPackageLoadedSlot;
        // Connection 'New Command' event
        boost::signals::connection mNewScriptCommandSlot;

        // lua thread for this script
        lua_State *mLuaThread;
        volatile int mScriptRef;

        //! Any component created by calling newc (both variants) will be stored here
        std::set<component::InstanceId> mNewedObjects;

        //! Created script components
        std::vector<script::Component *> mScriptComponents;
    };

} // namespace eg

#endif // COMPONENT_SCRIPT_LUA_H

