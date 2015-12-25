/////////////////////////////////////////////////////////////////////////
// bind_renderer.cpp
//
//  Copyright 4/27/2009 Edgar Glowacki.
//
// NOTES:
//      Provides bindings for log output from lua
//
//
//
/////////////////////////////////////////////////////////////////////////
//! \file
//#include "IRendererPlugin.h"
#include "IRendererView.h"
#include "Logger/Log.h"
#include "App/AppWindow.h"
#include "Script/ScriptAsset.h"
#include "Script/ScriptErrorHandler.h"
#include "UIAsset.h"
#include "UISheet.h"
#include "UIText.h"
#include "../ShaderAsset.h"
#include "Shiny.h"
#include "../RendererPlugin.h"
#include <string>
#include <boost/bind.hpp>
#pragma warning(push)
#pragma warning (disable : 4100)  // '' : unreferenced formal parameter
    #include <luabind/luabind.hpp>
#pragma warning(pop)
//#include <luabind/iterator_policy.hpp>
#include "Script/ref_iterator_policy.hpp"


namespace
{
    using namespace eg;

    const int kNumTries = 5;

    class View : public IRendererView
    {
        // no copy semantics
        View(const View&);
        View& operator=(const View&);

    public:
        View(const luabind::object &lua_handle)
        : mRenderer(script::get_ref<RendererPlugin>(lua_handle.interpreter(), "renderer"))
        , mUiAsset(script::get_ref<AssetLoadSystem>(lua_handle.interpreter(), "als"))
        , mFontAsset(script::get_ref<AssetLoadSystem>(lua_handle.interpreter(), "als"))
        , mDispatcher(script::get_ref<Dispatcher>(lua_handle.interpreter(), "dsp"))
        , mViewToken((IRendererPlugin::view_t)-1)
        , mErrorCounter(kNumTries)
        , mWinHandle(luabind::object_cast<long>(lua_handle))
        , mTextToken((uint32_t)-1)
        {
            app::handle_t win = app::to_handle(mWinHandle);
            mViewToken = mRenderer.NewView(win);

            std::string fontName("fonts_en");
            try
            {
                mFontAsset = fontName;
            }
            catch (const ex::resource& ex)
            {
                log_error << "Could not get FontAsset: '" << fontName << "', Exception: '" << ex.what() << "'";
            }
            mTickTextConnection = mDispatcher[mtype::kFrameTick].connect(boost::bind(&View::onTickText, this, _1));
            mTickTextConnection.block();
        }

        virtual ~View()
        {
            Close();
        }

        void Close()
        {
            mTickUIConnection.disconnect();
            mInputConnection.disconnect();
            mTickTextConnection.disconnect();
            if (view::isValid(mViewToken))
            {
                mRenderer.DestroyView(mViewToken);
                mViewToken = (IRendererPlugin::view_t)-1;
            }

            mUiSheet.reset();
            mUIText.reset();
            mUiAsset = "";
            mFontAsset = "";
        }

        Vector4 GetSize() const
        {
            math::Rect_t viewSize = mRenderer.GetViewSize(mViewToken);
            return viewSize;
        }

        void Resize()
        {
            if (view::isValid(mViewToken))
            {
                math::Rect_t rect = mRenderer.ResizeView(mViewToken);
                if (mUiSheet)
                {
                    mUiSheet->Resize(rect);
                }
                if (mUIText)
                {
                    mUIText->Resize(rect);
                }
            }
        }

        void set_ui(const std::string& name)
        {
            try
            {
                mUiAsset = name;
                mUiSheet = mUiAsset->GetSheet();
                if (mUiSheet)
                {
                    Resize();

                    mTickUIConnection = mDispatcher[mtype::kFrameTick].connect(boost::bind(&View::onTickUI, this, _1));
                    mInputConnection = mDispatcher[mtype::kInput].connect(boost::bind(&View::onInput, this, _1));
                }
                else
                {
                    mTickUIConnection.disconnect();
                    mInputConnection.disconnect();
                }
            }
            catch (const ex::resource& ex)
            {
                mUiSheet.reset();
                mTickUIConnection.disconnect();
                mInputConnection.disconnect();
                log_error << "Could not get UIAsset: '" << name << "', Exception: '" << ex.what() << "'";
            }
        }

        const std::string& get_ui() const
        {
            return mUiAsset.name();
        }

