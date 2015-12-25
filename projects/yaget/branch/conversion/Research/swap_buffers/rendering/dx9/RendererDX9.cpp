//--------------------------------------------------------------------------------------------------------------------//
//                                                DX9 RENDERER CLASS                                                  //
// 04/05/02, Mythos                                                                                                   //
// 31/07/03: DX9
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  "stdafx.h"
#include  "RendererDX9.h"
#ifdef    _DEBUG
#include  "RendererDX9.inl"
#endif

#include  "SwapChainDX9.h"
#include  "Maths/VectMat.h"

//--------------------------------------------------------------------------------------------------------------------//
//                                                BASIC OPS                                                           //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- Create -------------------------------------------------------------//
// create & init renderer
// 20/05/02, Mythos
// in : window handle,flags(=_WINDOWED_|_ZBUFFER_),width(=0) & height(=0)
// out: OK?
// rem: if width==height==0 the client area of the window is used
//--------------------------------------------------------------------------------------------------------------------//

bool Mythos::CRendererDX9::Create(const HWND hWnd,const u32 u32Flags,const u32 u32Width,const u32 u32Height)
  {
  Destroy();

  bool boWindowed =(u32Flags & _WINDOWED_) != 0;
  bool boZBuffer  =(u32Flags & _ZBUFFER_ ) != 0;
  u32  u32Width2  = u32Width;
  u32  u32Height2 = u32Height;

  if(boWindowed)
    {
    RECT rect;
    if(!GetClientRect(hWnd,&rect)) return false;
    if(!u32Width)  u32Width2  = rect.right;
    if(!u32Height) u32Height2 = rect.bottom;
    }

  //

  m_pD3D = Direct3DCreate9(D3D_SDK_VERSION);
  if(!m_pD3D) return false;

  D3DDISPLAYMODE d3ddm;
  if(FAILED(m_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT,&d3ddm))) return false;

  DWORD    dwVtxProc = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
  D3DCAPS9 d3dcaps;
  if(SUCCEEDED(m_pD3D->GetDeviceCaps(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,&d3dcaps)))
    {
    if(d3dcaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) dwVtxProc = D3DCREATE_HARDWARE_VERTEXPROCESSING;
    }

  D3DPRESENT_PARAMETERS d3dpp; 
  ZeroMemory(&d3dpp,sizeof(d3dpp));
  d3dpp.BackBufferWidth  = u32Width2;
  d3dpp.BackBufferHeight = u32Height2;
  d3dpp.BackBufferFormat = d3ddm.Format;
  d3dpp.BackBufferCount  = 1;
  d3dpp.MultiSampleType  = D3DMULTISAMPLE_NONE;
  d3dpp.SwapEffect       = D3DSWAPEFFECT_DISCARD;
  d3dpp.hDeviceWindow    = hWnd;
  d3dpp.Windowed         = boWindowed;
  d3dpp.EnableAutoDepthStencil = boZBuffer;
  d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
  d3dpp.Flags            = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;        // for screenshots

  HRESULT hrErr = m_pD3D->CreateDevice(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,hWnd,dwVtxProc,&d3dpp,&m_pDevice);
  if(FAILED(hrErr))
    {
    TCHAR* pszError = (TCHAR*)DXGetErrorString9(hrErr);
    _ASSERT_(false,"CreateDevice failed");
    if(pszError)
      {
      _ASSERT_(false,pszError);
      }
    return false;
    }

  D3DCAPS9 d3dDevDesc9;
  if(FAILED(m_pDevice->GetDeviceCaps(&d3dDevDesc9))) return false;

  //

  D3DVIEWPORT9 d3dViewport;
  d3dViewport.X      = 0;
  d3dViewport.Y      = 0;
  d3dViewport.Width  = u32Width2;
  d3dViewport.Height = u32Height2;
  d3dViewport.MinZ   = 0.f;
  d3dViewport.MaxZ   = 1.f;
  m_pDevice->SetViewport(&d3dViewport);

  D3DCOLORVALUE black_color = { 0.0f, 0.0f, 0.0f, 0.0f };
  D3DCOLORVALUE white_color = { 1.0f, 1.0f, 1.0f, 1.0f };
  ZeroMemory(&m_matCurrent,sizeof(m_matCurrent));
  m_matCurrent.Diffuse  = white_color;
  m_matCurrent.Ambient  = white_color;
  m_matCurrent.Specular = black_color;
  m_matCurrent.Emissive = black_color;
  m_matCurrent.Power    = 0.f;
  m_pDevice->SetMaterial(&m_matCurrent);

  InitStates();

  // trfs

  SetModel2World(Mythos::CMat4x4::Identity);
  SetWorld2View (Mythos::CMat4x4::Identity);
  SetPerspective(_DEG2RAD_(60.f),1.f,1000.f);

  // swap chain 0

  CSwapChainDX9* pSwapChain = new CSwapChainDX9(0,boWindowed,boZBuffer,hWnd,u32Width2,u32Height2);
  if(!pSwapChain) return false;
  AddSwapChain(pSwapChain);

  IDirect3DSurface9* pBackBuffer;
  IDirect3DSurface9* pZStencil;
  m_pDevice->GetBackBuffer(0,0,D3DBACKBUFFER_TYPE_MONO,&pBackBuffer);
  m_pDevice->GetDepthStencilSurface(&pZStencil);

  pSwapChain->SetInterface (NULL);
  pSwapChain->SetBackBuffer(pBackBuffer);
  pSwapChain->SetZStencil  (pZStencil);

  SelectSwapChain(0); 

  // hardcoded light

  CVect3D v3Dir(1.f,-1.f,-1.f);
  CVect4D v4Col(1.f,1.f,0.5f,1.f);
  float*  pfCol = (float*)&v4Col;

  D3DLIGHT9 d3dLight;
  d3dLight.Type         = D3DLIGHT_DIRECTIONAL;
  d3dLight.Diffuse      = *(D3DCOLORVALUE*)pfCol;
  d3dLight.Specular     = black_color;
  d3dLight.Ambient      = black_color;
  d3dLight.Direction    = *(D3DVECTOR*)&v3Dir;
  d3dLight.Range        = 1e6;                              // otherwise SetLight can fail :(

  hrErr = m_pDevice->SetLight   (0,&d3dLight);
  hrErr = m_pDevice->LightEnable(0,true);

  for(u32 L = 1; L < 8; L++) m_pDevice->LightEnable(L,false);

  // hardcoded texture

  hrErr = D3DXCreateTexture(m_pDevice,32,32,1,0,D3DFMT_R8G8B8,D3DPOOL_DEFAULT,&m_pTexWhite);
  if(FAILED(hrErr)) MessageBox(NULL,"D3DXCreateTexture failed","CRendererDX9::Create",MB_OK|MB_ICONEXCLAMATION);
  else
    {
    CVect4D FillColor(1.f);
    hrErr = D3DXFillTexture(m_pTexWhite,&CRendererDX9::FillPlain,(LPVOID)&FillColor);
    if(FAILED(hrErr)) MessageBox(NULL,"D3DXFillTexture failed","CRendererDX9::Create",MB_OK|MB_ICONEXCLAMATION);
    }

  // test object

  hrErr = D3DXCreateTeapot(m_pDevice,&m_pTeapot,NULL);
  if(FAILED(hrErr)) MessageBox(NULL,"D3DXCreateTeapot failed","CRendererDX9::Create",MB_OK|MB_ICONEXCLAMATION);

  //

  m_boInit = true;
  return true;
  }

