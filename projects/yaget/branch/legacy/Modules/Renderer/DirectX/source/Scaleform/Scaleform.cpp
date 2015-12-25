#include "Scaleform.h"
#include "GFxDrawText.h"
#include "GFxDrawText.h"
#include "GRendererD3D9.h"
#include "GFxRenderConfig.h"
#include "GFxPlayer.h"

#include "FxPlayerLog.h"
#include "GRendererD3D9.h"
#include "GFxRenderConfig.h"

namespace eg { namespace ui {

Scaleform::Scaleform()
{
    mLoader.SetLog(GPtr<GFxLog>(*new GFxPlayerLog()));
    GPtr<GFxFileOpener> pfileOpener = *new GFxFileOpener;
    mLoader.SetFileOpener(pfileOpener);
    mRenderer = *GRendererD3D9::CreateRenderer();
    mRenderConfig = *new GFxRenderConfig(mRenderer);
    mRenderConfig->SetRenderFlags(GFxRenderConfig::RF_EdgeAA);
    mLoader.SetRenderConfig(mRenderConfig);
}


Scaleform::~Scaleform()
{
}


void Scaleform::Resize(const math::Rect_t& rect)
{
    DWORD windowWidth = static_cast<DWORD>(rect.z - rect.x);
    DWORD windowHeight = static_cast<DWORD>(rect.w - rect.y);
    mMovie->SetViewport(windowWidth, windowHeight, 0, 0, windowWidth, windowHeight);
}


void Scaleform::Attach(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS *presentParams, const math::Rect_t& rect)
{
    bool result = mRenderer->SetDependentVideoMode(device, presentParams, GRendererD3D9::VMConfig_NoSceneCalls, 0);

    mMovieDef = *(mLoader.CreateMovie("P:/Yaget/branch/main/bin/ups_Debug/Window.swf", GFxLoader::LoadKeepBindData | GFxLoader::LoadWaitFrame1));
    mMovie = *mMovieDef->CreateInstance(true);
    mMovie->SetBackgroundAlpha(0.0f);
    Resize(rect);
}


void Scaleform::Render(float deltaTime)
{
    if (mMovie)
    {
        mMovie->Advance(deltaTime, 0);
        mMovie->Display();
    }
}

}} // namespace ui // namespace eg

