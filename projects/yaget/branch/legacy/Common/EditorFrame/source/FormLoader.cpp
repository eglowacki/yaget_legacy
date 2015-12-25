#include "FormLoader.h"
#include "ManagedLoader.h"
#include "IEditorMessages.h"
#include "INetwork.h"
#include "Message/Dispatcher.h"
#include "File/AssetLoadSystem.h"
#include "File/VirtualFileSystem.h"
#include "Asset/AssetHandle.h"
#include "Asset/TextureAsset.h"


#include <vector>
#include <string>
#include <bitset>
#include <gcroot.h>
#using <System.dll>
#using <System.Drawing.dll>
#using <System.Windows.Forms.dll>
#using <System.Web.dll>
using namespace System;
using namespace System::IO;
using namespace System::Drawing;
using namespace System::Drawing::Imaging;
using namespace System::Collections::Generic;
using namespace System::Windows::Forms;
using namespace System::Reflection;
using namespace System::ComponentModel;
using namespace System::Runtime::CompilerServices;
using namespace System::Diagnostics;

#include "IPlugin.h"
#include "ObjectManager.h"
#include "Managed/Network.h"
#include "Managed/PropertyTypes.h"
#include "Managed/AssetLoadSystem.h"
#include "Managed/VirtualFileSystem.h"


namespace
{

    //-------------------------------------------------------------------------------------
    //! Interface between  MannagedLoader and this MC++
    //! This class is not manages by garbage collector, since it uses C++ allocators
    class MI : public eg::IManagedInterface
    {
    public:
        //-------------------------------------------------------------------------------------
        MI(eg::managed::IPlugin^ plugin) : mPlugin(plugin) {}

        //-------------------------------------------------------------------------------------
        virtual bool Init() {return mPlugin->Init();}

    private:
        gcroot<eg::managed::IPlugin^> mPlugin;
    };

    /*!
    This memory file is given to .NET code when it asks for attached stream
    On dispose it will dump it's content to C++ stream
    */
    public ref class ManagedMemoryFile : public MemoryStream
    {
    public:
        ManagedMemoryFile(String^ fileName) : mFileName(fileName)
        {
        }

        virtual ~ManagedMemoryFile()
        {
            eg::VirtualFileSystem& vfs = eg::registrate::ref_cast<eg::VirtualFileSystem>(_T("VirtualFileSystem"));
            if (eg::VirtualFileFactory::ostream_t ostream = vfs.AttachFileStream(SToS(mFileName)))
            {
                if (long streamSize = this->Length)
                {
                    // we got attached file, so let's copy our content to
                    array<unsigned char>^ buffer = this->GetBuffer();
                    pin_ptr<unsigned char> p = &buffer[0];
                    unsigned char *pSourceBuffer = p;
                    ostream->write((const char *)pSourceBuffer, static_cast<std::streamsize>(streamSize));
                }
            }
        }

    private:
        String^ mFileName;
    };


}  // namespace