//

void WINAPI Mythos::CRendererDX9::FillPlain(D3DXVECTOR4* pOut,const D3DXVECTOR2* pTexCoord,const D3DXVECTOR2* pTexelSize,LPVOID pData)
  {
  CVect4D* pColor = (CVect4D*)pData;
  if(pColor)
    {
    pOut->x = pColor->r;
    pOut->y = pColor->g;
    pOut->z = pColor->b;
    pOut->w = pColor->a;
    }

  UNREFERENCED_PARAMETER(pTexCoord);
  UNREFERENCED_PARAMETER(pTexelSize);
  }

//----------------------------------------------- InitStates ---------------------------------------------------------//
// init render & texture stages states
// 24/05/02, Mythos
// in :
// out:
//--------------------------------------------------------------------------------------------------------------------//

void Mythos::CRendererDX9::InitStates()
  {
  // render states

  float fZero = 0.f;
  float fOne  = 1.f;
  float f64   = 64.f;

  m_boLighted = true;
  m_boZTest   = true;
  m_boTexture = false;

  m_pDevice->SetRenderState(D3DRS_FILLMODE,                 D3DFILL_SOLID);
  m_pDevice->SetRenderState(D3DRS_SHADEMODE,                D3DSHADE_GOURAUD);
  m_pDevice->SetRenderState(D3DRS_CULLMODE,                 D3DCULL_CW);
  m_pDevice->SetRenderState(D3DRS_CLIPPING,                 true);

  m_pDevice->SetRenderState(D3DRS_ZENABLE,                  m_boZTest);
  m_pDevice->SetRenderState(D3DRS_ZWRITEENABLE,             true);
  m_pDevice->SetRenderState(D3DRS_ZFUNC,                    D3DCMP_LESSEQUAL);
  m_pDevice->SetRenderState(D3DRS_DEPTHBIAS,                0);

  m_pDevice->SetRenderState(D3DRS_SPECULARENABLE,           false);
  m_pDevice->SetRenderState(D3DRS_LIGHTING,                 m_boLighted);
  m_pDevice->SetRenderState(D3DRS_AMBIENT,                  D3DCOLOR_XRGB(128,128,128));
  m_pDevice->SetRenderState(D3DRS_COLORVERTEX,              false);
  m_pDevice->SetRenderState(D3DRS_LOCALVIEWER,              true);
  m_pDevice->SetRenderState(D3DRS_NORMALIZENORMALS,         false);

  // texture stages states

	m_pDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);

	m_pDevice->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pDevice->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pDevice->SetSamplerState(1, D3DSAMP_MIPFILTER, D3DTEXF_NONE);

  m_pDevice->SetTexture(0,NULL);
  m_pDevice->SetTexture(1,NULL);

  m_pDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
  m_pDevice->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);
  m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
  m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TEXTURE);
  m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);
  m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
  m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE);

  m_pDevice->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 1);
  m_pDevice->SetTextureStageState(1, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);
  m_pDevice->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_CURRENT);
  m_pDevice->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_TEXTURE);
  m_pDevice->SetTextureStageState(1, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);
  m_pDevice->SetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_CURRENT);
  m_pDevice->SetTextureStageState(1, D3DTSS_ALPHAARG2, D3DTA_TEXTURE);

  m_pDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
  m_pDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

  m_pDevice->SetTextureStageState(2, D3DTSS_COLOROP, D3DTOP_DISABLE);
  m_pDevice->SetTextureStageState(2, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
  }

