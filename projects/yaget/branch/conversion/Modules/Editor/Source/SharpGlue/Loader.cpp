#include <string>
using namespace System;
using namespace System::Collections::Generic;
using namespace System::Reflection;

namespace managed
{
    inline String^ stos(const char *text)
    {
        String^ newText = gcnew String(text);
        return newText;
    }

    //-------------------------------------------------------------------------------------
    inline const std::string stos(String^ text)
    {
        std::string newText;
        using namespace System::Runtime::InteropServices;
        const char* chars = (const char*)(Marshal::StringToHGlobalAnsi(text)).ToPointer();
        newText = chars;
        Marshal::FreeHGlobal(System::IntPtr((void*)chars));
        return newText;
    }

    public ref class IPlugin abstract
    {
    };

    ref class Holder
    {
    public:
        static List<managed::IPlugin^>^ pluginList = gcnew List<managed::IPlugin^>();
    };

} // namespace managed

int sharp_Loader(const char *name)
{
    using namespace managed;
    try
    {
        String^ filePath = stos(name) + ".dll";
        Assembly^ a = Assembly::LoadFrom(filePath);
        Type^ pluginType = managed::IPlugin::typeid;
        array<Type^>^ types =  a->GetTypes();
        for each (Type^ type in types)
        {
            if (pluginType->IsAssignableFrom(type))
            {
                // we have our interface here, so let's create an object
                if (Object^ obj = Activator::CreateInstance(type))
                {
                    if (managed::IPlugin^ pPluginPanel = dynamic_cast<managed::IPlugin^>(obj))
                    {
                        Holder::pluginList->Add(pPluginPanel);
                    }
                }
            }
        }
    }
    catch (Exception^ e)
    {
        String^ msg = "(Plugin) Exception " + e->ToString() + " loading " + stos(name) + " ignoring\n";
        msg;
        //Log::LogError(msg);
    }

    return 0;
}


int sharp_Lister(const char *name, char *validNames, int *size)
{
    using namespace managed;
    int maxSize = *size;
    *size = 0;
    try
    {
        String^ filePath = stos(name) + ".dll";
        Assembly^ a = Assembly::LoadFrom(filePath);
        Type^ pluginType = managed::IPlugin::typeid;
        array<Type^>^ types =  a->GetTypes();
        for each (Type^ type in types)
        {
            if (pluginType->IsAssignableFrom(type))
            {
                std::string validPluginName = stos(type->ToString());

                if (validNames)
                {
                    if (*size + validPluginName.size() >= maxSize)
                    {
                        // we don't have enough room in memory buffer
                        // to copy next plugin name
                        return -1;
                    }

                    std::copy(validPluginName.begin(), validPluginName.end(), &validNames[*size]);
                    *size += validPluginName.size();
                    validNames[*size] = '\0';
                    (*size)++;
                }
                else
                {
                    *size += validPluginName.size()+1;
                }
            }
        }
    }
    catch (Exception^ e)
    {
        String^ msg = "(Plugin) Exception " + e->ToString() + " loading " + stos(name) + " ignoring\n";
        msg;
        //Log::LogError(msg);
    }

    return *size;
}

int sharp_Unloader(const char *name)
{
    return 0;
}