        void Print(int32_t x, int32_t y, const Color4_t& color, const std::string& text)
        {
            if (mFontAsset)
            {
                if (!mUIText)
                {
                    if (!(mUIText = mFontAsset->GetText()))
                    {
                        //return yaget::name_id(mAsset.get());
                        log_error << "Could not get UIText object for '" << text << "' from Asset " << yaget::name_id(mFontAsset.get()) << ".";
                        return;
                    }

                    UIText::Plate& plate = mUIText->GetPlate(mTextToken);
                    mTextToken = plate.Token();
                    Resize();
                }

                UIText::Plate& plate = mUIText->GetPlate(mTextToken);
                plate.Print(x, y, color, text);
                if (!text.empty() && mTickTextConnection.blocked())
                {
                    mTickTextConnection.unblock();
                }
                else if (text.empty() && !mTickTextConnection.blocked())
                {
                    mTickTextConnection.block();
                }
            }
        }

        void Render(uint32_t clearFlags, const Color4_t& clearColor, const luabind::object &callback)
        {
            mRenderer.RenderView(mViewToken, clearFlags, clearColor, boost::bind(&View::onRender, this, callback));
        }

    private:
        void onTickUI(const Message& msg)
        {
            assert(mUiSheet);
            float delta = static_cast<float>(msg);
            mUiSheet->Tick(delta);
        }

        void onTickText(const Message& msg)
        {
            assert(mUIText);
            float delta = static_cast<float>(msg);
            mUIText->Tick(delta);
        }

        void onInput(const Message& msg)
        {
            assert(mUiSheet);
            const InputManager::Input *input = msg.GetValue<const InputManager::Input *>();
            if (mWinHandle == input->mUser)
            {
                mUiSheet->HandleMessage(msg);
            }
        }

        void onRender(const luabind::object &callback)
        {
            if (mErrorCounter && luabind::type(callback) == LUA_TFUNCTION)
            {
                try
                {
                    luabind::call_function<void>(callback);
                    mErrorCounter = kNumTries;
                }
                catch (luabind::error& e)
                {
                    //mErrorCounter--;
                    lua_State *L = e.state();

                    script::luaError(L, false);
                    luabind::object error_msg(luabind::from_stack(L, -1));
                    log_error << "Script runtime error, tries left(" << mErrorCounter << "):\n" << error_msg << ">";
                    lua_pop(L, 1);
                }
            }

            if (mUiSheet)
            {
                mUiSheet->Render();
            }
            if (mUIText)
            {
                mUIText->Render();
            }
        }

        RendererPlugin& mRenderer;
        IRendererPlugin::view_t mViewToken;
        Dispatcher& mDispatcher;
        int mErrorCounter;
        hAsset<UIAsset> mUiAsset;
        hAsset<UIAsset> mFontAsset;
        UISheet_t mUiSheet;
        UIText_t mUIText;
        uint32_t mTextToken;
        boost::signals::connection mTickUIConnection;
        boost::signals::connection mInputConnection;
        boost::signals::connection mTickTextConnection;
        long mWinHandle;
    };


    typedef std::vector<Vector3> Vectors3_t;
    struct LuaVectors3
    {
        Vectors3_t Vectors3;
        size_t Size() const {return Vectors3.size();}
    };

    class Geometry
    {
    public:
        typedef Vector3 Vertex_t;
        Geometry(const luabind::object &callback, const luabind::object &decTypes)
        : mRenderer(script::get_ref<RendererPlugin>(callback.interpreter(), "renderer"))
        , mCallback(callback)
        , mBufferSize(0)
        , mErrorCounter(kNumTries)
        {
            using namespace boost::algorithm;
            namespace lb = luabind;
            Declarations_t declarations;

            if (lb::type(decTypes) == LUA_TTABLE)
            {
                for (lb::iterator i(decTypes), end; i != end; ++i)
                {
                    try
                    {
                        std::string streamType = luabind::object_cast<std::string>(*i);
                        if (iequals(streamType, "position"))
                        {
                            addStreamType(0, stPosition, declarations);
                        }
                        else if (iequals(streamType, "color"))
                        {
                            addStreamType(0, stColor, declarations);
                        }
                        else if (iequals(streamType, "normal"))
                        {
                            addStreamType(0, stNormal, declarations);
                        }
                        else if (iequals(streamType, "uv"))
                        {
                            addStreamType(0, stUV, declarations);
                        }
                        else if (iequals(streamType, "custom"))
                        {
                            addStreamType(0, stCustom, declarations);
                        }
                        else
                        {
                            log_error << "Invalid streamType '" << streamType << "' specified to Geometry constructor.";
                            luaL_error(callback.interpreter(), "Invalid streamType '%s' specified to Geometry constructor.", streamType.c_str());
                        }
                    }
                    catch (lb::cast_failed& ex)
                    {
                        log_error << "Invalid parameter, specified to Geometry constructor. Exception: " << ex.what();
                        luaL_error(callback.interpreter(), "Invalid parameter, specified to Geometry constructor. Exception: %s", ex.what());
                    }
                }

                addStreamType(0, stEnd, declarations);
            }

            if (declarations.empty())
            {
                addStreamType(0, stPosition, declarations);
                addStreamType(0, stEnd, declarations);
            }

            mToken = mRenderer.Generate(boost::ref(*this), sizeof(Vertex_t), D3DPT_LINELIST, &declarations[0]);
        }