//----------------------------------------------- Destroy ------------------------------------------------------------//
// destroy renderer
// 20/05/02, Mythos
// in :
// out:
//--------------------------------------------------------------------------------------------------------------------//

void Mythos::CRendererDX9::Destroy()
  {
  CRenderer::Destroy();

  if(m_pDevice) m_pDevice->Release();
  if(m_pD3D)    m_pD3D   ->Release();

  m_pDevice = NULL;
  m_pD3D    = NULL;

  if(m_pTexWhite) m_pTexWhite->Release();
  m_pTexWhite = NULL;

  if(m_pTeapot) m_pTeapot->Release();
  m_pTeapot = NULL;
  }

//----------------------------------------------- CreateSwapChain ----------------------------------------------------//
// create additional swap chain
// 21/05/02, Mythos
// in : window handle,flags(=_WINDOWED_|_ZBUFFER_),width(=0) & height(=0)
// out: chain handle, 0==error
// rem: if width==height==0 the client area of the window is used
//--------------------------------------------------------------------------------------------------------------------//

u32 Mythos::CRendererDX9::CreateSwapChain(const HWND hWnd,const u32 u32Flags,const u32 u32Width,const u32 u32Height)
  {
  bool boWindowed =(u32Flags & _WINDOWED_) != 0;
  bool boZBuffer  =(u32Flags & _ZBUFFER_ ) != 0;
  u32  u32Width2  = u32Width;
  u32  u32Height2 = u32Height;

  if(boWindowed)
    {
    RECT rect;
    if(!GetClientRect(hWnd,&rect)) return 0;
    if(!u32Width)  u32Width2  = rect.right;
    if(!u32Height) u32Height2 = rect.bottom;
    }

  if(!u32Width2 || !u32Height2) return 0;

  // back buffer

  if(!m_pD3D | !m_pDevice) return 0;

  D3DDISPLAYMODE d3ddm;
  if(FAILED(m_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT,&d3ddm))) return 0;

  D3DPRESENT_PARAMETERS d3dpp; 
  ZeroMemory(&d3dpp,sizeof(d3dpp));
  d3dpp.BackBufferWidth  = u32Width2;
  d3dpp.BackBufferHeight = u32Height2;
  d3dpp.BackBufferFormat = d3ddm.Format;
  d3dpp.BackBufferCount  = 1;
  d3dpp.MultiSampleType  = D3DMULTISAMPLE_NONE;
  d3dpp.SwapEffect       = D3DSWAPEFFECT_DISCARD;
  d3dpp.hDeviceWindow    = hWnd;
  d3dpp.Windowed         = boWindowed;
  d3dpp.EnableAutoDepthStencil = false;
  d3dpp.Flags            = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;

  IDirect3DSwapChain9* pInterface;
  HRESULT hrErr = m_pDevice->CreateAdditionalSwapChain(&d3dpp,&pInterface);
  if(FAILED(hrErr))
    {
    TCHAR* pszError = (TCHAR*)DXGetErrorString9(hrErr);
    _ASSERT_(false,"CreateAdditionalSwapChain failed");
    if(pszError)
      {
      _ASSERT_(false,pszError);
      }
    return 0;
    }

  // Z & stencil

  IDirect3DSurface9* pZStencil = NULL;

  if(boZBuffer)
    {
    hrErr = m_pDevice->CreateDepthStencilSurface(u32Width2,u32Height2,D3DFMT_D16,D3DMULTISAMPLE_NONE,0,FALSE,&pZStencil,NULL);
    if(FAILED(hrErr))
      {
      TCHAR* pszError = (TCHAR*)DXGetErrorString9(hrErr);
      _ASSERT_(false,"CreateDepthStencilSurface failed");
      if(pszError)
        {
        _ASSERT_(false,pszError);
        }
      pInterface->Release();
      return 0;
      }
    }

  //

  CSwapChainDX9* pSwapChain = new CSwapChainDX9(CSwapChain::GetFreeHandle(),boWindowed,boZBuffer,hWnd,u32Width2,u32Height2);
  if(!pSwapChain)
    {
    pInterface->Release();
    if(pZStencil) pZStencil->Release();
    return 0;
    }

  IDirect3DSurface9* pBackBuffer;
  pInterface->GetBackBuffer(0,D3DBACKBUFFER_TYPE_MONO,&pBackBuffer);

  AddSwapChain(pSwapChain);
  pSwapChain->SetInterface (pInterface);
  pSwapChain->SetBackBuffer(pBackBuffer);
  pSwapChain->SetZStencil  (pZStencil);

  return pSwapChain->GetHandle();
  }