namespace eg {
namespace managed {
namespace internal {

//-------------------------------------------------------------------------------------
// implementation of Property types query
Type^ PropertyImpl::GetPropType()
{
    if (mpProperty->IsType<bool>())
    {
        return bool::typeid;
    }
    else if (mpProperty->IsType<int>() || mpProperty->IsType<int32_t>())
    {
        return int::typeid;
    }
    else if (mpProperty->IsType<uint32_t>())
    {
        return unsigned int::typeid;
    }
    else if (mpProperty->IsType<float>())
    {
        return float::typeid;
    }
    else if (mpProperty->IsType<eg::Vector3>())
    {
        return Vector3::typeid;
    }
    else if (mpProperty->IsType<eg::Vector4>())
    {
        return Vector4::typeid;
    }
    else if (mpProperty->IsType<eg::Color3_Tag>())
    {
        return Color3_Tag::typeid;
    }
    else if (mpProperty->IsType<eg::Color4_Tag>())
    {
        return Color4_Tag::typeid;
    }
    else if (mpProperty->IsType<eg::CustomLabels_Tag>())
    {
        return CustomLabels_Tag::typeid;
    }
    else if (mpProperty->IsType<eg::Flags_Tag>())
    {
        return Flags_Tag::typeid;
    }
    else if (mpProperty->IsType<std::string>())
    {
        return String::typeid;
    }
    else if (mpProperty->IsType<eg::Enum_Tag>())
    {
        return Enum_Tag::typeid;
    }

    return nullptr;
}



//-------------------------------------------------------------------------------------
Type^ PropertyImpl::GetPropValue()
{
    if (mpProperty->IsValue<bool>())
    {
        return bool::typeid;
    }
    else if (mpProperty->IsValue<int>() || mpProperty->IsValue<int32_t>())
    {
        return int::typeid;
    }
    else if (mpProperty->IsValue<uint32_t>())
    {
        return unsigned int::typeid;
    }
    else if (mpProperty->IsValue<float>())
    {
        return float::typeid;
    }
    else if (mpProperty->IsValue<eg::Vector3>())
    {
        return Vector3::typeid;
    }
    else if (mpProperty->IsValue<eg::Vector4>())
    {
        return Vector4::typeid;
    }
    else if (mpProperty->IsType<std::string>())
    {
        return String::typeid;
    }

    return nullptr;
}


//-------------------------------------------------------------------------------------
String^ PropertyImpl::GetConverterName()
{
    String^ convertorName = "NotSupported";
    Type^ type = GetPropType();
    Type^ value = GetPropValue();
    if (type != nullptr && value != nullptr)
    {
        convertorName = type->Name + value->Name;
    }

    return convertorName;
}
//-------------------------------------------------------------------------------------

} // namespace internal

//-------------------------------------------------------------------------------------
void LoadPlugins(const std::vector<std::string>& pluginNameList)
{
    // first just create new instance of each requested plugin object
    // those plugin objects need to be derived from managed::IPlugin
    List<IPlugin^>^ pluginList = gcnew List<IPlugin^>();
    for (std::vector<std::string>::const_iterator it = pluginNameList.begin(); it != pluginNameList.end(); ++it)
    {
        String^ pathName = SToS(*it);
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
            Log::LogError(msg);
        }
    }

    // init all plugins
    for each (IPlugin^ plugin in pluginList)
    {
        try
        {
            // we create one plugin class for each managed panel
            MI *pPluginInterface = new MI(plugin);
            IMetPlugin *pTool = new ManagedLoader(SToS(plugin->Name).c_str(), pPluginInterface);
            eg::Message(eg::mtype::kNewToolPlugin, pTool).Send();
        }
        catch (Exception^ e)
        {
            String^ msg = "(Plugin) Exception " + e->ToString() + " initializing '" + plugin->Name + "' failed\n";
            Log::LogError(msg);
        }
    }

