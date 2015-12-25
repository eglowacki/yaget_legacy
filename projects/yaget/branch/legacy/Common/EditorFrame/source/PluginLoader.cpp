#if 0
#include "PluginLoader.h"
//#include "ObjectID.h"

//#using <System.dll>
//#using <System.Drawing.dll>
//#using <System.Windows.Forms.dll>

//#include "IPlugin.h"

//using namespace System;
//using namespace System::Collections::Generic;
//using namespace System::Reflection;


/*
namespace
{
    public ref class LoadedPackages
    {
    public:
        static Dictionary<String^, List<yaget::managed::IPlugin^>^>^ plugins = gcnew Dictionary<String^, List<yaget::managed::IPlugin^>^>();
    };
} // namespace
*/


namespace yaget {
namespace managed {

void UnloadPlugins(const std::string& packageName)
{
    /*
    List<IPlugin^>^ package = gcnew List<IPlugin^>();
    if (LoadedPackages::plugins->TryGetValue(stos(packageName), package))
    {
        try
        {
            for each (IPlugin^ plugin in package)
            {
                plugin->Close();
            }
        }
        catch(Exception^ e)
        {
            String^ msg = "(Plugin) Exception " + e->ToString() + " unloading.";
            msg;
        }

        LoadedPackages::plugins->Remove(stos(packageName));

		GC::Collect();
		GC::WaitForPendingFinalizers();
    }
    */
}


bool LoadPlugins(const std::vector<std::string>& pluginNameList, const std::string& packageName)
{
    /*
    // first just create new instance of each requested plugin object
    // those plugin objects need to be derived from managed::IPlugin
    List<IPlugin^>^ pluginList = gcnew List<IPlugin^>();
    for (std::vector<std::string>::const_iterator it = pluginNameList.begin(); it != pluginNameList.end(); ++it)
    {
        String^ pathName = stos(*it);
        try
        {
            Assembly^ a = Assembly::LoadFrom(pathName);
            Type^ pluginType = IPlugin::typeid;
            array<Type^>^ types =  a->GetTypes();
            for each (Type^ type in types)
            {
                if (pluginType->IsAssignableFrom(type))
                {
                    // we have our interface here, so let's create an object
                    if (Object^ obj = Activator::CreateInstance(type))
                    {
                        if (IPlugin^ pPluginPanel = dynamic_cast<IPlugin^>(obj))
                        {
                            pluginList->Add(pPluginPanel);
                        }
                    }
                }
            }
        }
        catch (Exception^ e)
        {
            String^ msg = "(Plugin) Exception " + e->ToString() + " loading " + pathName + " ignoring\n";
            msg;
            //Log::LogError(msg);
        }
    }

    LoadedPackages::plugins->Add(stos(packageName), pluginList);
    */
    return true;
}

} // namespace managed
} // namespace yaget
#endif // 0