        void Generate()
        {
            mRenderer.Generate(mToken);
        }

        uint32_t Token() const {return mToken;}

        size_t operator()(uint8_t *data)
        {
            if (mErrorCounter)
            {
                try
                {
                    if (data)
                    {
                        LuaVectors3 positions;
                        positions.Vectors3.resize(mBufferSize/sizeof(Vertex_t));
                        /*mBufferSize =*/ luabind::call_function<size_t>(mCallback, boost::ref(positions));
                        std::copy(positions.Vectors3.begin(), positions.Vectors3.end(), (Vertex_t *)data);
                    }
                    else
                    {
                        size_t numVerticies = luabind::call_function<size_t>(mCallback);
                        mBufferSize = numVerticies * sizeof(Vertex_t);
                    }

                    mErrorCounter = kNumTries;
                    return mBufferSize;
                }
                catch (luabind::error& e)
                {
                    mErrorCounter--;
                    lua_State *L = e.state();

                    script::luaError(L, false);
                    luabind::object error_msg(luabind::from_stack(L, -1));
                    log_error << "Script runtime error, tries left(" << mErrorCounter << "):\n" << error_msg << ">";
                    lua_pop(L, 1);
                }
            }

            return 0;
        }

    private:
        typedef std::vector<D3DVERTEXELEMENT9> Declarations_t;
        enum StreamType {stEnd, stPosition, stColor, stNormal, stUV, stCustom};
        //! Add one requested D3DVERTEXELEMENT9 to declarations list based on stype
        void addStreamType(size_t streamId, StreamType stype, Declarations_t& declarations) const
        {
            D3DVERTEXELEMENT9 declaration;
            switch (stype)
            {
            case stPosition:
                {
                    D3DVERTEXELEMENT9 elem = {0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0};
                    declaration = elem;
                }
                break;
            case stColor:
                {
                    D3DVERTEXELEMENT9 elem = {0, 0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0};
                    declaration = elem;
                }
                break;
            case stNormal:
                {
                    D3DVERTEXELEMENT9 elem = {0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0};
                    declaration = elem;
                }
                break;
            case stUV:
                {
                    D3DVERTEXELEMENT9 elem = {0, 0, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0};
                    declaration = elem;
                }
                break;
            case stCustom:
                {
                    D3DVERTEXELEMENT9 elem = {0, 0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0};
                    declaration = elem;
                }
                break;
            default:
                {
                    D3DVERTEXELEMENT9 elem = D3DDECL_END();
                    declarations.push_back(elem);
                    return;
                }
                break;
            }

            declaration.Stream = static_cast<WORD>(streamId);
            declaration.Offset = static_cast<WORD>(getStreamSize(declarations));
            declarations.push_back(declaration);
        }

        //! This will return size of one element in bytes (vertex structure)
        size_t getStreamSize(const Declarations_t& declarations) const
        {
            size_t size = 0;
            BOOST_FOREACH(const D3DVERTEXELEMENT9& elem, declarations)
            {
                if (elem.Offset == 0)
                {
                    if (elem.Type == D3DDECLTYPE_FLOAT1)
                    {
                        size += sizeof(float);
                    }
                    else if (elem.Type == D3DDECLTYPE_FLOAT2)
                    {
                        size += sizeof(float)*2;
                    }
                    else if (elem.Type == D3DDECLTYPE_FLOAT3)
                    {
                        size += sizeof(float)*3;
                    }
                    else if (elem.Type == D3DDECLTYPE_FLOAT4)
                    {
                        size += sizeof(float)*4;
                    }
                    else
                    {
                        log_error << "There is no handler for D3DVERTEXELEMENT9 type: " << elem.Type;
                    }
                }
                else
                {
                    size += elem.Offset;
                }
            }

            return size;
        }

        RendererPlugin& mRenderer;
        luabind::object mCallback;
        uint32_t mToken;
        size_t mBufferSize;
        int mErrorCounter;
    };