//----------------------------------------------- ResizeSwapChain ----------------------------------------------------//
// resize additional swap chain buffers
// 13/05/03, Mythos
// in : swap chain,new width & height
// out: chain handle, 0==error
// rem: if width==height==0 the client area of the window is used
//--------------------------------------------------------------------------------------------------------------------//

u32 Mythos::CRendererDX9::ResizeSwapChain(const u32 u32Chain,const u32 u32Width,const u32 u32Height)
  {
  CSwapChainDX9* pSwapChain = reinterpret_cast<CSwapChainDX9*>(GetSwapChain(u32Chain));
  if(!pSwapChain) return 0;

  HWND hWnd       = pSwapChain->GetHwnd();
  bool boWindowed = pSwapChain->IsWindowed();
  bool boZBuffer  = pSwapChain->HasZBuffer();
  u32  u32OldW    = pSwapChain->GetWidth();
  u32  u32OldH    = pSwapChain->GetHeight();
  u32  u32Width2  = u32Width;
  u32  u32Height2 = u32Height;

  if((u32OldW == u32Width) && (u32OldH == u32Height)) return u32Chain;
                                                            // no change
  if(boWindowed)
    {
    RECT rect;
    if(!GetClientRect(hWnd,&rect)) return 0;
    if(!u32Width)  u32Width2  = rect.right;
    if(!u32Height) u32Height2 = rect.bottom;
    }

  if(!u32Width2 || !u32Height2) return 0;

  // back buffer

  if(!m_pD3D | !m_pDevice) return 0;

  D3DDISPLAYMODE d3ddm;
  if(FAILED(m_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT,&d3ddm))) return 0;

  D3DPRESENT_PARAMETERS d3dpp; 
  ZeroMemory(&d3dpp,sizeof(d3dpp));
  d3dpp.BackBufferWidth  = u32Width2;
  d3dpp.BackBufferHeight = u32Height2;
  d3dpp.BackBufferFormat = d3ddm.Format;
  d3dpp.BackBufferCount  = 1;
  d3dpp.MultiSampleType  = D3DMULTISAMPLE_NONE;
  d3dpp.SwapEffect       = D3DSWAPEFFECT_DISCARD;
  d3dpp.hDeviceWindow    = hWnd;
  d3dpp.Windowed         = boWindowed;
  d3dpp.EnableAutoDepthStencil = false;
  d3dpp.Flags            = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;

  IDirect3DSwapChain9* pInterface;
  HRESULT hrErr = m_pDevice->CreateAdditionalSwapChain(&d3dpp,&pInterface);
  if(FAILED(hrErr))
    {
    TCHAR* pszError = (TCHAR*)DXGetErrorString9(hrErr);
    _ASSERT_(false,"CreateAdditionalSwapChain failed");
    if(pszError)
      {
      _ASSERT_(false,pszError);
      }
    return 0;
    }

  // Z & stencil

  IDirect3DSurface9* pZStencil = NULL;

  if(boZBuffer)
    {
    hrErr = m_pDevice->CreateDepthStencilSurface(u32Width2,u32Height2,D3DFMT_D16,D3DMULTISAMPLE_NONE,0,FALSE,&pZStencil,NULL);
    if(FAILED(hrErr))
      {
      TCHAR* pszError = (TCHAR*)DXGetErrorString9(hrErr);
      _ASSERT_(false,"CreateDepthStencilSurface failed");
      if(pszError)
        {
        _ASSERT_(false,pszError);
        }
      pInterface->Release();
      return 0;
      }
    }

  //

  CSwapChainDX9* pNewChain = new CSwapChainDX9(pSwapChain->GetHandle(),boWindowed,boZBuffer,hWnd,u32Width2,u32Height2);
  if(!pNewChain)
    {
    pInterface->Release();
    if(pZStencil) pZStencil->Release();
    return 0;
    }

  CVect3D v3ClearColor = pSwapChain->GetClearColor();
  ReleaseSwapChain(u32Chain);                               // destroy old chain
  pSwapChain = NULL;

  IDirect3DSurface9* pBackBuffer;
  pInterface->GetBackBuffer(0,D3DBACKBUFFER_TYPE_MONO,&pBackBuffer);

  AddSwapChain(pNewChain);                                  // replace by new one
  pNewChain->SetInterface (pInterface);
  pNewChain->SetBackBuffer(pBackBuffer);
  pNewChain->SetZStencil  (pZStencil);
  pNewChain->SetClearColor(v3ClearColor);

  return pNewChain->GetHandle();
  }

