///////////////////////////////////////////////////////////////////////
// Script.h
//
//  Copyright 06/01/2008 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//      Implementation of a Script class which handles of lua initializations
//      and setting any needed bindings
//
//
//  #include "Script.h"
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef SCRIPT_H
#define SCRIPT_H

//#include "Plugin/IPluginObject.h"
#include "Interface/IScript.h"
#include "ComponentScriptLua.h"
#include "ScriptModule.h"
#include <vector>
#include <string>
#include <map>

// forward declarations
struct lua_State;


namespace eg
{
    /*!
    Implementation of our Script object
    */
    class Script : public IScript
    {
        DECLARE_USER_EXPORTED_PLUGGABLE_CLASS(Script, DLLIMPEXP_SCRIPT)

    public:
        Script();
        virtual ~Script();

		// from IScript
		virtual bool RegisterFunction(const std::string& functionSignature);

        // from IPluginObject
        virtual const std::string& GetName() const;
        virtual const Version GetVersion() const;

		ComponentScriptLua::Parameters_t GetFunctionParameters(const std::string& functionSignature) const;

        //! Global Lua state
        static lua_State *L;

    private:
        // from IPluginObject
        virtual eInitReturnCode OnInit(int32_t initPass);

        void onShutdown(Message& msg);

		typedef std::map<uint64_t, ComponentScriptLua::Parameters_t> FunctionSignatures_t;
		FunctionSignatures_t mFunctionSignatures;
    };

} // namespace eg

#endif // SCRIPT_H


