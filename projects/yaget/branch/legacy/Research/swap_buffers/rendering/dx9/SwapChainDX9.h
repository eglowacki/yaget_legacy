//--------------------------------------------------------------------------------------------------------------------//
//                                                DX9 SWAP CHAIN CLASS                                                //
//                                                (MULTIPLE VIEWS)                                                    //
// 22/05/02, Mythos                                                                                                   //
// 31/07/03: DX9
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _MYTHOS_SWAPCHAINDX9_H_
#define   _MYTHOS_SWAPCHAINDX9_H_
#pragma    once

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#define   D3D_OVERLOADS
#include  <d3d9.h>

#include  "Rendering/SwapChain.h"

//----------------------------------------------- CLASSES ------------------------------------------------------------//

//--------------------------------------------------------------------------------------------------------------------//
//                                                CSwapChainDX9                                                       //
//--------------------------------------------------------------------------------------------------------------------//

namespace Mythos
{
  class CSwapChainDX9 : public CSwapChain
  {
  // public methods

  public:

    // constructors & destructor

                              CSwapChainDX9       (u32 u32Handle,bool boWindowed,bool boZBuffer,HWND hWnd,u32 u32Width,u32 u32Height);
    virtual                  ~CSwapChainDX9       (void);

    // get/set

    IDirect3DSwapChain9*      GetInterface        (void) const;
    void                      SetInterface        (IDirect3DSwapChain9* pInterface);
    IDirect3DSurface9*        GetBackBuffer       (void) const;
    void                      SetBackBuffer       (IDirect3DSurface9* pBackBuffer);
    IDirect3DSurface9*        GetZStencil         (void) const;
    void                      SetZStencil         (IDirect3DSurface9* pZStencil);

  // protected data

  protected:

    IDirect3DSwapChain9*      m_pInterface;
    IDirect3DSurface9*        m_pBackBuffer;
    IDirect3DSurface9*        m_pZStencil;
  };

  // smart ptr

  _SMART_POINTER_(CSwapChainDX9)
}

//----------------------------------------------- INLINES ------------------------------------------------------------//

#ifndef   _DEBUG
#include  "SwapChainDX9.inl"
#endif

//--------------------------------------------------------------------------------------------------------------------//

#endif // _MYTHOS_SWAPCHAINDX9_H_