//----------------------------------------------- ReleaseSwapChain ---------------------------------------------------//
// release additional swap chain
// 21/05/02, Mythos
// in : chain handle
// out:
//--------------------------------------------------------------------------------------------------------------------//

void Mythos::CRendererDX9::ReleaseSwapChain(const u32 u32Chain)
  {
  CSwapChainDX9* pSwapChain = reinterpret_cast<CSwapChainDX9*>(GetSwapChain(u32Chain));
  if(!pSwapChain) return;

  RemoveSwapChain(pSwapChain);
  delete pSwapChain;
  }

//----------------------------------------------- SelectSwapChain ----------------------------------------------------//
// select as rendering target
// 21/05/02, Mythos
// in : chain handle
// out: OK?
//--------------------------------------------------------------------------------------------------------------------//

bool Mythos::CRendererDX9::SelectSwapChain(const u32 u32Chain)
  {
  if(!m_pDevice) return false;
  if((u32Chain == m_u32CurrentChain) && m_pCurrentChain) return true;

  CSwapChainDX9* pSwapChain = reinterpret_cast<CSwapChainDX9*>(GetSwapChain(u32Chain));
  if(!pSwapChain) return false;

  IDirect3DSurface9* pBackBuffer = pSwapChain->GetBackBuffer();
  IDirect3DSurface9* pZStencil   = pSwapChain->GetZStencil();
  if(!pBackBuffer) return false;

  if(FAILED(m_pDevice->SetRenderTarget   (0,pBackBuffer))) return false;
  if(FAILED(m_pDevice->SetDepthStencilSurface(pZStencil))) return false;

  m_pCurrentChain   = pSwapChain;
  m_u32CurrentChain = u32Chain;
  return true;
  }