    // and then send message that all plugins are initialized
    eg::Message(eg::mtype::kAllToolPluginInit).Send();
    eg::Message(mtype::kShowPlugins).Send();
}


//-------------------------------------------------------------------------------------
//! Convert object id and type id into string format 'objectId.TypeId'
String^ ComponentInstanceId::ToString()
{
    String^ text = objectId.ToString() + "." + typeId.ToString();
    return text;
}


//-------------------------------------------------------------------------------------
String^ ComponentInstanceId::GetInstanceName()
{
    String^ name;
    name = "Name: '" + ObjectManager::Get->GetObjectName(objectId) + "', Type: '" + ObjectManager::Get->GetComponentName(typeId) + "'";
    return name;
}


//-------------------------------------------------------------------------------------
String^ ComponentInstanceId::GetTypeName()
{
    String^ name = ObjectManager::Get->GetComponentName(typeId);
    return name;
}


//-------------------------------------------------------------------------------------
String^ ComponentInstanceId::GetObjectName()
{
    String^ name = ObjectManager::Get->GetObjectName(objectId);
    return name;
}


void ComponentInstanceId::SendMessage(Message^ msg)
{
    if (Component^ componenet = ObjectManager::Get->FindComponentInstance(objectId, typeId))
    {
        componenet->HandleMessage(msg);
    }
}


//-------------------------------------------------------------------------------------
// copy data from C++ Message to .NET Message
Message^ Marshal(const eg::Message& msg)
{
    Object^ value = nullptr;
    switch (msg.mType)
    {
        case eg::mtype::kNewToolPlugin:
        case eg::mtype::kAllToolPluginInit:
            //value =
            break;

        case eg::mtype::kComponentNew:
        case eg::mtype::kComponentDelete:
        {
            eg::ComponentInstanceId_t id = msg.GetValue<eg::ComponentInstanceId_t>();
            ComponentInstanceId newId;
            newId.objectId = id.first.GetValue();
            newId.typeId = id.second.GetValue();
            value = newId;

            break;
        }

        case eg::mtype::kNewCommand:
            break;

        case eg::mtype::kGetChoiceLabels:
        {
            // handy typedef's
            typedef std::vector<std::string> StringVector_t;
            typedef std::pair<std::string, StringVector_t> Labels_t;

            // get value from message
            Labels_t labelTag = msg.GetValue<Labels_t>();

            // iterate trough all label names and add to it our List collection (MC++)
            List<String^>^ labelNames = gcnew List<String^>();
            for (StringVector_t::const_iterator it = labelTag.second.begin(); it != labelTag.second.end(); ++it)
            {
                labelNames->Add(SToS(*it));
            }

            System::Web::UI::Pair^ mcLabelTag = gcnew System::Web::UI::Pair(SToS(labelTag.first), labelNames);
            value = mcLabelTag;

            break;
        }

        case eg::mtype::kLoginInfo:
        {
            if (msg.Is<eg::network::UserInfo>())
            {
                eg::network::UserInfo userInfo = msg.GetValue<eg::network::UserInfo>();
                System::Web::UI::Pair^ mcUserInfo = gcnew System::Web::UI::Pair(SToS(userInfo.User), SToS(userInfo.Password));
                value = mcUserInfo;
            }

            break;
        }

        case eg::mtype::kWorldList:
        {
            typedef std::vector<eg::network::WorldInfo> WorldInfoList_t;

            if (msg.Is<WorldInfoList_t>())
            {
                WorldInfoList_t worldList = msg.GetValue<WorldInfoList_t>();

                List<WorldInfo^>^ worldInfoList = gcnew List<WorldInfo^>();
                for (WorldInfoList_t::const_iterator it = worldList.begin(); it != worldList.end(); ++it)
                {
                    WorldInfo^ world = gcnew WorldInfo();
                    world->Name = SToS((*it).Name);
                    worldInfoList->Add(world);
                }

                value = worldInfoList;
            }

            break;
        }

        case eg::mtype::kWorldSelected:
        {
            if (msg.Is<eg::network::WorldInfo>())
            {
                eg::network::WorldInfo worldInfo = msg.GetValue<eg::network::WorldInfo>();
                WorldInfo^ netWorldInfo = gcnew WorldInfo();
                netWorldInfo->Name = SToS(worldInfo.Name);

                value = netWorldInfo;
            }

            break;
        }

        case eg::mtype::kNetworkError:
        {
            if (msg.Is<std::string>())
            {
                std::string errorText = msg.GetValue<std::string>();
                value = SToS(errorText);
            }

            break;
        }
    }

    return gcnew Message(msg.mType, value);
}


//-------------------------------------------------------------------------------------
bool CanMarshal(Message^ msg)
{
    switch (msg->mType)
    {
        case eg::mtype::kNewToolPlugin:
        case eg::mtype::kAllToolPluginInit:
        case eg::mtype::kComponentNew:
        case eg::mtype::kComponentDelete:
        case eg::mtype::kNewCommand:
        case eg::mtype::kGetChoiceLabels:
        case eg::mtype::kLoginInfo:
        case eg::mtype::kWorldList:
        case eg::mtype::kWorldSelected:
        case eg::mtype::kNetworkError:
            return true;
    }

    return false;
}


//-------------------------------------------------------------------------------------
// copy data from .NET Message to C++ Message
eg::Message Marshal(Message^ msg)
{
    boost::any value;
    switch (msg->mType)
    {
        case eg::mtype::kNewToolPlugin:
        case eg::mtype::kAllToolPluginInit:
            //value =
            break;

        case eg::mtype::kComponentNew:
        case eg::mtype::kComponentDelete:
        {
            ComponentInstanceId id = (ComponentInstanceId)msg->mAnyData;
            eg::ComponentInstanceId_t newId;
            unsigned int v = id.objectId;
            newId.first = eg::ObjectId(v);
            v = id.typeId;
            newId.second = eg::ObjectId(v);
            value = newId;
            break;
        }

        case eg::mtype::kNewCommand:
            break;

        case eg::mtype::kGetChoiceLabels:
        {
            // handy typedef's
            typedef std::vector<std::string> StringVector_t;
            typedef std::pair<std::string, StringVector_t> Labels_t;

            System::Web::UI::Pair^ mcLabelTag = (System::Web::UI::Pair^)msg->mAnyData;

            Labels_t labelTag(SToS((String^)mcLabelTag->First) , StringVector_t());
            value = labelTag;

            break;
        }

        case eg::mtype::kLoginInfo:
        {
            System::Web::UI::Pair^ mcUserInfo = (System::Web::UI::Pair^)msg->mAnyData;
            eg::network::UserInfo userInfo;
            userInfo.User = SToS((String^)mcUserInfo->First);
            userInfo.Password = SToS((String^)mcUserInfo->Second);
            value = userInfo;

            break;
        }

        case eg::mtype::kWorldList:
        {
            typedef std::vector<eg::network::WorldInfo> WorldInfoList_t;
            WorldInfoList_t worldInfoData;
            List<WorldInfo^>^ worldInfoList = (List<WorldInfo^>^)msg->mAnyData;
            for each (WorldInfo^ worldInfo in worldInfoList)
            {
                eg::network::WorldInfo world;
                world.Name = SToS(worldInfo->Name);
                worldInfoData.push_back(world);
            }

            value = worldInfoData;
            break;
        }

        case eg::mtype::kWorldSelected:
        {
            WorldInfo^ newWorldInfo = (WorldInfo^)msg->mAnyData;
            eg::network::WorldInfo worldInfo;
            worldInfo.Name = SToS(newWorldInfo->Name);

            value = worldInfo;
            break;
        }

        case eg::mtype::kNetworkError:
        {
            String^ errorText = (String^)msg->mAnyData;
            value = SToS(errorText);

            break;
        }
    }

    return eg::Message(msg->mType, value);
}


//-------------------------------------------------------------------------------------
AssetLoadSystem::AssetLoadSystem() : mAssetList(gcnew Dictionary<String^, Object^>())
{
}


//-------------------------------------------------------------------------------------
generic <typename T>
Object^ AssetLoadSystem::GetAsset(String^ name, Load loadType)
{
    return GetAsset(name, loadType, T::typeid);
}


//-------------------------------------------------------------------------------------
//enum class Load { eLoadAsync, eLoadBlocking };
Object^ AssetLoadSystem::GetAsset(String^ name, Load loadType, Type^ assetType)
{
    if (mAssetList->ContainsKey(name))
    {
        return mAssetList[name];
    }

    if (assetType == Image::typeid || assetType == Bitmap::typeid)
    {
        typedef eg::asset::AssetHandle<eg::TextureAsset, eg::asset::AutoLock<eg::TextureAsset>, eg::asset::LoadTypeBlock> TextureAsset_t;
        TextureAsset_t textureAsset(SToS(name).c_str());
        if (eg::TextureAsset::Asset_t asset = textureAsset())
        {
            if (uint8_t *pData = asset->GetData())
            {
                uint32_t w = asset->GetWidth();
                uint32_t h = asset->GetHeight();
                uint32_t pixelSize = 3; // defualt to rgb
                eg::TextureAsset::PixelFormat pixelFormat = asset->GetFormat();
                PixelFormat bitmapFormat = PixelFormat::Format24bppRgb;

                switch (pixelFormat)
                {
                    case eg::TextureAsset::pfAlpha:
                    case eg::TextureAsset::pfLuminance:
                        bitmapFormat = PixelFormat::Alpha;
                        pixelSize = 1;
                        break;

                    case eg::TextureAsset::pfRGB:
                        bitmapFormat = PixelFormat::Format24bppRgb;
                        pixelSize = 3;
                        break;

                    case eg::TextureAsset::pfRGBA:
                        bitmapFormat = PixelFormat::Format32bppArgb;
                        pixelSize = 4;
                        break;
                }

                System::IntPtr pSourceData(pData);
                uint8_t *pConverted = (uint8_t *)pSourceData.ToPointer();

                // we have valid pixel data, let's create .NET Image object (in this case it will be Bitmap object)
                //Bitmap^ bitmap = gcnew Bitmap(w, h, w * pixelSize, bitmapFormat, ptrBuf);
                Bitmap^ bitmap = gcnew Bitmap(w, h, bitmapFormat);

                System::Drawing::Rectangle rect = System::Drawing::Rectangle(0, 0, bitmap->Width, bitmap->Height);
                System::Drawing::Imaging::BitmapData^ bmpData = bitmap->LockBits(rect, System::Drawing::Imaging::ImageLockMode::ReadWrite, bitmap->PixelFormat);

                IntPtr pDestData = bmpData->Scan0;
                int bytes = bitmap->Width * bitmap->Height * pixelSize;
                memcpy(pDestData.ToPointer(), pSourceData.ToPointer(), bytes);

                /*
                array<Byte>^rgbValues = gcnew array<Byte>(bytes);
                System::Runtime::InteropServices::Marshal::Copy(ptr, rgbValues, 0, bytes);
                // crappy way to copy pixel data from original (C++ texture asset) to
                // managed c++ Bitmap object
                if (pixelSize == 1)
                {
                    for (int counter = 0; counter < rgbValues->Length; counter += pixelSize)
                    {
                        rgbValues[counter] = pData[counter];
                    }
                }
                else if (pixelSize == 3)
                {
                    for (int counter = 0; counter < rgbValues->Length; counter += pixelSize)
                    {
                        rgbValues[counter] = pData[counter];
                        rgbValues[counter+1] = pData[counter+1];
                        rgbValues[counter+2] = pData[counter+2];
                    }
                }
                else if (pixelSize == 4)
                {
                    for (int counter = 0; counter < rgbValues->Length; counter += pixelSize)
                    {
                        rgbValues[counter] = pData[counter];
                        rgbValues[counter+1] = pData[counter+1];
                        rgbValues[counter+2] = pData[counter+2];
                        rgbValues[counter+3] = pData[counter+3];
                    }
                }

                System::Runtime::InteropServices::Marshal::Copy(rgbValues, 0, ptr, bytes);
                */
                bitmap->UnlockBits(bmpData);
                bitmap->RotateFlip(RotateFlipType::RotateNoneFlipY);

                mAssetList->Add(name, bitmap);
                return bitmap;
            }
        }
    }

    return nullptr;
}


//-------------------------------------------------------------------------------------
bool AssetLoadSystem::SaveAsset(String^ name)
{
    return false;
}


//-------------------------------------------------------------------------------------
List<String^>^ AssetLoadSystem::GetAssetList(String^ filter)
{
    eg::AssetLoadSystem& als = eg::registrate::ref_cast<eg::AssetLoadSystem>(_T("AssetLoadSystem"));
    std::vector<std::string> assetList = als.GetAssetList(SToS(filter));
    List<String^>^ managedList = gcnew List<String^>();

    for (std::vector<std::string>::const_iterator it = assetList.begin(); it != assetList.end(); ++it)
    {
        managedList->Add(SToS(*it));
    }

    return managedList;
}


//-------------------------------------------------------------------------------------
Stream^ VirtualFileSystem::GetFileStream(String^ name)
{
    eg::VirtualFileSystem& vfs = eg::registrate::ref_cast<eg::VirtualFileSystem>(_T("VirtualFileSystem"));
    if (VirtualFileFactory::istream_t istream = vfs.GetFileStream(SToS(name)))
    {
        // we got stream, we need to copy it's content into .NET memory stream
        size_t streamSize = eg::GetIStreamSize(*istream.get());
        MemoryStream^ memStream = gcnew MemoryStream(static_cast<int>(streamSize));
        try
        {
            array<unsigned char>^ buffer = memStream->GetBuffer();
            pin_ptr<unsigned char> p = &buffer[0];
            unsigned char *pDestBuffer = p;
            istream->read((char *)pDestBuffer, static_cast<std::streamsize>(streamSize));
            memStream->Write(buffer, 0, static_cast<int>(streamSize));
            memStream->Seek(0, SeekOrigin::Begin);

            return memStream;
        }
        catch (Exception^ e)
        {
            delete memStream;
            String^ msg = "VirtualFileSystem Exception " + e->ToString() + ". GetFileStream for '" + name + "' failed\n";
            Log::LogError(msg);
        }
    }

    return nullptr;
}


//-------------------------------------------------------------------------------------
Stream^ VirtualFileSystem::AttachFileStream(String^ name)
{
    eg::VirtualFileSystem& vfs = eg::registrate::ref_cast<eg::VirtualFileSystem>(_T("VirtualFileSystem"));
    if (vfs.CanAttachFileStream(SToS(name)))
    {
        ManagedMemoryFile^ memStream = gcnew ManagedMemoryFile(name);
        return memStream;
    }

    return nullptr;
}


//-------------------------------------------------------------------------------------
void Message::Send()
{
    if (CanMarshal(this))
    {
        Dispatcher::Get[mType](this);
    }
    else
    {
        Dispatcher::Get->SendMessageTo_NET(this);
    }
}


//-------------------------------------------------------------------------------------
String^ App::Name()
{
    return SToS(eg::registrate::GetAppName());
}


//-------------------------------------------------------------------------------------
Vector3::Vector3(float v)
{
    x = y = z = v;
}


Vector3::Vector3(String^ stringValue)
{
    stringValue = stringValue->Replace(",", " ");
    eg::Vector3 tempValue = eg::ConvertProperty<eg::Vector3>(SToS(stringValue).c_str());
    x = tempValue.x;
    y = tempValue.y;
    z = tempValue.z;
}


String^ Vector3::ToString()
{
    eg::Vector3 tempValue(x, y, z);
    std::string vectorValue = eg::ConvertProperty(tempValue);
    String^ stringValue = SToS(vectorValue);
    stringValue = stringValue->Trim();
    stringValue = stringValue->Replace(' ', ',');
    stringValue = stringValue->Replace(" ", "");
    stringValue = stringValue->Replace(",", ", ");

    return stringValue;
}
//-------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------
Vector4::Vector4(float v)
{
    x = y = z = w = v;
}


Vector4::Vector4(String^ stringValue)
{
    stringValue = stringValue->Replace(",", " ");
    eg::Vector4 tempValue = eg::ConvertProperty<eg::Vector4>(SToS(stringValue).c_str());
    x = tempValue.x;
    y = tempValue.y;
    z = tempValue.z;
    w = tempValue.w;
}


String^ Vector4::ToString()
{
    eg::Vector4 tempValue(x, y, z, w);
    std::string vectorValue = eg::ConvertProperty(tempValue);
    String^ stringValue = SToS(vectorValue);
    stringValue = stringValue->Trim();
    stringValue = stringValue->Replace(' ', ',');
    stringValue = stringValue->Replace(" ", "");
    stringValue = stringValue->Replace(",", ", ");

    return stringValue;
}
//-------------------------------------------------------------------------------------


String^ Flags_Tag::Convert(unsigned int value, List<String^>^ labels)
{
    std::bitset<32> bits(value);
    String^ textValue = "";
    if (labels == nullptr)
    {
        labels = gcnew List<String^>();
        for (int i = 1; i < 32; i++)
        {
            labels->Add(i.ToString());
        }
    }

    bool bNeedPeriod = false;
    for (int i = 0; i < labels->Count; i++)
    {
        if (bits.test(i))
        {
            if (bNeedPeriod)
            {
                textValue += ".";
            }
            bNeedPeriod = true;
            textValue += labels[i];
        }
    }

    return textValue;
}


unsigned int Flags_Tag::Convert(String^ textValue, List<String^>^ labels)
{
    std::bitset<32> bits(0);
    if (labels != nullptr)
    {
        for (int i = 0; i < labels->Count; i++)
        {
            if (textValue->Contains(labels[i]))
            {
                bits.set(i);
            }
        }
    }

    return bits.to_ulong();
}


bool Flags_Tag::IsOn(unsigned int value, int index)
{
    std::bitset<32> bits(value);
    return bits.test(index);
}


ObjectId ObjectManager::GetObjectId(String^ objectName)
{
    std::string name = SToS(objectName);
    eg::ObjectId id(name.c_str());
    return id.GetValue();
}


Hash ObjectManager::GetComponentId(String^ componentName)
{
    std::string name = SToS(componentName);
    eg::Hash hash(name.c_str());
    return (Hash)hash.GetValue();
}

} // namespace managed
} // namespace eg
