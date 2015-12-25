//#include "Precompiled.h"
#include "UIAsset.h"
#include "File/AssetLoadSystem.h"
#include "Message/Dispatcher.h"
#include "Input/InputManager.h"
#include "Plugin/PluginManager.h"
#include "File/VirtualFileSystem.h"
#include "StringHelper.h"
#include "GFxLoader.h"
#include "GFxPlayer.h"
#include "FxPlayerLog.h"
#include "GRendererD3D9.h"
#include "GFxDrawText.h"

namespace eg {

namespace {

    class AssetUiStream : public GFile
    {
    public:

        const char *GetFilePath()       { return mFilePath.c_str(); }

        bool IsValid() { return mValid; }
        bool IsWritable() { return false; }

        bool Flush() { return true; }
        SInt GetErrorCode() { return 0; }

        SInt Tell() { return mFileIndex; }
        SInt64 LTell() { return (SInt64)mFileIndex; }

        SInt GetLength() { return mFileSize; }
        SInt64 LGetLength() { return (SInt64)mFileSize; }

        bool Close()
        {
            mValid = false;
            return false;
        }

        SInt CopyFromStream(GFile *pstream, SInt byteSize)
        {   GUNUSED2(pstream, byteSize);
            return 0;
        }

        SInt Write(const UByte *pbuffer, SInt numBytes)
        {   GUNUSED2(pbuffer, numBytes);
            return 0;
        }

        SInt Read(UByte *pbufer, SInt numBytes)
        {
            if (mFileIndex + numBytes > mFileSize)
            {
                numBytes = mFileSize - mFileIndex;
            }

            if (numBytes > 0)
            {
                mStream.read((char *)pbufer, numBytes);
                mFileIndex += numBytes;
            }

            return numBytes;
        }

        SInt SkipBytes(SInt numBytes)
        {
            if (mFileIndex + numBytes > mFileSize)
            {
                numBytes = mFileSize - mFileIndex;
            }

            Seek(numBytes, Seek_Cur);
            return numBytes;
        }

        SInt BytesAvailable()
        {
            return (mFileSize - mFileIndex);
        }

        SInt Seek(SInt offset, SInt origin = Seek_Set)
        {
            switch (origin)
            {
            case Seek_Set:
                mStream.seekg(offset);
                mFileIndex = offset;
                break;

            case Seek_Cur:
                mStream.seekg(offset, std::ios_base::cur);
                mFileIndex += offset;
                break;

            case Seek_End:
                mStream.seekg(offset, std::ios_base::end);
                mFileIndex = mFileSize - offset;
                break;
            }

            return mFileIndex;
        }

        SInt64 LSeek(SInt64 offset, SInt origin = Seek_Set)
        {
            return (SInt64)Seek((SInt) offset, origin);
        }

        bool ChangeSize(SInt /*newSize*/)
        {
            return false;
        }

    public:
        // pfileName should be encoded as UTF-8 to support international file names.
        AssetUiStream(std::istream& stream, const std::string& filePath)
        : mStream(stream)
        , mFilePath(filePath)
        , mFileSize(stream::size(stream))
        , mFileIndex(0)
        , mValid(mFileSize > 0)
        {
        }

    private:
        std::istream& mStream;
        std::string mFilePath;
        SInt mFileSize;
        SInt mFileIndex;
        bool mValid;
    };


    class UiFileOpener : public GFxFileOpenerBase
    {
    public:

        UiFileOpener() : GFxFileOpenerBase()
        {
        }

        virtual GFile *OpenFile(const char *purl, SInt /*flags = GFileConstants::Open_Read|GFileConstants::Open_Buffered*/, SInt /*mode = GFileConstants::Mode_ReadWrite*/)
        {
            VirtualFileFactory::istream_t stream = Streams[purl];
            Streams.erase(purl);

            if (stream)
            {
                AssetUiStream *assetUiStream = new AssetUiStream(*stream, purl);
                return assetUiStream;
            }
            return 0;
        }

        virtual SInt64 GetFileModifyTime(const char * /*purl*/)
        {
            return 0;
        }

        virtual GFile *OpenFileEx(const char *purl, GFxLog * plog, SInt flags = GFileConstants::Open_Read|GFileConstants::Open_Buffered, SInt mode = GFileConstants::Mode_ReadWrite)
        {
            if (GFile *pin = OpenFile(purl, flags, mode))
            {
                return pin;
            }

            if (plog)
            {
                plog->LogError("Error: GFxLoader failed to open '%s'\n", purl);
            }

            return 0;
        }

        std::map<std::string, VirtualFileFactory::istream_t> Streams;
    };



    class UIAssetFactory : public IAsset::Factory
    {
        UIAssetFactory(const UIAssetFactory&);
        UIAssetFactory& operator=(const UIAssetFactory&);

    public:
        UIAssetFactory(AssetLoadSystem& als)
        : mAls(als)
        , mLoadType(AssetLoadSystem::eLoadBlocking)
        {
            mLoader.SetLog(GPtr<GFxLog>(*new GFxPlayerLog()));
            mFileOpener = *new UiFileOpener;
            mLoader.SetFileOpener(mFileOpener);
            mRenderer = *GRendererD3D9::CreateRenderer();
            mRenderConfig = *new GFxRenderConfig(mRenderer);
            mRenderConfig->SetRenderFlags(GFxRenderConfig::RF_EdgeAA|GFxRenderConfig::RF_StrokeNormal);
            mLoader.SetRenderConfig(mRenderConfig);

            mAls.RegisterFactory(UIAsset::kType, boost::bind(&UIAssetFactory::New, this, _1, _2),
                                                 boost::bind(&UIAssetFactory::Load, this, _1, _2, _3),
                                                 boost::bind(&UIAssetFactory::Save, this, _1, _2, _3),
                                                 boost::bind(&UIAssetFactory::Query, this, _1, _2));

            log_trace(tr_ui) << "UIAssetFactory object created.";
        }