//----------------------------------------------- BeginScene ---------------------------------------------------------//
// begin frame
// 20/05/02, Mythos
// in :
// out: OK?
//--------------------------------------------------------------------------------------------------------------------//

bool Mythos::CRendererDX9::BeginScene()
  {
  if(!m_pDevice) return false;
  return SUCCEEDED(m_pDevice->BeginScene());
  }

//----------------------------------------------- EndScene -----------------------------------------------------------//
// end frame
// 20/05/02, Mythos
// in :
// out: OK?
//--------------------------------------------------------------------------------------------------------------------//

bool Mythos::CRendererDX9::EndScene()
  {
  if(!m_pDevice) return false;
  return SUCCEEDED(m_pDevice->EndScene());
  }

//----------------------------------------------- Clear --------------------------------------------------------------//
// clear buffers
// 20/05/02, Mythos
// in : buffers to clear(=IRenderer::_CLEAR_BACKDEPTH_)
// out: OK?
//--------------------------------------------------------------------------------------------------------------------//

bool Mythos::CRendererDX9::Clear(const u32 u32Flags)
  {
  if(!m_pDevice) return false;
  
  u32 u32Buffers = 0;
  if(u32Flags & IRenderer::_CLEAR_BACK_)    u32Buffers |= D3DCLEAR_TARGET;
  if(u32Flags & IRenderer::_CLEAR_DEPTH_)   u32Buffers |= D3DCLEAR_ZBUFFER;
  if(u32Flags & IRenderer::_CLEAR_STENCIL_) u32Buffers |= D3DCLEAR_STENCIL;

  D3DCOLOR ClearColor = D3DCOLOR_XRGB(255,128,128);
  if(m_pCurrentChain)
    {
    CVect3D v3Color = m_pCurrentChain->GetClearColor();
    ClearColor = D3DCOLOR_XRGB(u8(255*v3Color.r),u8(255*v3Color.g),u8(255*v3Color.b));
    }

  HRESULT hrErr = m_pDevice->Clear(0,NULL,u32Buffers,ClearColor,1.f,128);
  if(SUCCEEDED(hrErr)) return true;

  TCHAR* pszError = (TCHAR*)DXGetErrorString9(hrErr);
  _ASSERT_(false,"Clear failed");
  if(pszError)
    {
    _ASSERT_(false,pszError);
    }
  return false;
  }

