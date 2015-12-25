//--------------------------------------------------------------------------------------------------------------------//
//                                                DX9 RENDERER CLASS                                                  //
// 04/05/02, Mythos                                                                                                   //
// 31/07/03: DX9
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _MYTHOS_RENDERERDX9_H_
#define   _MYTHOS_RENDERERDX9_H_
#pragma    once

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#define   D3D_OVERLOADS
#include  <d3d9.h>
#include  <d3dx9core.h>
#include  <dxerr9.h>

#include  "Rendering/Renderer.h"

//----------------------------------------------- CLASSES ------------------------------------------------------------//

//--------------------------------------------------------------------------------------------------------------------//
//                                                CRendererDX9                                                        //
//--------------------------------------------------------------------------------------------------------------------//

namespace Mythos
{
  class CRendererDX9 : public CRenderer
  {
  // public methods

  public:

    // constructors & destructor

                              CRendererDX9        (void);
    virtual                  ~CRendererDX9        (void);

    // basic ops

    virtual bool              Create              (const HWND hWnd,const u32 u32Flags=_WINDOWED_|_ZBUFFER_,
                                                   const u32 u32Width=0,const u32 u32Height=0);
    virtual void              Destroy             (void);

    virtual u32               CreateSwapChain     (const HWND hWnd,const u32 u32Flags=_WINDOWED_|_ZBUFFER_,
                                                   const u32 u32Width=0,const u32 u32Height=0);
    virtual u32               ResizeSwapChain     (const u32 u32Chain,const u32 u32Width=0,const u32 u32Height=0);
    virtual void              ReleaseSwapChain    (const u32 u32Chain);
    virtual bool              SelectSwapChain     (const u32 u32Chain);

    virtual bool              BeginScene          (void);
    virtual bool              EndScene            (void);
    virtual bool              Clear               (const u32 u32Flags=IRenderer::_CLEAR_BACKDEPTH_);
    virtual bool              Swap                (const RECT* pSrcRect=NULL,const RECT* pDestRect=NULL,const u32 u32Vbls=0);

    // get

    virtual CStdString        GetSignature        (void) const;

    // draw

    virtual u32               DrawTeapot          (void);

    // trf

    virtual bool              SetModel2World      (const CMat4x4& m4Matrix);
    virtual bool              SetWorld2View       (const CMat4x4& m4Matrix);
    virtual bool              SetPerspective      (const float fFov,const float fNear,const float fFar,const float fRatio=4.f/3.f);
    virtual bool              SetOrtho            (const CVect3D& v3Min,const CVect3D& v3Max);

  // protected methods

    static void WINAPI        FillPlain           (D3DXVECTOR4* pOut,const D3DXVECTOR2* pTexCoord,const D3DXVECTOR2* pTexelSize,LPVOID pData);
    void                      InitStates          (void);

  // protected data

  protected:

    // DX9 access

    LPDIRECT3DDEVICE9         m_pDevice;
    LPDIRECT3D9               m_pD3D;

    // material

    D3DMATERIAL9              m_matCurrent;
    LPDIRECT3DTEXTURE9        m_pTexWhite;

    // test object

    LPD3DXMESH                m_pTeapot;
  };

  // smart ptr

  _SMART_POINTER_(CRendererDX9)
}

//----------------------------------------------- INLINES ------------------------------------------------------------//

#ifndef   _DEBUG
#include  "RendererDX9.inl"
#endif

//--------------------------------------------------------------------------------------------------------------------//

#endif // _MYTHOS_RENDERERDX9_H_