    class Material
    {
    public:
        Material(const luabind::object &lua_name)
        : mRenderer(script::get_ref<RendererPlugin>(lua_name.interpreter(), "renderer"))
        , mShaderAsset(luabind::object_cast<std::string>(lua_name), script::get_ref<AssetLoadSystem>(lua_name.interpreter(), "als"))
        , GeometryToken((uint32_t)-1)
        {
            mMaterial = mShaderAsset->GetMaterial("");
        }

        Material(const luabind::object &lua_shader_name, const std::string& mat_name)
        : mRenderer(script::get_ref<RendererPlugin>(lua_shader_name.interpreter(), "renderer"))
        , mShaderAsset(luabind::object_cast<std::string>(lua_shader_name), script::get_ref<AssetLoadSystem>(lua_shader_name.interpreter(), "als"))
        , GeometryToken((uint32_t)-1)
        {
            mMaterial = mShaderAsset->GetMaterial(mat_name);
        }

        /*
#define LUA_TNIL        0
#define LUA_TBOOLEAN        1
#define LUA_TLIGHTUSERDATA  2
#define LUA_TNUMBER     3
#define LUA_TSTRING     4
#define LUA_TTABLE      5
#define LUA_TFUNCTION       6
#define LUA_TUSERDATA       7
#define LUA_TTHREAD     8
        */

        //! Exposes setting of arbitrary input to user value
        void SetInput(const std::string& name, const luabind::object &lua_value)
        {
            if (luabind::type(lua_value) == LUA_TBOOLEAN)
            {
                mMaterial->GetInput(name).set<bool>(luabind::object_cast<bool>(lua_value));
            }
            else if (luabind::type(lua_value) == LUA_TNUMBER)
            {
                mMaterial->GetInput(name).set<float>(luabind::object_cast<float>(lua_value));
            }
            else if (luabind::object_cast_nothrow<Vector3>(lua_value))
            {
                mMaterial->GetInput(name).set<Vector3>(luabind::object_cast<Vector3>(lua_value));
            }
            else if (luabind::object_cast_nothrow<Vector4>(lua_value))
            {
                mMaterial->GetInput(name).set<Vector4>(luabind::object_cast<Vector4>(lua_value));
            }
            else if (luabind::object_cast_nothrow<Matrix44>(lua_value))
            {
                mMaterial->GetInput(name).set<Matrix44>(luabind::object_cast<Matrix44>(lua_value));
            }
            else
            {
                using namespace boost;
                log_error << "Material: '" << mMaterial->Name() << "', input: '" << name << "' has invalid parameter as a value. Type: " << luabind::type(lua_value);
                // we should throw an exception here
                //throw ex::script_runtime(str(format("Material: '%1%', input: '%1%' has invalid parameter as a value. Type: %3%") % mMaterial->Name() % name % luabind::type(lua_value)).c_str());
                luaL_error(lua_value.interpreter(), str(format("Material: '%1%', input: '%1%' has invalid parameter as a value. Type: %3%") % mMaterial->Name() % name % luabind::type(lua_value)).c_str());
            }
        }

        void Render() const {mMaterial->Render(boost::ref(*this));}
        void operator()() const {mRenderer.Render(GeometryToken);}

        uint32_t GeometryToken;

    private:
        RendererPlugin& mRenderer;
        hAsset<ShaderAsset> mShaderAsset;
        shaders::Material_t mMaterial;
    };

} // namespace


//.def("token", &Geometry::Token),

namespace eg { namespace script {

void init_renderer(lua_State *L, RendererPlugin *renderer)
{
    using namespace luabind;
    module(L, "yaget")
    [
        class_<RendererPlugin>("RendererPlugin"),

        class_<View>("View")
            .def(constructor<const object&>())
            .def("render", &View::Render)
            .def("close", &View::Close)
            .def("resize", &View::Resize)
            .def("print", &View::Print)
            .def("size", &View::GetSize)
            .property("ui", &View::get_ui, &View::set_ui),

        class_<Geometry>("Geometry")
            .def(constructor<const object&, const object&>())
            .def("generate", &Geometry::Generate)
            .property("token", &Geometry::Token),

        class_<Material>("Material")
            .def(constructor<const object&>())
            .def(constructor<const object&, const std::string&>())
            .def("setInput", &Material::SetInput)
            .def("render", &Material::Render)
            .def_readwrite("geometry", &Material::GeometryToken)
    ];

    module(L, "math")
    [
        class_<LuaVectors3>("Vectors")
            .def_readwrite("ptr", &LuaVectors3::Vectors3, return_ref_stl_iterator)
            .property("size", &LuaVectors3::Size)
    ];

    object g = globals(L);
    object lua_yaget = gettable(g, "yaget");

    lua_yaget["renderer"] = renderer;
}

}} // namespace script // namespace eg


