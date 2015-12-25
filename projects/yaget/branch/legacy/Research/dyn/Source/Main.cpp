// Main.cpp
#include "Base.h"
#include "Plugin/IPluginObject.h"
#include "App/AppUtilities.h"
#include <string>
#include <boost/shared_ptr.hpp>
#include "vld.h"

using namespace eg;


namespace
{
    class DynPlugin : public IPluginObject
    {
    public:
        DynPlugin() : v(true) {}
        virtual ~DynPlugin()
        {
            v = false;
        }

        virtual const char *GetName() const {return "dyn";}
        virtual const Version GetVersion() const {return YagetVersion;}
        virtual void blah() {}

    private:
        bool v;

    };

    boost::shared_ptr<DynPlugin> dynPlugin;
}


extern "C" __declspec(dllexport) IPluginObject *get_plugin(const Version& version, const Environment *environment)
{
    version;environment;
    if (!dynPlugin)
    {
        dynPlugin.reset(new DynPlugin);
    }

    return dynPlugin.get();
}


extern "C" __declspec(dllexport) void free_plugin(IPluginObject *plugin)
{
    if (dynPlugin && dynPlugin->GetName() == plugin->GetName())
    {
        dynPlugin.reset();
    }
}