//----------------------------------------------- Swap ---------------------------------------------------------------//
// swap front & back buffers
// 20/05/02, Mythos
// in : source(=NULL) & dest(=NULL) rects,nb vbls(=0) to wait (TBI)
// out: OK?
//--------------------------------------------------------------------------------------------------------------------//

bool Mythos::CRendererDX9::Swap(const RECT* pSrcRect,const RECT* pDestRect,const u32 u32Vbls)
  {
  if(!m_pDevice) return false;
  if(!m_u32CurrentChain) return SUCCEEDED(m_pDevice->Present(pSrcRect,pDestRect,NULL,NULL));

  CSwapChainDX9* pSwapChain = reinterpret_cast<CSwapChainDX9*>(m_pCurrentChain);
  if(!pSwapChain) return false;
  IDirect3DSwapChain9* pInterface = pSwapChain->GetInterface();
  if(!pInterface) return false;
  return SUCCEEDED(pInterface->Present(pSrcRect,pDestRect,NULL,NULL,0));

  UNREFERENCED_PARAMETER(u32Vbls);
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                DRAW                                                                //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- DrawTeapot ---------------------------------------------------------//
// render teapot object
// 24/08/02, Mythos
// in :
// out: 0=OK
//--------------------------------------------------------------------------------------------------------------------//

u32 Mythos::CRendererDX9::DrawTeapot()
  {
  if(!m_pTeapot) return -1;

  if(FAILED(m_pTeapot->DrawSubset(0))) return -2;
  return 0;
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                TRF                                                                 //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- SetModel2World -----------------------------------------------------//
// set model->world trf
// 26/06/02, Mythos
// in : model->world matrix
// out: OK?
//--------------------------------------------------------------------------------------------------------------------//

bool Mythos::CRendererDX9::SetModel2World(const CMat4x4& m4Matrix)
  {
  if(!m_pDevice) return false;
  return SUCCEEDED(m_pDevice->SetTransform(D3DTS_WORLD,(D3DMATRIX*)&m4Matrix));
  }

//----------------------------------------------- SetWorld2View ------------------------------------------------------//
// set world->view trf
// 26/06/02, Mythos
// in : world->view matrix
// out: OK?
//--------------------------------------------------------------------------------------------------------------------//

bool Mythos::CRendererDX9::SetWorld2View(const CMat4x4& m4Matrix)
  {
  if(!m_pDevice) return false;
  return SUCCEEDED(m_pDevice->SetTransform(D3DTS_VIEW,(D3DMATRIX*)&m4Matrix));
  }

//----------------------------------------------- SetPerspective -----------------------------------------------------//
// set perspective projection
// 26/06/02, Mythos
// in : fov,near & far distances,screen ratio (=4/3)
// out: OK?
//--------------------------------------------------------------------------------------------------------------------//

bool Mythos::CRendererDX9::SetPerspective(const float fFov,const float fNear,const float fFar,const float fRatio)
  {
  if(!m_pDevice) return false;

  float fRight = (float)tan(fFov*0.5f)*fNear;
  float fTop   = fRight/fRatio;

  D3DXMATRIX m4Proj;
  D3DXMatrixPerspectiveRH(&m4Proj,fRight*2.f,fTop*2.f,fNear,fFar);
  return SUCCEEDED(m_pDevice->SetTransform(D3DTS_PROJECTION,(D3DMATRIX*)&m4Proj));
  }

//----------------------------------------------- SetOrtho -----------------------------------------------------------//
// set orthographic projection
// 26/06/02, Mythos
// in : volume extents
// out: OK?
//--------------------------------------------------------------------------------------------------------------------//

bool Mythos::CRendererDX9::SetOrtho(const CVect3D& v3Min,const CVect3D& v3Max)
  {
  if(!m_pDevice) return false;

  D3DXMATRIX m4Proj;
  D3DXMatrixOrthoOffCenterRH(&m4Proj,v3Min.x,v3Max.x,v3Min.y,v3Max.y,v3Min.z,v3Max.z);
  return SUCCEEDED(m_pDevice->SetTransform(D3DTS_PROJECTION,(D3DMATRIX*)&m4Proj));
  }
