#include "Precompiled.h"
#include "EditorPlugin.h"
#include "Logger/Log.h"
#include "App/AppUtilities.h"
#include "Script/ScriptAsset.h"
#include "Plugin/PluginManager.h"
#include "File/file_util.h"
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>
#include "vld.h"

using namespace eg;

std::vector<std::string> getTraceModules()
{
    std::vector<std::string> traceModules;
    traceModules.push_back(tr_editor);
    return traceModules;
}

std::string getLogName()
{
    return util::getAppFullPath() + ".editor.log";
}
logs::LoggerInit loggerInit(getLogName(), logs::kDest_file|logs::kDest_debug, 0, true, getTraceModules());

namespace
{
    boost::shared_ptr<EditorPlugin> editorPlugin;

    typedef void (*SharpGlueLoader)(const char * /*name*/);
    SharpGlueLoader sharpGlueLoader = 0;

    typedef void (*SharpGlueUnloader)(const char * /*name*/);
    SharpGlueUnloader sharpGlueUnloader = 0;

    typedef void (*SharpGlueLister)(const char * /*name*/, char * /*validNames*/, int * /*size*/);
    SharpGlueLister sharpGlueLister = 0;

} // namespace



namespace eg {
namespace script
{
    extern void init_editor(lua_State * /*L*/, EditorPlugin * /*edp*/);
} // namespace script

EditorPlugin::EditorPlugin(IAsset::Token script)
{
    log_trace(tr_editor) << "EditorPlugin object created.";

    ScriptAsset::RegisterBinds(script, boost::bind(&script::init_editor, _1, this));
}


EditorPlugin::~EditorPlugin()
{
    log_trace(tr_editor) << "EditorPlugin object deleted.";
}


const Version EditorPlugin::GetVersion() const
{
    return YagetVersion;
}


bool EditorPlugin::Load(const std::string& name)
{
    if (sharpGlueLoader)
    {
        std::string filePath = util::getAppPath() + "/" + name;
        sharpGlueLoader(filePath.c_str());
        return true;
    }

    return false;
}


std::vector<std::string> EditorPlugin::List(const std::string& name) const
{
    std::vector<std::string> pluginList;
    if (sharpGlueLister)
    {

        std::vector<std::string> file_names;
        std::string filePath = util::getAppPath() + "/";// + name;
        if (name.empty())
        {
            struct dllFilter
            {
                std::string operator()(const std::string& fileName) const
                {
                    if (boost::algorithm::iends_with(fileName, ".dll"))
                    {
                        return boost::algorithm::ierase_last_copy(fileName, ".dll");
                    }

                    return "";
                }
            };
            file::get_all_files(filePath, file_names, dllFilter());
        }

        if (file_names.empty())
        {
            file_names.push_back(filePath + name);                                                    
        }

        char validNames[256*1000] = {0};
        int size = 256*1000;
        int currIndex = 0;

        BOOST_FOREACH(const std::string& fileName, file_names)
        {
            sharpGlueLister(fileName.c_str(), validNames, &size);

            while (currIndex < size)
            {
                boost::filesystem::path path(fileName);
                std::string baseDllName = path.filename();
                std::string pluginName = &validNames[currIndex];
                pluginList.push_back(baseDllName + "." + pluginName);
                currIndex += pluginName.size() + 1;
            }
        }
    }

    return pluginList;
}


bool EditorPlugin::Unload(const std::string& name)
{
    if (sharpGlueUnloader)
    {
        std::string filePath = util::getAppPath() + "/" + name;
        sharpGlueUnloader(filePath.c_str());
        return true;
    }

    return false;
}


namespace editor { extern void bind_vfs(const Environment * /*environment*/); }

} // namespace eg


//-------------------------------------------------------------------------------------
// yaget plugin exported functions
extern "C" __declspec(dllexport) EditorPlugin *get_plugin(const Version& /*version*/, const Environment *environment)
{
    //version;environment;
    if (!editorPlugin)
    {
        PluginManager& loader = environment->get_ref<PluginManager>("loader");
        sharpGlueLoader = loader.f_cast<SharpGlueLoader>("SharpGlue", "load");
        sharpGlueUnloader = loader.f_cast<SharpGlueUnloader>("SharpGlue", "unload");
        sharpGlueLister = loader.f_cast<SharpGlueLister>("SharpGlue", "lister");

        IAsset::Token script = environment->get<IAsset::Token>("Script");
        eg::editor::bind_vfs(environment);
        editorPlugin.reset(new EditorPlugin(script));
    }

    return editorPlugin.get();
}


extern "C" __declspec(dllexport) void free_plugin(IEditorPlugin *plugin)
{
    if (editorPlugin.get() == plugin)
    {
        sharpGlueLoader = 0;
        editorPlugin.reset();
        eg::editor::bind_vfs(0);
    }
    else
    {
        log_error << "Could not free plugin '" << plugin->GetName();
    }
}
