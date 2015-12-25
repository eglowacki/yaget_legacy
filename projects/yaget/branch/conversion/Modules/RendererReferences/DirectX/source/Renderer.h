///////////////////////////////////////////////////////////////////////
// Renderer.h
//
//  Copyright 10/8/2005 Edgar Glowacki.
//
//  Maintained by: Edgar
//
//  NOTES:
//      Implementation of a renderer class
//
//
//  #include "Renderer.h"
//
//////////////////////////////////////////////////////////////////////
//! \file

#ifndef RENDERER_H
#define RENDERER_H

#include "IRenderer.h"
#include "RenderModule.h"
#include "GeometryTextureAsset.h"
#include "GeometryAsset.h"
#include "MaterialAsset.h"
#include "ShaderAsset.h"
#include <vector>
#include <string>
#include <D3dx9core.h>


namespace eg
{
    // forward declarations
    namespace widgets
    {
        class LinePresenter;
        class TrianglePresenter;
    } // namespace widgets

    /*!
    Implementation of our render interface
    */
    class Renderer : public IRenderer
    {
        DECLARE_USER_EXPORTED_PLUGGABLE_CLASS(Renderer, DLLIMPEXP_RENDER)

    public:
        Renderer();
        virtual ~Renderer();

        virtual void Tick(float deltaTime);
        virtual const std::string& GetName() const;

        //! from IRenderer
        virtual void Reset();
        virtual bool BeginScene() const;
        virtual void EndScene() const;
        virtual void Flip() const;
        virtual ViewToken CreateViewToken(void *pWindow);
        virtual bool UseViewToken(ViewToken viewToken);
        virtual void DrawLine(const Vector3& begPos, const Vector3& endPos, const Color4_t& begColor, const Color4_t& endColor, const Matrix44 *pMatrix);
        virtual void Print(int32_t x, int32_t y, const std::string& text, const Color4_t& color);
        virtual void SetFont(const std::string& fontName, size_t size, bool bBold);
        virtual collision::Response GetHitObjects(uint64_t viewObjectId, const Vector2& mousePos, bool bGetAllObjects = false) const;
        virtual collision::Response GetHitObjects(const std::string& viewObjectName, const Vector2& mousePos, bool bGetAllObjects = false) const;

        // from IPluginObject
        virtual const Version GetVersion() const;

        static CComPtr<IDirect3DDevice9> mD3DDevice;

    private:
        // from IPluginObject
        virtual eInitReturnCode OnInit(int32_t initPass);

        //! respond to kShutdownBegin event
        void onShutdown(Message& msg);
        boost::signals::connection mShutdownConnection;

        //!{@ Helper methods
        //! Check to see if this is valid format for current device
        bool checkDeviceType(UINT adapter, D3DFORMAT displayFormat, D3DFORMAT backBuferFormat);
        //! render all queued up lines.
        void renderLines() const;
        //! render all queued up text.
        void renderText() const;
        //! Will load/create any assets which will be used as default one in case main one is missing
        //! This might be purple texture with "Missing Texture" text on it.
        //! Or even name of the missing texture asset.
        void setDefaultAssets();
        //! create and register all widget presenters
        void registerPresenters();
        //! }@

        /*!
        This will hold one segment of the line to draw
        */
        struct Line
        {
            Line(const Vector3& begPos, const Vector3& endPos, const Color4_t& begColor, const Color4_t& endColor, const Matrix44 *pMatrix) :
                mBegPos(begPos),
                mEndPos(endPos),
                mBegColor(begColor),
                mEndColor(endColor),
                mUseMatrix(false),
                mMatrix(m44::Identity())
            {
                if (pMatrix)
                {
                    mMatrix = *pMatrix;
                    mUseMatrix = true;
                    //mBegPos = (*pMatrix) * mBegPos;
                    //mEndPos = (*pMatrix) * mEndPos;
                }
            }

            Vector3 mBegPos;
            Vector3 mEndPos;
            Color4_t mBegColor;
            Color4_t mEndColor;
            Matrix44 mMatrix;
            bool mUseMatrix;
        };

        struct Text
        {
            Text(int32_t x, int32_t y, const std::string& text, const Color4_t& color)
            : mPosX(x)
            , mPosY(y)
            , mText(text)
            , mColor(color)
            {
            }

            int32_t mPosX;
            int32_t mPosY;
            std::string mText;
            Color4_t mColor;
        };

        // this is only used in drawing lines
        mutable std::vector<Line> mLines;
        mutable CComPtr<ID3DXLine> mLineRenderer;
        // this is only used in printing text lines
        mutable std::vector<Text> mText;
        mutable CComPtr<ID3DXFont> mTextRenderer;
        mutable CComPtr<ID3DXSprite> mTextSprite;

        /*!
        This represent one swap chain. This is used in multi
        window environment.
        */
        struct SwapChain
        {
            //! directX swap chain, which is represented by color buffer surface
            CComPtr<IDirect3DSwapChain9> mSwapChain;
            //! Depth and stencil surface used with this swap chain
            CComPtr<IDirect3DTexture9> mDepthStencilTexture;
            //! Which window uses this swap chain
            WXWidget mWidgetOwner;
        };

        //! Find swap chain which is used by widgetOwner.
        //! If -1 then no swap chain, otherwise index into mViewTokens vector
        int32_t FindSwapChain(const WXWidget& widgetOwner) const;
        //! Update already create SwapChain structure
        void updateSwapChain(SwapChain& swapChain);

        typedef std::vector<SwapChain> SwapChains_t;
        SwapChains_t mViewTokens;
        int32_t mCurrViewToken;
        CComPtr<IDirect3DSurface9> mMainDepthSurface;
        CComPtr<IDirect3DSurface9> mMainRenderSurface;

        boost::shared_ptr<widgets::LinePresenter> mLinePresenter;
        boost::shared_ptr<widgets::TrianglePresenter> mTrianglePresenter;

        //! Used for missing assets and resources
        typedef asset::AssetHandle<GeometryTextureAsset, asset::AutoLock<GeometryTextureAsset>, asset::LoadTypeBlock> GeometryTextureAsset_t;
        GeometryTextureAsset_t mMissingTexture;

        typedef asset::AssetHandle<GeometryAsset, asset::AutoLock<GeometryAsset>, asset::LoadTypeBlock> GeometryAsset_t;
        GeometryAsset_t mMissingGeometry;

        typedef asset::AssetHandle<MaterialAsset, asset::AutoLock<MaterialAsset>, asset::LoadTypeBlock> MaterialAsset_t;
        MaterialAsset_t mMissingMaterial;

        typedef asset::AssetHandle<ShaderAsset, asset::AutoLock<ShaderAsset>, asset::LoadTypeBlock> ShaderAsset_t;
        ShaderAsset_t mMissingShader;

        struct NewFont
        {
            NewFont(const std::string& name, size_t size, bool bold) : Name(name), Size(size), bBold(bold), bCreate(true) {}
            std::string Name;
            size_t Size;
            bool bBold;
            bool bCreate;
        };

        mutable NewFont mNewFont;
    };

} // namespace eg

#endif // RENDERER_H