        ~UIAssetFactory()
        {
            mAls.UnregisterFactory(UIAsset::kType);
            log_trace(tr_ui) << "UIAssetFactory object deleted.";
        }

        bool Attach(IDirect3DDevice9* device, _D3DPRESENT_PARAMETERS *presentParams)
        {
            //D3DPRESENT_PARAMETERS *casted = reinterpret_cast<D3DPRESENT_PARAMETERS *>(presentParams);
            return mRenderer->SetDependentVideoMode(device, reinterpret_cast<D3DPRESENT_PARAMETERS *>(presentParams), GRendererD3D9::VMConfig_NoSceneCalls, 0);
        }

    private:
        IAsset *New(const std::string& name, uint64_t asssetId)
        {
            IAsset *pAsset = new UIAsset(asssetId, name, mRenderConfig.GetPtr());
            return pAsset;
        }

        void Load(AssetLoadSystem::IAsset_t asset, VirtualFileFactory::istream_t stream, const std::string& name)
        {
            boost::shared_ptr<UIAsset> uiAsset = boost::dynamic_pointer_cast<UIAsset>(asset);
            if (uiAsset)
            {
                if (stream && stream::size(*stream))
                {
                    if (WildCompareISafe("*.options.swf", name))
                    {
                        // we have options let's process it
                        //stream::stream_addapter<std::istream> astream(*stream);
                    }
                    else if (WildCompareISafe("*.swf", name))
                    {
                        mFileOpener->Streams[name] = stream;
                        uiAsset->mMovieDef = mLoader.CreateMovie(name.c_str(), GFxLoader::LoadKeepBindData | GFxLoader::LoadWaitFrame1);
                    }
                }
                else if (!stream)
                {
                }
            }
        }

        void Save(AssetLoadSystem::IAsset_t asset, VirtualFileFactory::ostream_t stream, const std::string& name)
        {
            name;
            // we can catch ex::serialize exception here if needed.
            // AssetLoadSystem does this already.
            boost::shared_ptr<UIAsset> scriptAsset = boost::dynamic_pointer_cast<UIAsset>(asset);
            if (scriptAsset && stream)
            {
                if (WildCompareISafe("*.options.swf", name))
                {
                    //stream::stream_addapter<std::ostream> astream(*stream);
                }
                else if (WildCompareISafe("*.swf", name))
                {
                    // we don't save any swf files
                }
            }
        }

        std::vector<std::string> Query(AssetLoadSystem::IAsset_t asset, const VirtualFileSystem& vfs)
        {
            vfs;
            const std::string& name = asset->Name();
            // here we control the order of iteration over streams for usage in Load(...) and Save(...) methods
            std::vector<std::string> files;
            std::string file = name;
            file += ".options.swf";
            files.push_back(file);
            file = name;
            file += ".swf";
            files.push_back(file);
            return files;
        }

        AssetLoadSystem& mAls;
        AssetLoadSystem::Load mLoadType;

        GFxSystem mSystem;
        GFxLoader mLoader;
        GPtr<GRendererD3D9> mRenderer;
        GPtr<GFxRenderConfig> mRenderConfig;
        GPtr<UiFileOpener> mFileOpener;
    };

} // namespace


IAsset::Token UIAsset::Register(AssetLoadSystem &als)
{
    Token factory(new UIAssetFactory(als));
    return factory;
}


bool UIAsset::Attach(Token token, IDirect3DDevice9* device, _D3DPRESENT_PARAMETERS *presentParams)
{
    if (UIAssetFactory *factory = dynamic_cast<UIAssetFactory *>(token.get()))
    {
        return factory->Attach(device, presentParams);
    }

    log_error << "Token used in RegisterBinds is not of UIAssetFactory type.";
    return false;
}


UIAsset::UIAsset(uint64_t id, const std::string& name, GFxRenderConfig *renderConfig)
: mId(id)
, mName(name)
, mFqn(name)
, mMovieDef(0)
, mRenderConfig(renderConfig)
{
}


UIAsset::~UIAsset()
{
    if (mMovieDef)
    {
        mMovieDef->Release();
    }
}


uint64_t UIAsset::GetId() const
{
    return mId;
}


const std::string& UIAsset::Name() const
{
    return mName;
}


const std::string& UIAsset::Fqn() const
{
    return mFqn;
}

IAsset::Locker_t UIAsset::Lock()
{
    return Locker_t();
}


guid_t UIAsset::Type() const
{
    return UIAsset::kType;
}


void UIAsset::HandleMessage(const Message& /*msg*/)
{
}


UISheet_t UIAsset::GetSheet() const
{
    if (mMovieDef)
    {
        GFxMovieView *movie = mMovieDef->CreateInstance(true);
        return UISheet_t(new UISheet(movie));
    }

    return UISheet_t();
}


UIText_t UIAsset::GetText() const
{
    if (mMovieDef)
    {
        GFxDrawTextManager *text = new GFxDrawTextManager(mMovieDef);
        text->SetRenderConfig(mRenderConfig);
        return UIText_t(new UIText(text));
    }

    return UIText_t();
}



} // namespace eg

